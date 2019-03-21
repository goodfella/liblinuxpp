#ifndef LIBLINUXPP_NET_SOCKET_OPTIONS_HPP
#define LIBLINUXPP_NET_SOCKET_OPTIONS_HPP

#include <liblinuxpp/net/sockopt_traits.hpp>

namespace linuxpp {
namespace net {
namespace so {

    template <int Name>
    struct socket_trait
    {
        static constexpr int level() noexcept { return SOL_SOCKET; };
        static constexpr int name = Name;
    };

    template <int Name>
    struct socket_int_trait : public socket_trait<Name>
    {
        using value_type = int;
    };

    /// SO_DOMAIN socket trait
    struct domain {};
    template <>
    struct traits<domain> : public socket_int_trait<SO_DOMAIN> {};

    /// SO_REUSEADDR socket trait
    struct reuse_addr {};
    template <>
    struct traits<reuse_addr> : public socket_int_trait<SO_REUSEADDR> {};

    /// SO_REUSEPORT socket trait
    struct reuse_port {};
    template <>
    struct traits<reuse_port> : public socket_int_trait<SO_REUSEPORT> {};

    /// SO_ACCEPTCONN SOCKET TRAIT
    struct accept_conn {};
    template <>
    struct traits<accept_conn> : public socket_int_trait<SO_ACCEPTCONN> {};
}}}

#endif
