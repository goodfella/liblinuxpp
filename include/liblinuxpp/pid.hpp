#ifndef LIBLINUXPP_PID_HPP
#define LIBLINUXPP_PID_HPP

#include <sys/types.h>
#include <utility>

#include <liblinuxpp/int_descriptor.hpp>

namespace linuxpp
{
    constexpr pid_t invalid_pid = -1;

    namespace detail
    {
        struct pid_tag;
    }

    template<>
    struct int_descriptor_traits<detail::pid_tag>
    {
        static constexpr pid_t invalid_value = invalid_pid;
    };

    using pid = int_descriptor<detail::pid_tag, pid_t>;

    static_assert(std::is_same<int_descriptor_traits<detail::pid_tag>, typename pid::type_traits>::value,
                  "pid::type_traits is incorrect");
}

#endif
