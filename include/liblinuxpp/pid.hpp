#ifndef LIBLINUXPP_PID_HPP
#define LIBLINUXPP_PID_HPP

#include <sys/types.h>
#include <utility>

namespace linuxpp
{
    constexpr pid_t invalid_pid = -1;
    class pid
    {
        public:

        constexpr pid(const pid_t value = linuxpp::invalid_pid) noexcept;

        pid(const pid& other) = default;
        pid & operator=(const pid& other) = default;

        pid(pid&& other) noexcept;
        pid& operator=(pid&& other) noexcept;

        pid_t get() const noexcept;

        void swap(pid& other) noexcept;
        void reset(const pid_t other = invalid_pid) noexcept;
        pid_t release() noexcept;

        explicit
        operator bool() const noexcept;

        private:

        pid_t value_;
    };

    inline constexpr pid::pid(const pid_t value) noexcept:
                                                  value_(value)
    {}

    inline pid::pid(pid&& other) noexcept:
                                  value_(other.value_)
    {}

    inline pid& pid::operator=(pid&& other) noexcept
    {
        this->value_ = other.value_;
        other.value_ = invalid_pid;
        return *this;
    }

    inline pid_t pid::get() const noexcept
    {
        return this->value_;
    }

    inline void pid::swap(pid& other) noexcept
    {
        std::swap(this->value_, other.value_);
    }

    inline void pid::reset(const pid_t other) noexcept
    {
        this->value_ = other;
    }

    inline pid_t pid::release() noexcept
    {
        const pid_t ret = this->value_;
        this->value_ = linuxpp::invalid_pid;
        return ret;
    }

    inline pid::operator bool() const noexcept
    {
        return this->value_ != linuxpp::invalid_pid;
    }
}

#endif
