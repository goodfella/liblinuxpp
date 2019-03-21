#ifndef LIBLINUXPP_NET_BIND_HPP
#define LIBLINUXPP_NET_BIND_HPP

#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    /** @defgroup net::bind net::bind
     *
     *  The net::bind overload set contains functions for binding a socket to an address and port
     *
     *  @param sd The socket descriptor
     *  @param addr The IP address to bind to
     *  @param port The port to bind to
     *  @param sockaddr A sockaddr struct to bind to
     *  @param reuse_addr Set the SO_REUSEADDR socket option
     *  @param reuse_port Set the SO_REUSEPORT socket option
     *
     *  @{
     */

    /// Binds a socket to the provided address and port
    void bind(const int sd,
              const ndgpp::net::ipv4_address addr,
              const ndgpp::net::port port,
              const bool reuse_addr = false,
              const bool reuse_port = false);

    /// Binds a socket to the provided address and ephemeral port
    void bind(const int sd,
              const ndgpp::net::ipv4_address addr,
              const bool reuse_addr = false);

    /// Binds a socket to the port and address specified in the provided sockaddr_in struct
    void bind(const int sd,
              const sockaddr_in sockaddr,
              const bool reuse_addr = false,
              const bool reuse_port = false);
}}

#endif
