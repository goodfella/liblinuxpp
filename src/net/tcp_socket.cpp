#include <sys/socket.h>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/accept.hpp>
#include <liblinuxpp/net/bind.hpp>
#include <liblinuxpp/net/connect.hpp>
#include <liblinuxpp/net/recv.hpp>
#include <liblinuxpp/net/send.hpp>
#include <liblinuxpp/net/tcp_socket.hpp>


linuxpp::net::tcp_socket::tcp_socket() noexcept = default;

linuxpp::net::tcp_socket::tcp_socket(tcp_socket &&) noexcept = default;
linuxpp::net::tcp_socket & linuxpp::net::tcp_socket::operator= (linuxpp::net::tcp_socket &&) noexcept = default;


linuxpp::net::tcp_socket::tcp_socket(const int domain):
    members_{linuxpp::unique_fd<>{linuxpp::net::socket(domain, SOCK_STREAM, 0)}}
{}

linuxpp::net::tcp_socket::tcp_socket(linuxpp::net::bind_socket_t do_bind,
                                     const ndgpp::net::ipv4_address address,
                                     const ndgpp::net::port port):
    linuxpp::net::tcp_socket(AF_INET)
{
    linuxpp::net::bind(this->descriptor(), address, port);
}

linuxpp::net::tcp_socket::tcp_socket(linuxpp::net::bind_socket_t do_bind,
                                     const ndgpp::net::ipv4_address address):
    linuxpp::net::tcp_socket(AF_INET)
{
    linuxpp::net::bind(this->descriptor(), address);
}

linuxpp::net::tcp_socket::tcp_socket(linuxpp::net::connect_socket_t do_connect,
                                     const ndgpp::net::ipv4_address address,
                                     const ndgpp::net::port port):
    linuxpp::net::tcp_socket(AF_INET)
{
    linuxpp::net::connect(this->descriptor(), address, port);
}

int linuxpp::net::tcp_socket::descriptor() const noexcept
{
    return std::get<sock_descriptor>(this->members_).get();
}

linuxpp::net::tcp_socket::operator bool() const noexcept
{
    return static_cast<bool>(std::get<sock_descriptor>(this->members_));
}

void linuxpp::net::tcp_socket::close() noexcept
{
    std::get<sock_descriptor>(this->members_).reset();
}

void linuxpp::net::tcp_socket::shutdown(const int how)
{
    const int ret = ::shutdown(this->descriptor(), how);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "shutdown failed");
    }
}

void linuxpp::net::tcp_socket::bind(const ndgpp::net::ipv4_address address,
                                    const ndgpp::net::port port)
{
    linuxpp::net::bind(this->descriptor(), address, port);
}

void linuxpp::net::tcp_socket::bind(const ndgpp::net::ipv4_address address)
{
    linuxpp::net::bind(this->descriptor(), address);
}

void linuxpp::net::tcp_socket::connect(const ndgpp::net::ipv4_address address,
                                       const ndgpp::net::port port)
{
    linuxpp::net::connect(this->descriptor(), address, port);
}

void linuxpp::net::tcp_socket::listen(const int backlog)
{
    linuxpp::net::listen(this->descriptor(), backlog);
}

int linuxpp::net::tcp_socket::accept(const int flags)
{
    return linuxpp::net::accept(this->descriptor(), flags);
}

int linuxpp::net::tcp_socket::accept(ndgpp::net::ipv4_address & addr,
                                     ndgpp::net::port & port,
                                     const int flags)
{
    return linuxpp::net::accept(this->descriptor(), addr, port, flags);
}

std::size_t linuxpp::net::tcp_socket::recv(void * buf,
                                           const std::size_t buflen,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buf, buflen, flags);
}

std::size_t linuxpp::net::tcp_socket::recv(struct ::iovec * const buffs,
                                           const std::size_t size_buffs,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buffs, size_buffs, flags);
}

std::size_t linuxpp::net::tcp_socket::send(void const * const buf,
                                           const std::size_t length,
                                           const int flags)
{
    return linuxpp::net::send(this->descriptor(), buf, length, flags);
}

std::size_t linuxpp::net::tcp_socket::send(struct iovec const * const buffers,
                                           const std::size_t size_buffers,
                                           const int flags)
{
    return linuxpp::net::send(this->descriptor(), buffers, size_buffers, flags);
}
