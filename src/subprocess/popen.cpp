#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdlib>

#include <algorithm>
#include <atomic>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <thread>
#include <tuple>

#include <libndgpp/error.hpp>

#include "stream_descriptors.hpp"
#include <liblinuxpp/file_descriptor.hpp>
#include <liblinuxpp/subprocess/popen.hpp>
#include <liblinuxpp/subprocess/wait.hpp>

std::mutex linuxpp::subprocess::popen::clone_mutex_ {};

struct subprocess_descriptor
{
    const std::string& executable;
    const std::vector<char *>& argv;
    linuxpp::subprocess::stream_descriptors stdin;
    linuxpp::subprocess::stream_descriptors stdout;
    linuxpp::subprocess::stream_descriptors stderr;
    std::atomic<int>* child_status;
    int (*exec)(const subprocess_descriptor&);
};

static int popen_execve(const subprocess_descriptor& policy)
{
    std::array <char*, 1> envp {nullptr};
    const int ret = ::execve(policy.executable.c_str(),
                             policy.argv.data(),
                             envp.data());

    if (ret == -1)
    {
        policy.child_status->store(errno, std::memory_order_release);
    }

    return 1;
}

static inline bool handle_child_stream(const int owned_fd,
                                       const int other_fd,
                                       const int new_fd,
                                       std::atomic<int>& child_status)
{
    /* This function ignores close errors for the following reasons:
     *
     * 1. Failing to close a file descriptor doesn't prevent the
     *    corresponding stream from functioning properly.
     *
     * 2. Closing the descriptor here is synonymous with closing a
     *    descriptor in a destructor which is not allowed to throw.
     */

    if (other_fd != owned_fd)
    {
        ::close(other_fd);
    }

    {
        const int ret = ::dup2(owned_fd, new_fd);
        if (ret == -1)
        {
            child_status.store(errno, std::memory_order_release);
            return false;
        }
    }

    ::close(owned_fd);

    return true;
}

static int clone_handler(void * subprocess_descriptorp)
{
    const subprocess_descriptor & policy = *static_cast<subprocess_descriptor*>(subprocess_descriptorp);

    if (policy.stdin)
    {
        if (handle_child_stream(policy.stdin.read_fd(),
                                policy.stdin.write_fd(),
                                0,
                                *policy.child_status) == false)
        {
            return 2;
        }
    }

    if (policy.stdout)
    {
        if (handle_child_stream(policy.stdout.write_fd(),
                                policy.stdout.read_fd(),
                                1,
                                *policy.child_status) == false)
        {
            return 3;
        }
    }

    if (policy.stderr)
    {
        if (handle_child_stream(policy.stderr.write_fd(),
                                policy.stderr.read_fd(),
                                2,
                                *policy.child_status) == false)
        {
            return 4;
        }
    }


    sigset_t sigset;
    sigfillset(&sigset);

    // Unblock all signals so the child process can decide what it
    // needs to block
    const int sigmask_errno = pthread_sigmask(SIG_UNBLOCK, &sigset, nullptr);
    if (sigmask_errno != 0)
    {
        policy.child_status->store(sigmask_errno, std::memory_order_release);
        return 5;
    }

    return policy.exec(policy);
}

static int handle_parent_stream(const int parent_fd,
                                const int child_fd) noexcept
{
    // Ignoring any close errors since the only alternative is to
    // throw in the constructor which would lead to a zombie process
    // since there's no other reference to the child process.  Also,
    // failing to close the child's file descriptor does not prevent
    // the stream from functioning properly.
    ::close(child_fd);

    if (parent_fd != child_fd)
    {
        return parent_fd;
    }
    else
    {
        return linuxpp::closed_fd;
    }
}

linuxpp::subprocess::popen::popen() noexcept = default;

linuxpp::subprocess::popen::popen(popen&&) noexcept = default;

linuxpp::subprocess::popen &
linuxpp::subprocess::popen::operator=(popen&&) noexcept = default;

