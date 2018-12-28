#ifndef LIBLINUXPP_SOCKET_HPP
#define LIBLINUXPP_SOCKET_HPP

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <tuple>

#include <liblinuxpp/unique_fd.hpp>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp
{
namespace net
{
    /** Constructs a socket with the given domain, type and protocol
     *
     *  @param domain The socket domain.  See man 2 socket
     *  @param type The socket type.  See man 2 socket
     *  @param protocol The socket protocol.  See man 2 socket
     *
     *  @note This function sets the SOCK_CLOEXEC flag
     *
     *  @return a linuxpp::unique_fd that references the socket
     *
     *  @throws ndgpp::error<std::system_error> if an error occurs creating the socket
     */
    linuxpp::unique_fd<> socket(const int domain, const int type, const int protocol);


    /** @defgroup net::getsockname net::getsockname
     *
     *  The net::getsockname
     *
     *  @param sd The socket descriptor
     *  @param sockaddr [out] The sockaddr_in to write the name information to
     *
     *  @{
     */
    void getsockname(const int sd, struct ::sockaddr_in & sockaddr);

    /// @}


    /** Returns IPv4 name information for a socket
     *
     *  @param sd The socket descriptor
     *
     *  @throws ndgpp::error<std::invalid_argument> if the socket is not bound to an IPv4 address
     */
    std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> getsockname_ipv4(const int sd);
}
}

#endif
