#ifndef LIBLINUXPP_NET_UDP_SOCKET_HPP
#define LIBLINUXPP_NET_UDP_SOCKET_HPP

#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <tuple>
#include <utility>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/multicast_ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
namespace net
{
    /// Resource owning type for a UDP socket
    class udp_socket final
    {
        public:

        /// Constructs a udp_socket object with no underlying socket
        udp_socket() noexcept;

        /** Constructs a UDP socket using the specified domain
         *
         *  @param domain The socket's domain e.g. AF_INET (IPv4), AF_INET6 (IPv6)
         *
         *  @note The socket is created with the SOCK_CLOEXEC flag
         *
         *  @throws ndgpp::error<std::system_error> if socket creation fails
         */
        explicit
        udp_socket(const int domain);

        /** Constructs and binds a UDP socket to the specified address and port
         *
         *  @param address The address to bind to
         *  @param port The port to bind to
         */
        explicit
        udp_socket(const ndgpp::net::ipv4_address address,
                   const ndgpp::net::port port);

        udp_socket(const udp_socket &) = delete;
        udp_socket(udp_socket &&) noexcept;

        udp_socket & operator=(const udp_socket &) = delete;
        udp_socket & operator=(udp_socket &&) noexcept;

        /// Closes the underlying socket
        void close() noexcept;

        /** Binds the socket to the provided address and port
         *
         *  @param addr The address to bind to
         *  @param port The port to bind to
         *  @param reuse_addr If true, set the SO_REUSEADDR socket option
         *  @param reuse_port If true, set the SO_REUSEPORT socket option
         */
        void bind(const ndgpp::net::ipv4_address addr,
                  const ndgpp::net::port port,
                  const bool reuse_addr = false,
                  const bool reuse_port = false);

        /** Binds the socket to the provided address and an ephemeral port
         *
         *  @param addr The address to bind to
         *  @param reuse_addr If true, set the SO_REUSEADDR socket option
         */
        void bind(const ndgpp::net::ipv4_address addr, const bool reuse_addr = false);

        void join_group(const ndgpp::net::multicast_ipv4_address addr,
                        const std::string & interface = "");

        /// Swaps the given udp_socket with this
        void swap(udp_socket & other) noexcept;

        /// Returns the underlying socket descriptor
        int descriptor() const noexcept;


        // void * buffer based receive functions

        /** Receives a datagram
         *
         *  @param buf The buffer to place the datagram into
         *  @parma buflen The length of the buf parameter
         *  @param flags The flags to pass to the recv system call see
         *         man 2 recv for details
         *
         *  @throws ndgpp::error<std::system_error> when an error is encountered
         */
        std::size_t recv(void * buf, std::size_t buflen, const int flags = 0);

        /** Receives a datagram
         *
         *  @param buf The buffer to place the datagram into
         *  @parma buflen The length of the buf parameter
         *  @param sockaddr The sockaddr_in struct to place the sender information
         *  @param flags The flags to pass to the recv system call see
         *         man 2 recv for details
         *
         *  @throws ndgpp::error<std::system_error> when an error is encountered
         */
        std::size_t recv(void * buf,
                         std::size_t buflen,
                         struct ::sockaddr_in & sockaddr,
                         const int flags = 0);

        /** Receives a datagram
         *
         *  @param buf The buffer to place the datagram into
         *  @parma buflen The length of the buf parameter
         *  @param addr The address value to place the sender address in
         *  @param port The port value to place the sender port in
         *  @param flags The flags to pass to the recv system call see
         *         man 2 recv for details
         *
         *  @throws ndgpp::error<std::system_error> when an error is encountered
         */
        std::size_t recv(void * buf,
                         std::size_t buflen,
                         ndgpp::net::ipv4_address & addr,
                         ndgpp::net::port & port,
                         const int flags = 0);

        std::size_t send(void const * buf,
                         const std::size_t length,
                         const ndgpp::net::ipv4_address addr,
                         const ndgpp::net::port port,
                         const int flags = 0);

        std::size_t send(void const * buf,
                         const std::size_t length,
                         const struct ::sockaddr_in sockaddr,
                         const int flags = 0);

        std::size_t send(struct ::iovec const * const buffers,
                         const std::size_t size_buffers,
                         const ndgpp::net::ipv4_address addr,
                         const ndgpp::net::port port,
                         const int flags = 0);

        std::size_t send(struct ::iovec const * const buffers,
                         const std::size_t size_buffers,
                         const struct ::sockaddr_in sockaddr,
                         const int flags = 0);

        /// Returns true if the underlying socket is valid
        explicit operator bool() const noexcept;

        private:

        enum members
        {
            sock_descriptor,
        };

        using tuple_type = std::tuple<linuxpp::unique_fd<>>;

        tuple_type members_ = {};
    };

    inline void udp_socket::swap(udp_socket & other) noexcept
    {
        std::swap(this->members_, other.members_);
    }

    /// Swaps two udp_socket objects
    inline void swap(udp_socket & lhs, udp_socket & rhs) noexcept
    {
        lhs.swap(rhs);
    }
}
}

#endif