linuxpp::subprocess::popen::popen(const std::string& executable,
                                  const linuxpp::subprocess::argv& argv,
                                  const linuxpp::subprocess::streams& streams)
{
    constexpr std::size_t stack_size = 1024 * 1024;
    std::unique_ptr<char []> child_stack {new char[stack_size]};
    std::atomic<int> child_status {0};

    std::vector<char*> cmdline (1U, const_cast<char*>(executable.c_str()));
    std::transform(argv.cbegin(), argv.cend(), std::back_inserter(cmdline),
                   [] (const std::unique_ptr<char []>& ptr) {
                       return ptr.get();
                   });
    cmdline.push_back(nullptr);

    std::unique_lock<std::mutex> lock(clone_mutex_);

    /// Open each stream's file descriptors behind the mutex to
    /// prevent leaking file descriptors to other threads creating a
    /// popen object.

    stream_descriptors stdin;
    stream_descriptors stdout;
    stream_descriptors stderr;

    if (streams.in.stream)
    {
        stdin = streams.in.stream->open();
    }

    if (streams.out.stream)
    {
        stdout = streams.out.stream->open();
    }

    if (streams.error.stream)
    {
        stderr = streams.error.stream->open();
    }

    subprocess_descriptor descriptor {executable,
                                      cmdline,
                                      stdin,
                                      stdout,
                                      stderr,
                                      &child_status,
                                      &popen_execve};

    const int ret = ::clone(&clone_handler,
                            child_stack.get() + (stack_size - 1),
                            CLONE_VM | CLONE_VFORK | SIGCHLD,
                            &descriptor);

    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "clone failed");
    }

    const int exec_status = child_status.load(std::memory_order_acquire);
    if (exec_status != 0)
    {
        // The clone_handler either failed to set up the streams, or
        // exec failed.  Call waitid to prevent zombie processes.
        try
        {
            siginfo_t siginfo = {};
            linuxpp::subprocess::waitid(P_PID,
                                        ret,
                                        siginfo,
                                        WEXITED);
        }
        catch (...)
        {
            // This is very pathological case.  For some reason, the
            // clone_handler failed, and the waitid system call
            // failed.  For now let's just try to throw the error
            // associated with clone_handler failing.
        }

        throw ndgpp_error(std::system_error,
                          std::error_code (exec_status, std::system_category()),
                          "subprocess execution failed");
    }

    if (stdin)
    {
        std::get<stdin_stream>(this->members_).reset(handle_parent_stream(stdin.write_fd(),
                                                                          stdin.read_fd()));
    }

    if (stdout)
    {
        std::get<stdout_stream>(this->members_).reset(handle_parent_stream(stdout.read_fd(),
                                                                           stdout.write_fd()));
    }

    if (stderr)
    {
        std::get<stderr_stream>(this->members_).reset(handle_parent_stream(stderr.read_fd(),
                                                                           stderr.write_fd()));
    }

    lock.unlock();

    std::get<child_pid>(this->members_).reset(ret);

    /* clone flags justifications:
     *
     * - CLONE_VM set.  This minimizes the overhead of starting a
     *   child process
     *
     * - CLONE_VFORK set.  This causes the calling thread to block
     *   until the child process calls exec or exits.  This is the
     *   behavior we want for the constructor because it guarantees
     *   that when we load from the atomic the child process has
     *   either written an error code to it or has succesfully
     *   executed the specified binary.
     *
     * - CLONE_FILES not set.  This allows the child process to
     *   receive a copy of the file descriptors which allows for pipes
     *   between the child process and the parent process.
     *
     * - CLONE_FS not set.  This allows the child process to get its
     *   own copy of file system info.
     *
     * - CLONE_IO not set.  This allows the child process to have its
     *   own IO context  (like fork)
     *
     * - CLONE_NEWIPC not set.  This creates the child process in the
     *   same IPC namespace as the parent (like fork)
     *
     * - CLONE_NEWNET not set.  This creates the child process in the
     *   same network namespace as the parent (like fork)
     *
     * - CLONE_NEWNS not set. This create the child process in the
     *   same mount namespace as the parent
     *
     * - CLONE_NEWPID not set. This create the child process in the
     *   same pid namespace as the parent (like fork)
     *
     * - CLONE_NEWUSER not set. This creates the child process in the
     *   same user namespace as the parent (like fork)
     *
     * - CLONE_NEWUTS not set. Creates the child in the same uts
     *   namespace as the parent (like fork)
     *
     * - CLONE_PARENT not set. Sets parent pid to the calling process
     *   (like fork)
     *
     * - CLONE_PARENT_SETID not set.  We don't care about this
     *
     * - CLONE_PID not set. We want the child process to have a different pid
     *
     * - CLONE_PTRACE not set.  Don't trace the child process if the
         parent is being traced
     *
     * - CLONE_SETTLS not set.  We don't need thread local storage
     *
     * - CLONE_SIGHAND not set.  We don't want to share signal handlers
     * - CLONE_STOPPED not set.  Do not initialy stop the child
     * - CLONE_SYSVSEM not set.  Do not share semaphore adjustments
     * - CLONE_THREAD not set.  We want a different thread group
     * - CLONE_UNTRACED not set.
     */
}

