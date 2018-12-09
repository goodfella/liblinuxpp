#ifndef LIBLINUXPP_BIND_HPP
#define LIBLINUXPP_BIND_HPP

#include <netinet/in.h>
#include <netinet/ip.h>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    /** Binds a socket to the provided address and port
     *
     *  @param sd The socket descriptor to bind
     *  @param addr The address to bind to
     *  @param port The port to bind to
     */
    void bind(const int sd, const ndgpp::net::ipv4_address addr, const ndgpp::net::port port);

    /** Binds a socket to INADDR_ANY and the provided port
     *
     *  @param sd The socket descriptor to bind
     *  @param port The port to bind to
     */
    void bind(const int sd, const ndgpp::net::port port);

    /** Binds a socket to the port and address specified in the provided sockaddr_in struct
     *
     *  @param sd The socket descriptor to bind
     *  @param addr The sockaddr_in struct to bind to
     */
    void bind(const int sd, const sockaddr_in addr);
}}

#endif
