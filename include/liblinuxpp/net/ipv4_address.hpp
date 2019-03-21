#ifndef LIBLINUXPP_NET_BIND_HPP
#define LIBLINUXPP_NET_BIND_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    static constexpr ndgpp::net::ipv4_address inaddr_any {INADDR_ANY};
    static constexpr ndgpp::net::ipv4_address inaddr_loopback {INADDR_LOOPBACK};
}}

#endif
