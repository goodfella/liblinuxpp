#ifndef LIBLINUXPP_NET_BIND_HPP
#define LIBLINUXPP_NET_BIND_HPP

#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>
#include <libndgpp/net/protocols.hpp>


namespace linuxpp {
namespace net {

    static constexpr ndgpp::net::ipv4_address inaddr_any {INADDR_ANY};
    static constexpr ndgpp::net::ipv4_address inaddr_localhost {INADDR_LOOPBACK};
}}

#endif
