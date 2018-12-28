#include <stdexcept>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/recv.hpp>

std::size_t linuxpp::net::recv(const int sd,
                               void * buf,
                               const std::size_t len,
                               const int flags)
{
    const int ret = ::recv(sd, buf, len, flags);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "recv failed");
    }

    return static_cast<std::size_t>(ret);
}

std::size_t linuxpp::net::recv(const int sd,
                               void * buf,
                               const std::size_t len,
                               struct ::sockaddr * sockaddr,
                               ::socklen_t * addrlen,
                               const int flags)
{
    const int ret = ::recvfrom(sd, buf, len, flags, sockaddr, addrlen);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "recvfrom failed");
    }

    return static_cast<std::size_t>(ret);
}

std::size_t linuxpp::net::recv(const int sd,
                               void * buf,
                               const std::size_t len,
                               struct ::sockaddr_in & sockaddr,
                               const int flags)
{
    ::socklen_t addr_len = sizeof(sockaddr);
    const std::size_t ret = linuxpp::net::recv(sd,
                                               buf,
                                               len,
                                               reinterpret_cast<struct ::sockaddr *>(&sockaddr),
                                               &addr_len,
                                               flags);
    if (addr_len != sizeof(sockaddr))
    {
        throw ndgpp_error(std::invalid_argument,
                          "invalid sockaddr length");
    }

    return ret;
}

std::size_t linuxpp::net::recv(const int sd,
                               void * buf,
                               const std::size_t len,
                               ndgpp::net::ipv4_address & addr,
                               ndgpp::net::port & port,
                               const int flags)
{
    struct ::sockaddr_in sockaddr = {};
    const std::size_t ret = linuxpp::net::recv(sd, buf, len, sockaddr, flags);
    addr = sockaddr.sin_addr.s_addr;
    port = sockaddr.sin_port;
    return ret;
}
