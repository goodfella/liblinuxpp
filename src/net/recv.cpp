#include <stdexcept>
#include <tuple>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/recv.hpp>
#include <liblinuxpp/net/sockaddr.hpp>

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
    std::tie(addr, port) = linuxpp::net::parse_sockaddr(sockaddr);
    return ret;
}

namespace detail
{
namespace
{
    std::size_t recv(const int sd,
                     struct iovec * const buffs,
                     const std::size_t size_buffs,
                     void * const msg_name,
                     const socklen_t size_msg_name,
                     const bool variable_name_length,
                     const int flags)
    {
        struct msghdr msghdr = {};
        msghdr.msg_name = msg_name;
        msghdr.msg_namelen = size_msg_name;
        msghdr.msg_iov = buffs;
        msghdr.msg_iovlen = size_buffs;
        msghdr.msg_control = nullptr;
        msghdr.msg_controllen = 0;

        const int ret = ::recvmsg(sd, &msghdr, flags);
        if (ret == -1)
        {
            throw ndgpp_error(std::system_error,
                              std::error_code (errno, std::system_category()),
                              "recvmsg failed");
        }

        if (!variable_name_length && msghdr.msg_namelen != size_msg_name)
        {
            throw ndgpp_error(std::invalid_argument,
                              "msg_name length missmatch in recvmsg");
        }

        return static_cast<std::size_t>(ret);
    }
}
}

std::size_t linuxpp::net::recv(const int sd,
                               struct iovec * const buffs,
                               const std::size_t size_buffs,
                               const int flags)
{
    return detail::recv(sd, buffs, size_buffs, nullptr, 0, false, flags);
}

std::size_t linuxpp::net::recv(const int sd,
                               struct iovec * const buffs,
                               const std::size_t size_buffs,
                               struct ::sockaddr_in & sockaddr,
                               const int flags)
{
    return detail::recv(sd, buffs, size_buffs, &sockaddr, sizeof(sockaddr), false, flags);
}

std::size_t linuxpp::net::recv(const int sd,
                               struct iovec * const buffs,
                               const std::size_t size_buffs,
                               ndgpp::net::ipv4_address & addr,
                               ndgpp::net::port & port,
                               const int flags)
{
    struct sockaddr_in sockaddr = {};
    const std::size_t ret = linuxpp::net::recv(sd, buffs, size_buffs, sockaddr, flags);
    std::tie(addr, port) = linuxpp::net::parse_sockaddr(sockaddr);
    return ret;
}
