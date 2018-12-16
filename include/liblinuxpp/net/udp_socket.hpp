#ifndef LIBLINUXPP_UDP_SOCKET_HPP
#define LIBLINUXPP_UDP_SOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>

#include <tuple>
#include <utility>

#include <libndgpp/net/ipv4_address.hpp>
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
        udp_socket & operator=(const udp_socket &) = delete;

        udp_socket(udp_socket &&) noexcept;
        udp_socket & operator=(udp_socket &&) noexcept;

        /// Closes the underlying socket
        void close() noexcept;

        /** Binds the socket to the provided address and port
         *
         *  @param addr The address to bind to
         *  @param port The port to bind to
         */
        void bind(const ndgpp::net::ipv4_address addr, const ndgpp::net::port port);

        /// Swaps the given udp_socket with this
        void swap(udp_socket & other) noexcept;

        /// Returns the underlying socket descriptor
        int descriptor() const noexcept;

        std::size_t recv(void * buf, std::size_t length);

        std::size_t send(void const * buf,
                         const std::size_t length,
                         const ndgpp::net::ipv4_address addr,
                         const ndgpp::net::port port,
                         const int flags = 0);

        std::size_t send(void const * buf,
                         const std::size_t length,
                         const struct sockaddr_in sockaddr,
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
