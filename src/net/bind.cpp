#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/bind.hpp>
#include <liblinuxpp/net/sockaddr.hpp>
#include <liblinuxpp/net/sockopt.hpp>

void linuxpp::net::bind(const int sd,
                        const ndgpp::net::ipv4_address addr,
                        const ndgpp::net::port port,
                        const bool reuse_addr,
                        const bool reuse_port)
{
    linuxpp::net::bind(sd, make_sockaddr(addr, port), reuse_addr, reuse_port);
}

void linuxpp::net::bind(const int sd,
                        const ndgpp::net::ipv4_address addr,
                        const bool reuse_addr)
{
    linuxpp::net::bind(sd, make_sockaddr(addr), reuse_addr, false);
}

void linuxpp::net::bind(const int sd,
                        const sockaddr_in addr,
                        const bool reuse_addr,
                        const bool reuse_port)
{
    if (reuse_addr)
    {
        linuxpp::net::setsockopt<linuxpp::net::so::reuse_addr>(sd, 1);
    }

    if (reuse_port)
    {
        linuxpp::net::setsockopt<linuxpp::net::so::reuse_port>(sd, 1);
    }

    const int ret = ::bind(sd, reinterpret_cast<sockaddr const *>(&addr), sizeof(sockaddr_in));
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to bind socket");
    }
}
