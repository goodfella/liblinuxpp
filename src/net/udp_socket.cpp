#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/bind.hpp>
#include <liblinuxpp/net/socket.hpp>
#include <liblinuxpp/net/udp_socket.hpp>
#include <liblinuxpp/net/send.hpp>
#include <liblinuxpp/net/recv.hpp>

linuxpp::net::udp_socket::udp_socket() noexcept = default;

linuxpp::net::udp_socket::udp_socket(const int domain):
    members_{linuxpp::unique_fd<>{linuxpp::net::socket(domain, SOCK_DGRAM, 0)}}
{}

linuxpp::net::udp_socket::udp_socket(const ndgpp::net::ipv4_address addr,
                                     const ndgpp::net::port port):
    members_{linuxpp::unique_fd<>{linuxpp::net::socket(AF_INET, SOCK_DGRAM, 0)}}
{
    linuxpp::net::bind(this->descriptor(), addr, port);
}

linuxpp::net::udp_socket::udp_socket(udp_socket &&) noexcept = default;
linuxpp::net::udp_socket & linuxpp::net::udp_socket::operator=(udp_socket &&) noexcept = default;

void linuxpp::net::udp_socket::close() noexcept
{
    std::get<sock_descriptor>(this->members_).reset();
}

void linuxpp::net::udp_socket::bind(const ndgpp::net::ipv4_address addr,
                                    const ndgpp::net::port port,
                                    const bool reuse_addr,
                                    const bool reuse_port)
{
    linuxpp::net::bind(this->descriptor(), addr, port, reuse_addr, reuse_port);
}

void linuxpp::net::udp_socket::bind(const ndgpp::net::ipv4_address addr, const bool reuse_addr)
{
    linuxpp::net::bind(this->descriptor(), addr, reuse_addr);
}

std::size_t linuxpp::net::udp_socket::recv(void * buf,
                                           const std::size_t buflen,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buf, buflen, flags);
}

std::size_t linuxpp::net::udp_socket::recv(void * buf,
                                           const std::size_t buflen,
                                           struct ::sockaddr_in & sockaddr,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buf, buflen, sockaddr, flags);
}

std::size_t linuxpp::net::udp_socket::recv(void * buf,
                                           const std::size_t buflen,
                                           ndgpp::net::ipv4_address & addr,
                                           ndgpp::net::port & port,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buf, buflen, addr, port, flags);
}

std::size_t linuxpp::net::udp_socket::recv(struct ::iovec * const buffs,
                                           const std::size_t size_buffs,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buffs, size_buffs, flags);
}

std::size_t linuxpp::net::udp_socket::recv(struct ::iovec * const buffs,
                                           const std::size_t size_buffs,
                                           struct ::sockaddr_in & sockaddr,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buffs, size_buffs, sockaddr, flags);
}

std::size_t linuxpp::net::udp_socket::recv(struct ::iovec * const buffs,
                                           const std::size_t size_buffs,
                                           ndgpp::net::ipv4_address & addr,
                                           ndgpp::net::port & port,
                                           const int flags)
{
    return linuxpp::net::recv(this->descriptor(), buffs, size_buffs, addr, port, flags);
}

std::size_t linuxpp::net::udp_socket::send(void const * const buf,
                                           const std::size_t length,
                                           const ndgpp::net::ipv4_address addr,
                                           const ndgpp::net::port port,
                                           const int flags)
{
    return linuxpp::net::send(this->descriptor(), buf, length, addr, port, flags);
}

std::size_t linuxpp::net::udp_socket::send(void const * const buf,
                                           const std::size_t length,
                                           const struct sockaddr_in sockaddr,
                                           const int flags)
{
    return linuxpp::net::send(this->descriptor(), buf, length, sockaddr, flags);
}

std::size_t linuxpp::net::udp_socket::send(struct iovec const * const buffers,
                                           const std::size_t size_buffers,
                                           const ndgpp::net::ipv4_address address,
                                           const ndgpp::net::port port,
                                           const int flags)
{
    return linuxpp::net::send(this->descriptor(), buffers, size_buffers, address, port, flags);
}

std::size_t linuxpp::net::udp_socket::send(struct iovec const * const buffers,
                                           const std::size_t size_buffers,
                                           const struct sockaddr_in sockaddr,
                                           const int flags)
{
    return linuxpp::net::send(this->descriptor(), buffers, size_buffers, sockaddr, flags);
}

int linuxpp::net::udp_socket::descriptor() const noexcept
{
    return std::get<sock_descriptor>(this->members_).get();
}

linuxpp::net::udp_socket::operator bool() const noexcept
{
    return static_cast<bool>(std::get<sock_descriptor>(this->members_));
}
