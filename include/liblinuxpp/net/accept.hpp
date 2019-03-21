#ifndef LIBLINUXPP_NET_ACCEPT_HPP
#define LIBLINUXPP_NET_ACCEPT_HPP

#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    /** @defgroup net::accept net::accept
     *
     *  The net::accept overload set contains functions for accepting a
     *  connection
     *
     *  @param sd The socket descriptor
     *  @param addr The IP address to accept to
     *  @param port The port to accept to
     *  @param sockaddr A sockaddr struct to accept to
     *  @param flags The flags to pass to accept
     *
     *  @{
     */

    /// Accepts a socket on the given socket descriptor
    int accept(const int sd,
               const int flags = 0);

    /// Accepts a socket on the given socket descriptor
    int accept(const int sd,
               ndgpp::net::ipv4_address & addr,
               ndgpp::net::port & port,
               const int flags = 0);

    /// Accepts a connection on the given socket descriptor
    int accept(const int sd,
               struct ::sockaddr_in & sockaddr,
               const int flags = 0);
}}

#endif
