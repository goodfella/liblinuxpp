#include <arpa/inet.h>

#include <liblinuxpp/net/sockaddr.hpp>

struct sockaddr_in linuxpp::net::make_sockaddr(const ndgpp::net::ipv4_address address,
                                               const ndgpp::net::port port)
{
    sockaddr_in sockaddr = {};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port.value());
    sockaddr.sin_addr.s_addr = htonl(address.to_uint32());

    return sockaddr;
}

struct sockaddr_in linuxpp::net::make_sockaddr(const ndgpp::net::ipv4_address addr)
{
    sockaddr_in sockaddr = {};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(addr.to_uint32());

    return sockaddr;
}
