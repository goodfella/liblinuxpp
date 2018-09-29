#ifndef LIBLINUXPP_SUBPROCESS_POPEN_HPP
#define LIBLINUXPP_SUBPROCESS_POPEN_HPP

#include <sys/types.h>

#include <mutex>
#include <tuple>
#include <string>

#include <liblinuxpp/subprocess/stream.hpp>
#include <liblinuxpp/subprocess/streams.hpp>
#include <liblinuxpp/subprocess/argv.hpp>
#include <liblinuxpp/subprocess/status.hpp>
#include <liblinuxpp/pid.hpp>
#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
namespace subprocess
{
    class popen
    {
        public:

        popen(const linuxpp::subprocess::argv& argv,
              const linuxpp::subprocess::streams& streams);

        ~popen();

        linuxpp::subprocess::status wait();
        linuxpp::subprocess::status poll();

        void signal(const int signal);

        linuxpp::unique_fd<>& stdin() noexcept;
        linuxpp::unique_fd<>& stdout() noexcept;
        linuxpp::unique_fd<>& stderr() noexcept;

        const linuxpp::unique_fd<>& stdin() const noexcept;
        const linuxpp::unique_fd<>& stdout() const noexcept;
        const linuxpp::unique_fd<>& stderr() const noexcept;

        linuxpp::pid pid() const noexcept;

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