linuxpp::subprocess::popen::~popen()
{
    if (this->pid())
    {
        std::terminate();
    }
}

linuxpp::subprocess::status
linuxpp::subprocess::popen::wait(const int options)
{
    if (!this->pid())
    {
        /// subprocess has already been waited on
        return std::get<child_status>(this->members_);
    }

    siginfo_t siginfo = {};
    const bool child_exited = linuxpp::subprocess::waitid(P_PID,
                                                          std::get<child_pid>(this->members_).get(),
                                                          siginfo,
                                                          options);

    if (child_exited)
    {
        std::get<child_pid>(this->members_).release();
        std::get<child_status>(this->members_) = linuxpp::subprocess::status(siginfo);
    }

    return std::get<child_status>(this->members_);
}

linuxpp::subprocess::status linuxpp::subprocess::popen::wait()
{
    return this->wait(WEXITED);
}

linuxpp::subprocess::status linuxpp::subprocess::popen::poll()
{
    return this->wait(WEXITED | WNOHANG);
}

void linuxpp::subprocess::popen::signal(const int signal)
{
    if (!this->pid())
    {
        throw ndgpp_error(std::logic_error,
                          "child pid not set");
    }

    const int ret = ::kill(std::get<child_pid>(this->members_).get(), signal);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to send signal to child process");
    }
}

linuxpp::pid linuxpp::subprocess::popen::pid() const noexcept
{
    return std::get<child_pid>(this->members_);
}

linuxpp::subprocess::status linuxpp::subprocess::popen::status() const noexcept
{
    return std::get<child_status>(this->members_);
}

linuxpp::unique_fd<> & linuxpp::subprocess::popen::stdin() noexcept
{
    return std::get<stdin_stream>(this->members_);
}

linuxpp::unique_fd<> & linuxpp::subprocess::popen::stdout() noexcept
{
    return std::get<stdout_stream>(this->members_);
}

linuxpp::unique_fd<> & linuxpp::subprocess::popen::stderr() noexcept
{
    return std::get<stderr_stream>(this->members_);
}

const linuxpp::unique_fd<> & linuxpp::subprocess::popen::stdin() const noexcept
{
    return std::get<stdin_stream>(this->members_);
}

const linuxpp::unique_fd<> & linuxpp::subprocess::popen::stdout() const noexcept
{
    return std::get<stdout_stream>(this->members_);
}

const linuxpp::unique_fd<> & linuxpp::subprocess::popen::stderr() const noexcept
{
    return std::get<stderr_stream>(this->members_);
}

linuxpp::subprocess::popen::operator bool() const noexcept
{
    return static_cast<bool>(std::get<child_pid>(this->members_));
}
