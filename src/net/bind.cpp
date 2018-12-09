#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <liblinuxpp/net/bind.hpp>

void linuxpp::net::bind(const int sd, const ndgpp::net::ipv4_address addr, const ndgpp::net::port port)
{
    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port.value());
    sockaddr.sin_addr.s_addr = addr.to_uint32();

    linuxpp::net::bind(sd, sockaddr);
}

void linuxpp::net::bind(const int sd, const ndgpp::net::port port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port.value());
    addr.sin_addr.s_addr = INADDR_ANY;

    linuxpp::net::bind(sd, addr);
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
