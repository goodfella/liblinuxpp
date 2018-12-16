#ifndef LIBLINUXPP_NET_SOCKADDR_HPP
#define LIBLINUXPP_NET_SOCKADDR_HPP

#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    /** @defgroup net::make_sockaddr net::make_sockaddr
     *
     *  The make_sockaddr function overload set returns struct sockaddr
     *  types based on the address type parameters
     *
     *  @param address The address to assign to the sockaddr
     *  @param port The port to assign to the sockaddr
     *
     *  @{
     */
    struct ::sockaddr_in make_sockaddr(const ndgpp::net::ipv4_address addr,
                                       const ndgpp::net::port port);

    /// @}
}}

#endif
