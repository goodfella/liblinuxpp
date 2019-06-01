#ifndef LIBLINUXPP_NET_TCP_DATAGRAM_SOCKET_HPP
#define LIBLINUXPP_NET_TCP_DATAGRAM_SOCKET_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <tuple>
#include <utility>
#include <vector>

#include <libndgpp/net/ipv4_address.hpp>
#include <libndgpp/net/port.hpp>

#include <liblinuxpp/net/socket.hpp>
#include <liblinuxpp/net/tcp_socket.hpp>

namespace linuxpp
{
namespace net
{
    class tcp_datagram_socket final
    {
        public:

        using msg_size_type = uint16_t;

        /// Constructs a tcp_datagram_socket object with no underlying socket
        tcp_datagram_socket();

        /** Constructs a TCP socket using the specified domain
         *
         *  @param domain The socket's domain e.g. AF_INET (IPv4), AF_INET6 (IPv6)
         *
         *  @note The socket is created with the SOCK_CLOEXEC flag
         *
         *  @throws ndgpp::error<std::system_error> if socket creation fails
         */
        explicit
        tcp_datagram_socket(const int domain);

        /** Constructs and binds a TCP socket to the specified address and port
         *
         *  @param do_bind Tag type indicating that a bind should be performed
         *  @param address The address to bind to
         *  @param port The port to bind to
         */
        explicit
        tcp_datagram_socket(linuxpp::net::bind_socket_t do_bind,
                            const ndgpp::net::ipv4_address address,
                            const ndgpp::net::port port);

        /** Constructs and binds a TCP socket to the specified address and ephemeral port
         *
         *  @param do_bind Tag type indicating that a bind should be performed
         *  @param address The address to bind to
         */
        explicit
        tcp_datagram_socket(linuxpp::net::bind_socket_t do_bind,
                            const ndgpp::net::ipv4_address address);

        /** Constructs and connects a TCP socket to the specified address and port
         *
         *  @param do_connect Tag type indicating that a connect should be performed
         *  @param address The address to bind to
         *  @param port The port to bind to
         */
        explicit
        tcp_datagram_socket(linuxpp::net::connect_socket_t do_connect,
                            const ndgpp::net::ipv4_address address,
                            const ndgpp::net::port port);

        tcp_datagram_socket(const tcp_datagram_socket &) = delete;
        tcp_datagram_socket(tcp_datagram_socket &&) noexcept;

        tcp_datagram_socket & operator=(const tcp_datagram_socket &) = delete;
        tcp_datagram_socket & operator=(tcp_datagram_socket &&) noexcept;

        /// Returns the underlying socket descriptor
        int descriptor() const noexcept;

        explicit
        operator bool () const noexcept;

        /// Closes the underlying socket
        void close() noexcept;

        /// Shuts down the socket
        void shutdown(const int how);

        void swap(linuxpp::net::tcp_datagram_socket & other) noexcept;

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
         *  @param flags The flags to pass to the recv system call see
         *         man 2 recv for details
         *
         *  @throws ndgpp::error<std::system_error> when an error is encountered
         */
        std::size_t recv(std::vector<unsigned char> & buf, const int flags = 0);

        struct receive_state
        {
            friend class tcp_datagram_socket;

            enum class state_type
            {
                receiving_msg_size,
                receiving_msg,
                finished
            };

            std::size_t msg_size;
            std::size_t bytes_received;
            state_type state;

            private:
            receive_state():
                msg_size(0),
                bytes_received(0),
                state(state_type::receiving_msg_size)
            {}
        };

        receive_state recv_part(const int flags = 0);

        receive_state recv_part(std::vector<unsigned char> & buf,
                                const receive_state state,
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

        template <std::size_t N>
        std::size_t send(struct ::iovec const * const buffers,
                         const int flags = 0);

        private:

        enum members
        {
            socket,
            iovec_vector,
        };

        using tuple_type = std::tuple<linuxpp::net::tcp_socket, std::vector<struct ::iovec>>;

        receive_state recv_size_part(const receive_state state,
                                     const int flags);

        receive_state recv_msg_part(std::vector<unsigned char> & buf,
                                    const receive_state state,
                                    const int flags);

        tuple_type members_ = {};
    };

    inline void tcp_datagram_socket::swap(linuxpp::net::tcp_datagram_socket & other) noexcept
    {
        std::swap(this->members_, other.members_);
    }

    inline void swap(linuxpp::net::tcp_datagram_socket & lhs, linuxpp::net::tcp_datagram_socket & rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <std::size_t N>
    std::size_t linuxpp::net::tcp_datagram_socket::send(struct ::iovec const * const bufs,
                                                        const int flags)
    {
        std::array<struct ::iovec, N + 1> iovec_array;

        // Determine how big the message is
        const std::size_t iovec_len_sum =
            std::accumulate(bufs,
                            bufs + N,
                            0U,
                            [] (const std::size_t val, const struct ::iovec iovec)
                            {
                                return val + iovec.iov_len;
                            });

        const msg_size_type msg_size = htons(static_cast<uint16_t>(iovec_len_sum));
        iovec_array[0].iov_len = msg_size;
        iovec_array[0].iov_base = const_cast<msg_size_type *>(&msg_size);

        // Copy the message into the iovec array
        std::copy(bufs, bufs + N, iovec_array.begin() + 1);

        return std::get<socket>(this->members_).send(iovec_array.data(), iovec_array.size(), flags);
    }
}}

#endif
