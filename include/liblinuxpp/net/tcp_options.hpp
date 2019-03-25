#ifndef LIBLINUXPP_NET_TCP_OPTIONS_HPP
#define LIBLINUXPP_NET_TCP_OPTIONS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <liblinuxpp/net/sockopt_traits.hpp>

namespace linuxpp {
namespace net {
namespace so {

    template <int Name>
    struct tcp_trait
    {
        static constexpr int level() noexcept { return IPPROTO_TCP; };
        static constexpr int name = Name;
    };

    /// IP_ADD_MEMBERSHIP socket option
    struct tcp_nodelay_t: public tcp_trait<TCP_NODELAY>
    {
        using value_type = int;
    };

    struct tcp_nodelay {};
    template <>
    struct traits<tcp_nodelay> : public tcp_nodelay_t {};
}}}

#endif
