#include <sys/types.h>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/send.hpp>
#include <liblinuxpp/net/sockaddr.hpp>

std::size_t linuxpp::net::send(const int sd,
                               const struct msghdr msg,
                               const int flags)
{
    const ssize_t ret = ::sendmsg(sd, &msg, flags);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "sendmsg failed");
    }

    return static_cast<std::size_t>(ret);
}

std::size_t linuxpp::net::send(const int fd,
                               struct iovec const * buffers,
                               const std::size_t size_buffers,
                               const ndgpp::net::ipv4_address address,
                               const ndgpp::net::port port,
                               const int flags)
{
    struct ::sockaddr_in sockaddr = linuxpp::net::make_sockaddr(address, port);
    struct msghdr msghdr = {};

    msghdr.msg_name = &sockaddr;
    msghdr.msg_namelen = sizeof(struct ::sockaddr_in);
    msghdr.msg_iov = const_cast<struct iovec*>(buffers);
    msghdr.msg_iovlen = size_buffers;

    return linuxpp::net::send(fd, msghdr, flags);
}

std::size_t linuxpp::net::send(const int fd,
                               const std::vector<struct iovec> & buffers,
                               const ndgpp::net::ipv4_address address,
                               const ndgpp::net::port port,
                               const int flags)
{
    return linuxpp::net::send(fd, buffers.data(), buffers.size(), address, port, flags);
}

std::size_t linuxpp::net::send(const int sd,
                               void const * const msg,
                               const std::size_t length,
                               const struct ::sockaddr_in sockaddr,
                               const int flags)
{
    const int ret = ::sendto(sd,
                             msg,
                             length,
                             flags,
                             reinterpret_cast<const struct ::sockaddr *>(&sockaddr),
                             sizeof(struct ::sockaddr_in));
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "sendmsg failed");
    }

    return static_cast<std::size_t>(ret);
}

std::size_t linuxpp::net::send(const int sd,
                               void const * const msg,
                               const std::size_t length,
                               const ndgpp::net::ipv4_address address,
                               const ndgpp::net::port port,
                               const int flags)
{
    return linuxpp::net::send(sd,
                              msg,
                              length,
                              linuxpp::net::make_sockaddr(address, port),
                              flags);
}

