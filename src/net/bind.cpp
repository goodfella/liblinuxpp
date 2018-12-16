#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/sockaddr.hpp>
#include <liblinuxpp/net/bind.hpp>

void linuxpp::net::bind(const int sd, const ndgpp::net::ipv4_address addr, const ndgpp::net::port port)
{
    linuxpp::net::bind(sd, make_sockaddr(addr, port));
}

void linuxpp::net::bind(const int sd, const sockaddr_in addr)
{
    const int ret = ::bind(sd, reinterpret_cast<sockaddr const *>(&addr), sizeof(sockaddr_in));
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to bind socket");
    }
}
