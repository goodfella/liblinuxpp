#ifndef LIBLINUXPP_NET_IP_OPTIONS_HPP
#define LIBLINUXPP_NET_IP_OPTIONS_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <liblinuxpp/net/sockopt_traits.hpp>

namespace linuxpp {
namespace net {
namespace so {

    template <int Name>
    struct ip_trait
    {
        static constexpr int level() noexcept { return IPPROTO_IP; };
        static constexpr int name = Name;
    };

    /// IP_ADD_MEMBERSHIP socket option
    struct add_membership_t: public ip_trait<IP_ADD_MEMBERSHIP>
    {
        using value_type = struct ip_mreqn;
    };

    struct add_membership {};
    template <>
    struct traits<add_membership> : public add_membership_t {};
}}}

#endif
