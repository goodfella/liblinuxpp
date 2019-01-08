#ifndef LIBLINUXPP_NET_RECV_HPP
#define LIBLINUXPP_NET_RECV_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <cstddef>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

#include <liblinuxpp/net/sockaddr.hpp>

namespace linuxpp {
namespace net {

    /** @defgroup net::recv net::recv
     *
     *  The net::recv overload set contains functions for receiving
     *  messages on a given socket
     *
     *  @param sd The socket descriptor
     *  @param buf The buffer to receive the message into
     *  @param buffs A iovec structure
     *  @param size_buffs The number of buffers in the iovec structure
     *  @param buflen The length of the buffer
     *  @param flags The flags for the recv system call
     *  @param sockaddr A sockaddr struct to store the address information
     *
     *  @param addrlen [in & out] This parameter should set to the
     *                 size of sockaddr and on return it will be set
     *                 to the size of the source address
     *
     *  @param addr A ndgpp::net IP address struct
     *  @param port A ndgpp::net::port type to store the port information
     *
     *  @{
     */

    /// Calls the recv system call
    std::size_t recv(const int sd,
                     void * buf,
                     const std::size_t buflen,
                     const int flags = 0);

    /// Calls the recvfrom system call
    std::size_t recv(const int sd,
                     void * buf,
                     const std::size_t buflen,
                     struct ::sockaddr * sockaddr,
                     ::socklen_t * addrlen,
                     const int flags = 0);

    /// Calls the recvfrom system call
    std::size_t recv(const int sd,
                     void * buf,
                     const std::size_t buflen,
                     struct ::sockaddr_in & sockaddr,
                     const int flags = 0);

    /// Calls the recvfrom system call
    std::size_t recv(const int sd,
                     void * buf,
                     const std::size_t buflen,
                     ndgpp::net::ipv4_address & addr,
                     ndgpp::net::port & port,
                     const int flags = 0);

    /// Calls the recvmsg system call
    std::size_t recv(const int sd,
                     struct iovec * const buffs,
                     const std::size_t size_bufs,
                     const int flags = 0);

    /// Calls the recvmsg system call
    std::size_t recv(const int sd,
                     struct iovec * const buffs,
                     const std::size_t size_bufs,
                     struct ::sockaddr_in & sockaddr,
                     const int flags = 0);

    /// Calls the recvmsg system call
    std::size_t recv(const int sd,
                     struct iovec * const buffs,
                     const std::size_t size_bufs,
                     ndgpp::net::ipv4_address & addr,
                     ndgpp::net::port & port,
                     const int flags = 0);
    /// @}
}}

#endif
