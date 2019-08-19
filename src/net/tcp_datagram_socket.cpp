#include <arpa/inet.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstdint>
#include <tuple>

#include <libndgpp/error.hpp>

#include <liblinuxpp/iovec.hpp>
#include <liblinuxpp/net/accept.hpp>
#include <liblinuxpp/net/bind.hpp>
#include <liblinuxpp/net/connect.hpp>
#include <liblinuxpp/net/recv.hpp>
#include <liblinuxpp/net/send.hpp>
#include <liblinuxpp/net/sockopt.hpp>
#include <liblinuxpp/net/tcp_datagram_socket.hpp>
#include <liblinuxpp/net/tcp_options.hpp>
#include <liblinuxpp/length_prefixed_message.hpp>


linuxpp::net::tcp_datagram_socket::tcp_datagram_socket() = default;

linuxpp::net::tcp_datagram_socket::tcp_datagram_socket(linuxpp::net::tcp_socket && sock) noexcept:
    members_{std::make_tuple(linuxpp::net::tcp_socket{std::move(sock)}, linuxpp::length_prefixed_message_vector<msg_size_type>{})}
{
    linuxpp::net::setsockopt<linuxpp::net::so::tcp_nodelay>(std::get<socket>(this->members_).descriptor(), 1);
}

linuxpp::net::tcp_datagram_socket::tcp_datagram_socket(tcp_datagram_socket &&) noexcept = default;
linuxpp::net::tcp_datagram_socket & linuxpp::net::tcp_datagram_socket::operator= (linuxpp::net::tcp_datagram_socket &&) noexcept = default;

linuxpp::net::tcp_datagram_socket & linuxpp::net::tcp_datagram_socket::operator= (linuxpp::net::tcp_socket && rhs) noexcept
{
    std::get<socket>(this->members_) = std::move(rhs);
    linuxpp::net::setsockopt<linuxpp::net::so::tcp_nodelay>(std::get<socket>(this->members_).descriptor(), 1);
    return *this;
}

linuxpp::net::tcp_datagram_socket::tcp_datagram_socket(const int domain):
    members_{std::make_tuple(linuxpp::net::tcp_socket{domain}, linuxpp::length_prefixed_message_vector<msg_size_type>{})}
{
    linuxpp::net::setsockopt<linuxpp::net::so::tcp_nodelay>(std::get<socket>(this->members_).descriptor(), 1);
}

linuxpp::net::tcp_datagram_socket::tcp_datagram_socket(linuxpp::net::bind_socket_t do_bind,
                                                       const ndgpp::net::ipv4_address address,
                                                       const ndgpp::net::port port):
    linuxpp::net::tcp_datagram_socket(AF_INET)
{
    std::get<socket>(this->members_).bind(address, port);
}

linuxpp::net::tcp_datagram_socket::tcp_datagram_socket(linuxpp::net::bind_socket_t do_bind,
                                                       const ndgpp::net::ipv4_address address):
    linuxpp::net::tcp_datagram_socket(AF_INET)
{
    std::get<socket>(this->members_).bind(address);
}

linuxpp::net::tcp_datagram_socket::tcp_datagram_socket(linuxpp::net::connect_socket_t do_connect,
                                                       const ndgpp::net::ipv4_address address,
                                                       const ndgpp::net::port port):
    linuxpp::net::tcp_datagram_socket(AF_INET)
{
    std::get<socket>(this->members_).connect(address, port);
}

int linuxpp::net::tcp_datagram_socket::descriptor() const noexcept
{
    return std::get<socket>(this->members_).descriptor();
}

linuxpp::net::tcp_datagram_socket::operator bool() const noexcept
{
    return static_cast<bool>(std::get<socket>(this->members_));
}

void linuxpp::net::tcp_datagram_socket::close() noexcept
{
    std::get<socket>(this->members_).close();
}

void linuxpp::net::tcp_datagram_socket::shutdown(const int how)
{
    std::get<socket>(this->members_).shutdown(how);
}

void linuxpp::net::tcp_datagram_socket::bind(const ndgpp::net::ipv4_address address,
                                    const ndgpp::net::port port)
{
    std::get<socket>(this->members_).bind(address, port);
}

void linuxpp::net::tcp_datagram_socket::bind(const ndgpp::net::ipv4_address address)
{
    std::get<socket>(this->members_).bind(address);
}

void linuxpp::net::tcp_datagram_socket::connect(const ndgpp::net::ipv4_address address,
                                       const ndgpp::net::port port)
{
    std::get<socket>(this->members_).connect(address, port);
}

void linuxpp::net::tcp_datagram_socket::listen(const int backlog)
{
    std::get<socket>(this->members_).listen(backlog);
}

int linuxpp::net::tcp_datagram_socket::accept(const int flags)
{
    return std::get<socket>(this->members_).accept(flags);
}

int linuxpp::net::tcp_datagram_socket::accept(ndgpp::net::ipv4_address & addr,
                                              ndgpp::net::port & port,
                                              const int flags)
{
    return std::get<socket>(this->members_).accept(addr, port, flags);
}

