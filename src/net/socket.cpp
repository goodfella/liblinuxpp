#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/socket.hpp>

linuxpp::unique_fd<> linuxpp::net::socket(const int domain, const int type, const int protocol)
{
    linuxpp::unique_fd<> fd{::socket(domain, type | SOCK_CLOEXEC, protocol)};
    if (!fd)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to open UDP socket");
    }

    return fd;
}

void linuxpp::net::getsockname(const int sd, struct ::sockaddr_in & sockaddr)
{
    socklen_t socklen = sizeof(sockaddr);
    const int ret = ::getsockname(sd, reinterpret_cast<struct ::sockaddr *>(&sockaddr), &socklen);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "getsockname failed");
    }
}

std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> linuxpp::net::getsockname_ipv4(const int sd)
{
    struct ::sockaddr_in sockaddr;
    linuxpp::net::getsockname(sd, sockaddr);

    return std::make_tuple(ndgpp::net::ipv4_address{ntohl(sockaddr.sin_addr.s_addr)}, ntohs(sockaddr.sin_port));
}

void linuxpp::net::listen(const int sd, const int backlog)
{
    const int ret = ::listen(sd, backlog);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "listen system call failed");
    }
}
