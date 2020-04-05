#ifndef LIBLINUXPP_MONOTONIC_TIMERFD_HPP
#define LIBLINUXPP_MONOTONIC_TIMERFD_HPP

#include <sys/timerfd.h>

#include <cstdint>

#include <chrono>

#include <libndgpp/error.hpp>

#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
    class monotonic_timerfd
    {
        public:

        monotonic_timerfd(const int flags);

        monotonic_timerfd();

        monotonic_timerfd(const monotonic_timerfd &) = delete;
        monotonic_timerfd & operator=(const monotonic_timerfd &) = delete;

        monotonic_timerfd(monotonic_timerfd &&);
        monotonic_timerfd & operator=(monotonic_timerfd &&);

        struct ::itimerspec
        set_oneshot(const std::chrono::steady_clock::time_point time);

        template <class Rep, class Period>
        struct ::itimerspec
        set_oneshot(const std::chrono::duration<Rep, Period> time);

        uint64_t
        read();

        int
        fd() const;

        private:

        linuxpp::unique_fd<> fd_;
    };

    template <class Rep, class Period>
    struct ::itimerspec
    monotonic_timerfd::set_oneshot(const std::chrono::duration<Rep, Period> time)
    {
        struct ::itimerspec new_spec = {};
        struct ::itimerspec old_spec = {};

        const long nanosecond_offset = time.count() == 0 ? 1 : 0;

        const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time);
        const auto nanoseconds =
            std::chrono::duration_cast<std::chrono::nanoseconds>(time) -
            std::chrono::duration_cast<std::chrono::nanoseconds>(seconds);

        new_spec.it_value.tv_sec = seconds.count();
        new_spec.it_value.tv_nsec = nanosecond_offset + nanoseconds.count();

        const int ret = ::timerfd_settime(this->fd_.get(), 0, &new_spec, &old_spec);
        if (ret == -1)
        {
            throw ndgpp_error(std::system_error,
                              std::error_code{errno, std::system_category()},
                              "timerfd_settime system call failed for given duration");
        }

        return old_spec;
    }
}

#endif
