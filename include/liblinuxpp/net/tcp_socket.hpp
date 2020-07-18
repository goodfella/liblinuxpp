#ifndef LIBLINUXPP_NET_TCP_SOCKET_HPP
#define LIBLINUXPP_NET_TCP_SOCKET_HPP

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <tuple>
#include <utility>

#include <liblinuxpp/file_descriptor.hpp>
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
        tcp_socket();

        /** Constructs a tcp_socket object from a socket descriptor
         *
         *  @param sock_fd The socket descriptor to take ownership of
         */
        tcp_socket(linuxpp::file_descriptor && sock_fd) noexcept;

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

        /** Constructs and binds a TCP socket to the specified address and ephemeral port
         *
         *  @param do_bind Tag type indicating that a bind should be performed
         *  @param address The address to bind to
         */
        explicit
        tcp_socket(linuxpp::net::bind_socket_t do_bind,
                   const ndgpp::net::ipv4_address address);

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

        explicit
        operator bool () const noexcept;

        /// Closes the underlying socket
        void close() noexcept;

        /// Shuts down the socket
        void shutdown(const int how);

        void swap(linuxpp::net::tcp_socket & other) noexcept;

        /** Binds the socket to the provided address and port
         *
         *  @param addr The address to bind to
         *  @param port The port to bind to
         */
        void bind(const ndgpp::net::ipv4_address addr,
                  const ndgpp::net::port port);

        /** Binds the socket to the provided address and an ephemeral port
         *
         *  @param addr The address to bind to
         */
        void bind(const ndgpp::net::ipv4_address addr);

        /** Connects the socket to the provided address
         *
         *  @param addr The address to connect to
         *  @param port The port to bind to
         */
        void connect(const ndgpp::net::ipv4_address addr, const ndgpp::net::port port);

        /** Marks the socket as a passive socket
         *
         *  @param backlog Maximum length of the pending connection
         *                 queue
         */
        void listen(const int backlog);

        /** Accepts a client connection
         *
         *  @param flags The flags to pass to the accept system call
         */
        int accept(const int flags = 0);

        /** Accepts a client connection
         *
         *  @param addr The address of the client socket
         *  @param addr The port of the client socket
         *  @param flags The flags to pass to the accept system call
         */
        int accept(ndgpp::net::ipv4_address & addr,
                   ndgpp::net::port & port,
                   const int flags = 0);

        // void * buffer based receive functions

        /** Receives a message
         *
         *  @param buf The buffer to place the message into
         *  @parma buflen The length of the buf parameter
         *  @param flags The flags to pass to the recv system call see
         *         man 2 recv for details
         *
         *  @throws ndgpp::error<std::system_error> when an error is encountered
         */
        std::size_t recv(void * buf, std::size_t buflen, const int flags = 0);


        // struct iovec based recv functions

        /** Calls recvmsg
         *
         *  @param buffs The iovec array representing the buffers to receive into
         *  @param size_buffs The number of instances in the iovec array
         *  @param flags The flags to pass to recvmsg see man 2 recv for details
         */
        std::size_t recv(struct ::iovec * const buffs,
                         const std::size_t size_buffs,
                         const int flags = 0);

        /** Calls sendto system call
         *
         *  @param buf The buffer of data to send
         *  @param length The length of the buffer
         *  @param addr The destination address
         *  @param port The destination port
         *  @param flags The flags to pass to sendto see man 2 sendto
         */
        std::size_t send(void const * buf,
                         const std::size_t length,
                         const int flags = 0);

        /** Calls sendmsg
         *
         *  @param buffers The buffers of data to send
         *  @param size_buffers The number of buffers
         *  @param flags The flags to pass to sendto see man 2 sendto
         */
        std::size_t send(struct ::iovec const * const buffers,
                         const std::size_t size_buffers,
                         const int flags = 0);

        private:

        enum members
        {
            sock_descriptor,
        };

        using tuple_type = std::tuple<linuxpp::unique_fd<>>;

        tuple_type members_ = {};
    };

    inline void tcp_socket::swap(linuxpp::net::tcp_socket & other) noexcept
    {
        std::swap(this->members_, other.members_);
    }

    inline void swap(linuxpp::net::tcp_socket & lhs, linuxpp::net::tcp_socket & rhs) noexcept
    {
        lhs.swap(rhs);
    }
}}

#endif
