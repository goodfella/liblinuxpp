#include <tuple>

#include <liblinuxpp/net/accept.hpp>
#include <liblinuxpp/net/sockaddr.hpp>

namespace detail
{
    int accept(const int sd,
               struct ::sockaddr * sockaddr,
               socklen_t * socklen,
               const int flags = 0)
    {
        const int ret = ::accept4(sd, sockaddr, socklen, flags);
        if (ret == -1)
        {
            throw ndgpp_error(std::system_error,
                              std::error_code (errno, std::system_category()),
                              "accept system call failed");
        }

        return ret;
    }
}

int linuxpp::net::accept(const int sd,
                         const int flags)
{
    return detail::accept(sd, nullptr, nullptr, flags | SOCK_CLOEXEC);
}

int linuxpp::net::accept(const int sd,
                         struct ::sockaddr_in & sockaddr,
                         const int flags)
{
    ::socklen_t socklen = sizeof(sockaddr);
    const int ret = detail::accept(sd,
                                   reinterpret_cast<struct ::sockaddr *>(&sockaddr),
                                   &socklen,
                                   flags | SOCK_CLOEXEC);

    if (socklen != sizeof(sockaddr))
    {
        throw ndgpp_error(std::runtime_error,
                          "accept system call returned invalid socklen_t");
    }

    return ret;
}

int linuxpp::net::accept(const int sd,
                         ndgpp::net::ipv4_address & addr,
                         ndgpp::net::port & port,
                         const int flags)
{
    struct ::sockaddr_in sockaddr = {};
    const int ret = linuxpp::net::accept(sd, sockaddr);
    std::tie(addr, port) = linuxpp::net::parse_sockaddr(sockaddr);
    return ret;
}
