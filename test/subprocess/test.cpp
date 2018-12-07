#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <system_error>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <libndgpp/error.hpp>

#include <liblinuxpp/epoll.hpp>
#include <liblinuxpp/read.hpp>
#include <liblinuxpp/subprocess/popen.hpp>
#include <liblinuxpp/write.hpp>
#include <liblinuxpp/pipe.hpp>

std::string test_path;

bool wait_for(linuxpp::subprocess::popen & bin, const std::chrono::seconds timeout)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);

    struct timespec timespec_timeout = {0, 0};

    const auto start = std::chrono::steady_clock::now();
    auto end = start;
    while (true)
    {
        const auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        if (elapsed_time >= timeout)
        {
            return false;
        }

        timespec_timeout.tv_sec = timeout.count() - elapsed_time.count();
        const int signal = ::sigtimedwait(&sigset, nullptr, &timespec_timeout);
        end = std::chrono::steady_clock::now();

        if (signal == -1)
        {
            return false;
        }

        if (signal != SIGCHLD)
        {
            continue;
        }

        const auto status = bin.poll();
        if (status)
        {
            return true;
        }
    }
}

TEST(ctor, bad_executable)
{
    const auto throws = [] () {
        linuxpp::subprocess::popen bin{"/dev/zero",
                                       {},
                                       {}};
    };

    EXPECT_THROW(throws(), ndgpp::error<std::system_error>);
}

TEST(ctor, default_ctor)
{
    linuxpp::subprocess::popen p{};

    EXPECT_FALSE(p.pid());
    EXPECT_FALSE(p);
}

TEST(vector_support, default_ctor)
{
    std::vector<linuxpp::subprocess::popen> processes(5U);
    ASSERT_EQ(5U, processes.size());

    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        processes[i] = linuxpp::subprocess::popen(test_path, {"--exit-code", i}, {});
    }

    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        linuxpp::subprocess::popen & proc = processes[i];
        ASSERT_TRUE(proc);

        const auto status = proc.wait();
        EXPECT_EQ(static_cast<int>(i), status.exit_code());
        EXPECT_FALSE(proc);
    }
}

TEST(member_tests, wait_exit_code_non_zero)
{
    linuxpp::subprocess::popen bin{test_path, {"--exit-code", 1},
                                   {}};

    const auto status = bin.wait();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_FALSE(status.dumped());
    EXPECT_EQ(1, status.exit_code());
}

TEST(member_tests, wait_exit_code_zero)
{
    linuxpp::subprocess::popen bin{test_path, {"--exit-code", 0},
                                   {}};

    const auto status = bin.wait();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_FALSE(status.dumped());
    EXPECT_EQ(0, status.exit_code());
}

TEST(member_tests, signal)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    // Block SIGUSR1 in the parent to confirm that the child can still
    // receive it.
    EXPECT_EQ(0, pthread_sigmask(SIG_BLOCK, &sigset, nullptr));

    linuxpp::subprocess::popen bin{test_path, {"--signal"},
                                   {}};
    bin.signal(SIGUSR1);
    const auto status = bin.wait();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_FALSE(status.called_exit());
    EXPECT_TRUE(status.signaled());
    EXPECT_EQ(SIGUSR1, status.signal());
}

TEST(member_tests, poll_signaled)
{
    {
        linuxpp::subprocess::popen bin{test_path, {"--signal"},
                                       {}};

        {
            const auto status = bin.poll();
            ASSERT_FALSE(static_cast<bool>(status));
            ASSERT_FALSE(status.exited());
            ASSERT_FALSE(status.called_exit());
            ASSERT_FALSE(status.signaled());
        }

        bin.signal(SIGKILL);
        ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

        const auto status = bin.status();
        EXPECT_TRUE(static_cast<bool>(status));
        EXPECT_TRUE(status.exited());
        EXPECT_FALSE(status.called_exit());
        EXPECT_TRUE(status.signaled());
        EXPECT_EQ(SIGKILL, status.signal());
    }
}

TEST(member_tests, poll_exit_code_zero)
{
    {
        linuxpp::subprocess::popen bin{test_path, {"--exit-code", 0},
                                       {}};

        ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

        const auto status = bin.status();
        EXPECT_TRUE(static_cast<bool>(status));
        EXPECT_TRUE(status.exited());
        EXPECT_TRUE(status.called_exit());
        EXPECT_FALSE(status.signaled());
        EXPECT_EQ(0, status.exit_code());
    }
}

class stream_test: public ::testing::Test
{
    protected:

    stream_test();

