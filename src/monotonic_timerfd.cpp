#include <sys/timerfd.h>


#include <libndgpp/error.hpp>

#include <liblinuxpp/monotonic_timerfd.hpp>
#include <liblinuxpp/read.hpp>

linuxpp::monotonic_timerfd::monotonic_timerfd(const int flags):
    fd_(::timerfd_create(CLOCK_MONOTONIC, flags | TFD_CLOEXEC))
{
    if (!this->fd_)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "timerfd_create system call failed");
    }
}

linuxpp::monotonic_timerfd::monotonic_timerfd():
    monotonic_timerfd(0)
{}

struct ::itimerspec
linuxpp::monotonic_timerfd::set_oneshot(const std::chrono::steady_clock::time_point timeout)
{
    const auto now = std::chrono::steady_clock::now();
    if (timeout > now)
    {
        return this->set_oneshot(timeout - now);
    }
    else
    {
        // By the time set_oneshot was called, the timeout time has
        // been surpased so just arm the timer so it expires right
        // away
        return this->set_oneshot(std::chrono::nanoseconds(1));
    }
}

uint64_t
linuxpp::monotonic_timerfd::read()
{
    uint64_t value;
    linuxpp::read(this->fd_.get(), &value, sizeof(value));
    return value;
}

int
linuxpp::monotonic_timerfd::fd() const
{
    return this->fd_.get();
}
