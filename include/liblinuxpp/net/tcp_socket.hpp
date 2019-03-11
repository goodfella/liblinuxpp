#ifndef LIBLINUXPP_NET_TCP_SOCKET_HPP
#define LIBLINUXPP_NET_TCP_SOCKET_HPP

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <tuple>
#include <utility>

#include <liblinuxpp/net/socket.hpp>
#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
namespace net
{
    class tcp_socket final
    {
        public:

        /// Constructs a tcp_socket object with no underlying socket
        tcp_socket() noexcept;

        /** Constructs a TCP socket using the specified domain
         *
         *  @param domain The socket's domain e.g. AF_INET (IPv4), AF_INET6 (IPv6)
         *
         *  @note The socket is created with the SOCK_CLOEXEC flag
         *
         *  @throws ndgpp::error<std::system_error> if socket creation fails
         */
        explicit
        tcp_socket(const int domain);

        /** Constructs and binds a TCP socket to the specified address and port
         *
         *  @param do_bind Tag type indicating that a bind should be performed
         *  @param address The address to bind to
         *  @param port The port to bind to
         */
        explicit
        tcp_socket(linuxpp::net::bind_socket_t do_bind,
                   const ndgpp::net::ipv4_address address,
                   const ndgpp::net::port port);

        /** Constructs and connects a TCP socket to the specified address and port
         *
         *  @param do_connect Tag type indicating that a connect should be performed
         *  @param address The address to bind to
         *  @param port The port to bind to
         */
        explicit
        tcp_socket(linuxpp::net::connect_socket_t do_connect,
                   const ndgpp::net::ipv4_address address,
                   const ndgpp::net::port port);

        tcp_socket(const tcp_socket &) = delete;
        tcp_socket(tcp_socket &&) noexcept;

        tcp_socket & operator=(const tcp_socket &) = delete;
        tcp_socket & operator=(tcp_socket &&) noexcept;

        /// Returns the underlying socket descriptor
        int descriptor() const noexcept;

        /// Closes the underlying socket
        void close() noexcept;

        /** Binds the socket to the provided address and port
         *
         *  @param addr The address to bind to
         *  @param port The port to bind to
         */
        void bind(const ndgpp::net::ipv4_address addr,
                  const ndgpp::net::port port);

        /** Connects the socket to the provided address
         *
         *  @param addr The address to connect to
         *  @param port The port to bind to
         */
        void connect(const ndgpp::net::ipv4_address addr, const ndgpp::net::port port);

        private:

        enum members
        {
            sock_descriptor,
        };

        using tuple_type = std::tuple<linuxpp::unique_fd<>>;

        tuple_type members_ = {};
    };
}}

#endif
