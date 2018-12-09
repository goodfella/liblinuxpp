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

linuxpp::net::udp_socket::udp_socket(const ndgpp::net::port port):
    members_{linuxpp::unique_fd<>{linuxpp::net::socket(AF_INET, SOCK_DGRAM, 0)}}
{
    linuxpp::net::bind(this->descriptor(), port);
}

linuxpp::net::udp_socket::udp_socket(udp_socket &&) noexcept = default;
linuxpp::net::udp_socket & linuxpp::net::udp_socket::operator=(udp_socket &&) noexcept = default;

void linuxpp::net::udp_socket::close() noexcept
{
    std::get<sock_descriptor>(this->members_).reset();
}

void linuxpp::net::udp_socket::bind(const ndgpp::net::ipv4_address addr, const ndgpp::net::port port)
{
    linuxpp::net::bind(this->descriptor(), addr, port);
}

void linuxpp::net::udp_socket::bind(const ndgpp::net::port port)
{
    linuxpp::net::bind(this->descriptor(), port);
}

int linuxpp::net::udp_socket::descriptor() const noexcept
{
    return std::get<sock_descriptor>(this->members_).get();
}

linuxpp::net::udp_socket::operator bool() const noexcept
{
    return static_cast<bool>(std::get<sock_descriptor>(this->members_));
}
