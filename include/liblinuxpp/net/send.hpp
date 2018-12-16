#ifndef LIBLINUXPP_NET_SEND_HPP
#define LIBLINUXPP_NET_SEND_HPP

#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <array>
#include <vector>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

namespace linuxpp {
namespace net {

    /** @defgroup net::send net::send
     *
     *  The send function overload set contains functions which can be used to send data on a socket
     *
     *  @param sd The socket descriptor to send the message on
     *  @param msghdr The struct msghdr type to use for sending the message
     *  @param flags The flags to use when sending the message
     *  @parma address The address to send the message to
     *  @param port The port to send the message to
     *  @param msg The message to send
     *  @param sockaddr The sockaddr type
     *  @param buffers The struct iovec array to send
     *  @param size_buffers the size of the struct iovec array
     *
     *  @return The number of bytes sent
     *  @throws ndgpp::error<std::system_error> if an error is encountered
     *  @{
     */

    std::size_t send(const int sd,
                     const struct msghdr msghdr,
                     const int flags);

    std::size_t send(const int sd,
                     void const * const msg,
                     std::size_t length,
                     const ndgpp::net::ipv4_address address,
                     const ndgpp::net::port port,
                     const int flags);

    std::size_t send(const int sd,
                     void const * const msg,
                     const std::size_t length,
                     const struct ::sockaddr_in sockaddr,
                     const int flags);

    std::size_t send(const int sd,
                     iovec const * buffers,
                     const std::size_t size_buffers,
                     const ndgpp::net::ipv4_address address,
                     const ndgpp::net::port port,
                     const int flags);

    std::size_t send(const int sd,
                     const std::vector<struct iovec> & buffers,
                     const ndgpp::net::ipv4_address address,
                     const ndgpp::net::port port,
                     const int flags);

    template <std::size_t N>
    std::size_t send(const int sd,
                     const std::array<struct iovec, N> & buffers,
                     const ndgpp::net::ipv4_address address,
                     const ndgpp::net::port port,
                     const int flags);

    /// @}

    template <std::size_t N>
    inline std::size_t send(const int sd,
                            const std::array<struct iovec, N> & buffers,
                            const ndgpp::net::ipv4_address address,
                            const ndgpp::net::port port,
                            const int flags)
    {
        return linuxpp::net::send(sd, buffers.data(), buffers.size(), address, port, flags);
    }
}}


#endif
