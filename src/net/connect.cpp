#include <sys/types.h>
#include <sys/socket.h>

#include <liblinuxpp/net/connect.hpp>
#include <liblinuxpp/net/sockaddr.hpp>

void linuxpp::net::connect(const int sd,
                           const struct ::sockaddr_in sockaddr)
{
    const int ret = ::connect(sd,
                              reinterpret_cast<const struct ::sockaddr*>(&sockaddr),
                              sizeof(struct ::sockaddr_in));
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to connect socket");
    }
}

void linuxpp::net::connect(const int sd,
                           const ndgpp::net::ipv4_address addr,
                           const ndgpp::net::port port)
{
    const struct ::sockaddr_in sockaddr = linuxpp::net::make_sockaddr(addr, port);
    linuxpp::net::connect(sd, sockaddr);
}
