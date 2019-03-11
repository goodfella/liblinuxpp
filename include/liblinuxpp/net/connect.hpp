#ifndef LIBLINUXPP_NET_CONNECT_HPP
#define LIBLINUXPP_NET_CONNECT_HPP

#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    /** @defgroup net::connect net::connect
     *
     *  The net::connect overload set contains functions for connecting a
     *  socket to an address and port
     *
     *  @param sd The socket descriptor
     *  @param addr The IP address to connect to
     *  @param port The port to connect to
     *  @param sockaddr A sockaddr struct to connect to
     *
     *  @{
     */

    /// Connects a socket to the provided address and port
    void connect(const int sd,
                 const ndgpp::net::ipv4_address addr,
                 const ndgpp::net::port port);

    /// Connects a socket to the provided sockaddr_in struct
    void connect(const int sd, const struct ::sockaddr_in sockaddr);
}}

#endif