std::size_t linuxpp::net::tcp_datagram_socket::recv(std::vector<unsigned char> & buf,
                                                    const int flags)
{
    {
        msg_size_type msg_size = 0;
        const std::size_t ret = std::get<socket>(this->members_).recv(&msg_size,
                                                                      sizeof(msg_size),
                                                                      MSG_WAITALL);
        if (ret != sizeof(msg_size))
        {
            throw ndgpp_error(std::system_error,
                              std::error_code (ENODATA, std::system_category()),
                              "message length truncated");
        }

        msg_size = ntohs(msg_size);
        buf.resize(msg_size);
    }

    const std::size_t ret = std::get<socket>(this->members_).recv(buf.data(),
                                                                  buf.size(),
                                                                  MSG_WAITALL | flags);
    if (ret != buf.size())
    {
            throw ndgpp_error(std::system_error,
                              std::error_code (ENODATA, std::system_category()),
                              "message truncated");
    }

    return ret;
}

linuxpp::net::tcp_datagram_socket::receive_state
linuxpp::net::tcp_datagram_socket::recv_size_part(const linuxpp::net::tcp_datagram_socket::receive_state state,
                                                  const int flags)
{
    linuxpp::net::tcp_datagram_socket::receive_state new_state {state};
    std::array<std::uint8_t, sizeof(msg_size_type)> network_byte_order_msg_size;

    new_state.bytes_received += std::get<socket>(this->members_).recv(network_byte_order_msg_size.data() + state.bytes_received,
                                                                      network_byte_order_msg_size.size() - state.bytes_received,
                                                                      flags | MSG_DONTWAIT);

    for(std::size_t i = state.bytes_received; i < new_state.bytes_received; ++i)
    {
        new_state.msg_size |=
            static_cast<msg_size_type>(network_byte_order_msg_size[i]) << (((sizeof(msg_size_type) - 1) - i) * 8);

    }

    if (new_state.bytes_received != network_byte_order_msg_size.size())
    {
        // The full message size was not received, so return the
        // current state
        return new_state;
    }

    new_state.state =
        linuxpp::net::tcp_datagram_socket::receive_state::state_type::receiving_msg;
    new_state.bytes_received = 0;
    return new_state;
}

linuxpp::net::tcp_datagram_socket::receive_state
linuxpp::net::tcp_datagram_socket::recv_msg_part(std::vector<unsigned char> & buf,
                                                 const linuxpp::net::tcp_datagram_socket::receive_state state,
                                                 const int flags)
{
    buf.resize(state.msg_size);
    linuxpp::net::tcp_datagram_socket::receive_state new_state {state};
    new_state.bytes_received += std::get<socket>(this->members_).recv(buf.data() + state.bytes_received,
                                                                      buf.size() - state.bytes_received,
                                                                      flags | MSG_DONTWAIT);

    if (new_state.bytes_received != new_state.msg_size)
    {
        return new_state;
    }

    new_state.state = linuxpp::net::tcp_datagram_socket::receive_state::state_type::finished;
    return new_state;
}

linuxpp::net::tcp_datagram_socket::receive_state
linuxpp::net::tcp_datagram_socket::recv_part(const int flags)
{
    return this->recv_size_part(linuxpp::net::tcp_datagram_socket::receive_state {}, flags);
}

linuxpp::net::tcp_datagram_socket::receive_state
linuxpp::net::tcp_datagram_socket::recv_part(std::vector<unsigned char> & buf,
                                             const linuxpp::net::tcp_datagram_socket::receive_state state,
                                             const int flags)
{
    if (state.state == linuxpp::net::tcp_datagram_socket::receive_state::state_type::receiving_msg_size)
    {
        const auto new_state = this->recv_size_part(state, flags);
        return new_state;
    }
    else if (state.state == linuxpp::net::tcp_datagram_socket::receive_state::state_type::receiving_msg)
    {
        return this->recv_msg_part(buf, state, flags);
    }
    else
    {
        const auto new_state = this->recv_size_part(linuxpp::net::tcp_datagram_socket::receive_state {},
                                                    flags);
        return new_state;
    }
}

std::size_t linuxpp::net::tcp_datagram_socket::send(void const * const buf,
                                                    const std::uint16_t length,
                                                    const int flags)
{
    const linuxpp::length_prefixed_message_buffer<msg_size_type> msg{buf, length};
    return std::get<socket>(this->members_).send(msg.iovec().data(),
                                                 msg.iovec().size(),
                                                 flags);
}

std::size_t linuxpp::net::tcp_datagram_socket::send(struct ::iovec const * const buffers,
                                                    const std::size_t size_buffers,
                                                    const int flags)
{
    linuxpp::length_prefixed_message_vector<msg_size_type> & msg = std::get<iovec_vector>(this->members_);
    msg.fill(buffers, size_buffers);
    return std::get<socket>(this->members_).send(msg.iovec().data(), msg.iovec().size(), flags);
}
