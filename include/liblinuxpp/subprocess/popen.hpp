#ifndef LIBLINUXPP_SUBPROCESS_POPEN_HPP
#define LIBLINUXPP_SUBPROCESS_POPEN_HPP

#include <sys/types.h>

#include <mutex>
#include <tuple>

#include <liblinuxpp/subprocess/stream.hpp>
#include <liblinuxpp/subprocess/streams.hpp>
#include <liblinuxpp/subprocess/path_stream.hpp>
#include <liblinuxpp/subprocess/pipe_stream.hpp>
#include <liblinuxpp/subprocess/null_stream.hpp>
#include <liblinuxpp/subprocess/fd_stream.hpp>
#include <liblinuxpp/subprocess/argv.hpp>
#include <liblinuxpp/subprocess/status.hpp>
#include <liblinuxpp/pid.hpp>
#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
namespace subprocess
{
    /** Manages a subprocess
     *
     *  @par Copy Semantics Non-copyable
     */
    class popen final
    {
        public:

        /** Spawns a subprocess
         *
         *  This version spawns the subprocess using the execve system
         *  call.
         *
         *  @param executable The path to the executable
         *  @param argv The command line parameters to pass to the executable
         *  @param streams The stdin, stdout, and stderr stream definitions
         */
        explicit
        popen(const std::string& executable,
              const linuxpp::subprocess::argv& argv,
              const linuxpp::subprocess::streams& streams);

        ~popen();

        popen(const popen&) = delete;
        popen& operator=(const popen&) = delete;

        popen(popen&&) noexcept;
        popen& operator= (popen&&) noexcept;

        /** Blocks until the subprocess exits
         *
         *  @throws ndgpp::error<std::system_error> on error
         */
        linuxpp::subprocess::status wait();

        /** Checks if the subprocess has exited
         *
         *  @return linuxpp::subprocess::status that represents an
         *          exited process if the subprocess has exited, else,
         *          returns a default constructed
         *          linuxpp::subprocess::status object
         *
         *  @throws ndgpp::error<std::system_error> on error
         */
        linuxpp::subprocess::status poll();

        /** Sends a signal to the subprocess
         *
         *  @param signal The signal to send to the subprocess
         *
         *  @throws ndgpp::error<std::system_error> on error, or
         *          ndgpp::error<std::logic_error> if the popen object
         *          is no longer associated with a running subprocess
         */
        void signal(const int signal) const;

        linuxpp::unique_fd<>& stdin() noexcept;
        linuxpp::unique_fd<>& stdout() noexcept;
        linuxpp::unique_fd<>& stderr() noexcept;

        const linuxpp::unique_fd<>& stdin() const noexcept;
        const linuxpp::unique_fd<>& stdout() const noexcept;
        const linuxpp::unique_fd<>& stderr() const noexcept;

        /** Returns the subprocess's pid
         *
         *  @note Once linuxpp::subprocess::popen::wait returns or
         *        linuxpp::subprocess::popen::poll returns a status
         *        object that represents an exited subprocess, this
         *        function returns a linuxpp::pid assigned to
         *        linuxp::invalid_pid.
         */
        linuxpp::pid pid() const noexcept;
        linuxpp::subprocess::status status() const noexcept;

        private:

        linuxpp::subprocess::status wait(const int options);

        enum members
        {
            child_pid,
            stdin_stream,
            stdout_stream,
            stderr_stream,
            child_status,
        };

        using tuple_type = std::tuple<linuxpp::pid,
                                      linuxpp::unique_fd<>,
                                      linuxpp::unique_fd<>,
                                      linuxpp::unique_fd<>,
                                      linuxpp::subprocess::status>;
        tuple_type members_;

        static std::mutex clone_mutex_;
    };
}
}

#endif