    linuxpp::epoll epoll;
    std::string stdin_value = "In in in as fast as you can, you can't catch me I'm the stinky cheese man";
    std::string stdout_value = "Out out out as fast as you can, you can't catch me I'm the stinky cheese man";
    std::string stderr_value = "Error error error as fast as you can, you can't catch me I'm the stinky cheese man";
    std::vector<char> stdout_buf;
    std::vector<char> stderr_buf;
};

stream_test::stream_test():
    stdout_buf(stdout_value.length()),
    stderr_buf(stderr_value.length())
{}

TEST_F(stream_test, stdin_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stdin", stdin_value},
                                   {linuxpp::subprocess::stdin{linuxpp::subprocess::pipe_stream{}}}};

    {
        const std::size_t ret = linuxpp::write(bin.stdin().get(), stdin_value.data(), stdin_value.length());
        ASSERT_EQ(stdin_value.length(), static_cast<std::size_t>(ret));
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

TEST_F(stream_test, stdin_stdout_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stdin", stdin_value, "--stdout", stdout_value},
                                   {linuxpp::subprocess::stdin{linuxpp::subprocess::pipe_stream{}},
                                    linuxpp::subprocess::stdout{linuxpp::subprocess::pipe_stream{}}}};

    {
        const std::size_t ret = linuxpp::write(bin.stdin().get(), stdin_value.data(), stdin_value.length());
        ASSERT_EQ(stdin_value.length(), static_cast<std::size_t>(ret));
    }

    {
        epoll.add(bin.stdout().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stdout().get(), stdout_buf.data(), stdout_buf.size());
        ASSERT_EQ(stdout_value.length(), length);
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

TEST_F(stream_test, stdin_stderr_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stdin", stdin_value, "--stderr", stderr_value},
                                   {linuxpp::subprocess::stdin{linuxpp::subprocess::pipe_stream{}},
                                    linuxpp::subprocess::stderr{linuxpp::subprocess::pipe_stream{}}}};

    {
        const std::size_t ret = linuxpp::write(bin.stdin().get(), stdin_value.data(), stdin_value.length());
        ASSERT_EQ(stdin_value.length(), static_cast<std::size_t>(ret));
    }

    {
        epoll.add(bin.stderr().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stderr().get(), stderr_buf.data(), stderr_buf.size());
        ASSERT_EQ(stderr_value.length(), length);
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

TEST_F(stream_test, stdin_stdout_stderr_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stdin", stdin_value, "--stdout", stdout_value, "--stderr", stderr_value},
                                   {linuxpp::subprocess::stdin{linuxpp::subprocess::pipe_stream{}},
                                    linuxpp::subprocess::stdout{linuxpp::subprocess::pipe_stream{}},
                                    linuxpp::subprocess::stderr{linuxpp::subprocess::pipe_stream{}}}};

    {
        const std::size_t ret = linuxpp::write(bin.stdin().get(), stdin_value.data(), stdin_value.length());
        ASSERT_EQ(stdin_value.length(), static_cast<std::size_t>(ret));
    }

    {
        epoll.add(bin.stdout().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stdout().get(), stdout_buf.data(), stdout_buf.size());
        ASSERT_EQ(stdout_value.length(), length);
    }

    {
        epoll.del(bin.stdout().get());
        epoll.add(bin.stderr().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stderr().get(), stderr_buf.data(), stderr_buf.size());
        ASSERT_EQ(stderr_value.length(), length);
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

TEST_F(stream_test, stdout_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stdout", stdout_value},
                                   {linuxpp::subprocess::stdout{linuxpp::subprocess::pipe_stream{}}}};

    {
        epoll.add(bin.stdout().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stdout().get(), stdout_buf.data(), stdout_buf.size());
        ASSERT_EQ(stdout_value.length(), length);
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

TEST_F(stream_test, stdout_stderr_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stdout", stdout_value, "--stderr", stderr_value},
                                    {linuxpp::subprocess::stdout{linuxpp::subprocess::pipe_stream{}},
                                     linuxpp::subprocess::stderr{linuxpp::subprocess::pipe_stream{}}}};

    {
        epoll.add(bin.stdout().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stdout().get(), stdout_buf.data(), stdout_buf.size());
        ASSERT_EQ(stdout_value.length(), length);
    }

    {
        epoll.del(bin.stdout().get());
        epoll.add(bin.stderr().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stderr().get(), stderr_buf.data(), stderr_buf.size());
        ASSERT_EQ(stderr_value.length(), length);
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

TEST_F(stream_test, stderr_pipe)
{
    linuxpp::subprocess::popen bin{test_path, {"--stderr", stderr_value},
                                   {linuxpp::subprocess::stderr{linuxpp::subprocess::pipe_stream{}}}};

    {
        epoll.add(bin.stderr().get(), EPOLLIN);
        const auto ready_fds = epoll.wait(std::chrono::seconds{5});
        ASSERT_EQ(1U, ready_fds.size());

        const auto length = linuxpp::read(bin.stderr().get(), stderr_buf.data(), stderr_buf.size());
        ASSERT_EQ(stderr_value.length(), length);
    }

    ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

    const auto status = bin.status();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_EQ(0, status.exit_code());
}

class bad_stream_test: public ::testing::Test
{
    protected:

    bad_stream_test();
    linuxpp::pipe pipe;
    int closed_fd;
};

bad_stream_test::bad_stream_test():
    closed_fd(pipe.write_fd().get())
{
    const int ret = ::close(pipe.write_fd().get());
    if (ret == -1)
    {
        throw std::runtime_error("failed to close file descriptor in bad_stream_test ctor");
    }
}

TEST_F(bad_stream_test, stdin_bad_read_fd)
{
    const auto throws = [this] () {
        linuxpp::subprocess::popen bin{"/dev/zero",
                                       {},
                                       {linuxpp::subprocess::stdin{linuxpp::subprocess::fd_stream{closed_fd}}}};
    };

    EXPECT_THROW(throws(), ndgpp::error<std::system_error>);
}


TEST_F(bad_stream_test, stdout_bad_write_fd)
{
    const auto throws = [this] () {
        linuxpp::subprocess::popen bin{test_path,
                                       {"--exit-code", 0},
                                       {linuxpp::subprocess::stdout{linuxpp::subprocess::fd_stream{closed_fd}}}};
    };

    EXPECT_THROW(throws(), ndgpp::error<std::system_error>);
}

TEST_F(bad_stream_test, stderr_bad_write_fd)
{
    const auto throws = [this] () {
        linuxpp::subprocess::popen bin{test_path,
                                       {"--exit-code", 0},
                                       {linuxpp::subprocess::stderr{linuxpp::subprocess::fd_stream{closed_fd}}}};
    };

    EXPECT_THROW(throws(), ndgpp::error<std::system_error>);
}

struct test_settings
{
    bool run_tests = true;
    bool check_stdin = false;
    bool write_stdout = false;
    bool write_stderr = false;
    std::string stdin_value = {};
    std::string stdout_value = {};
    std::string stderr_value = {};
    int exit_code = 0;
    int wait_for_signal = false;
};

int main(int argc, char ** argv)
{
    test_path = argv[0];
    test_settings settings;
    for (int i = 1; argv[i] != nullptr; ++i)
    {
        if (strcmp(argv[i], "--exit-code") == 0)
        {
            if (argv[i + 1] != nullptr)
            {
                settings.exit_code = std::atoi(argv[i + 1]);
            }
            else
            {
                return 2;
            }

            settings.run_tests = false;
        }

        if (strcmp(argv[i], "--signal") == 0)
        {
            settings.wait_for_signal = true;
            settings.run_tests = false;
        }

        if (strcmp(argv[i], "--stdin") == 0)
        {
            settings.check_stdin = true;
            settings.run_tests = false;
            if (argv[i + 1] != nullptr)
            {
                settings.stdin_value = argv[i + 1];
            }
            else
            {
                return 3;
            }
        }

        if (strcmp(argv[i], "--stdout") == 0)
        {
            settings.write_stdout = true;
            settings.run_tests = false;
            if (argv[i + 1] != nullptr)
            {
                settings.stdout_value = argv[i + 1];
            }
            else
            {
                return 4;
            }
        }

        if (strcmp(argv[i], "--stderr") == 0)
        {
            settings.write_stderr = true;
            settings.run_tests = false;
            if (argv[i + 1] != nullptr)
            {
                settings.stderr_value = argv[i + 1];
            }
            else
            {
                return 5;
            }
        }
    }

    if (settings.run_tests)
    {
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGCHLD);

        // Block SIGCHLD so sigtimedwait will work
        const int ret = pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
        if (ret == -1)
        {
            std::cerr << "failed to block SIGCHLD\n";
            return 6;
        }

        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    if (settings.wait_for_signal)
    {
        ::pause();
    }

    if (settings.check_stdin)
    {
        std::vector<char> stdin_value;
        stdin_value.resize(settings.stdin_value.length());
        std::cin.read(stdin_value.data(), stdin_value.size());

        if (std::equal(stdin_value.cbegin(),
                       stdin_value.cend(),
                       settings.stdin_value.cbegin(),
                       settings.stdin_value.cend()) == false)
        {
            return 7;
        }
    }

    if (settings.write_stdout)
    {
        std::cout << settings.stdout_value;
        std::cout.flush();
    }

    if (settings.write_stderr)
    {
        std::cerr << settings.stderr_value;
        std::cerr.flush();
    }

    return settings.exit_code;
}
