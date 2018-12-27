#include <libndgpp/error.hpp>

#include <liblinuxpp/net/sockopt.hpp>

int linuxpp::net::getsockopt(const int sd, const int level, const int optname, void * const optval, ::socklen_t * optlen)
{
    const int ret = ::getsockopt(sd, level, optname, optval, optlen);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "getsockopt failed");
    }

    return ret;
}

int linuxpp::net::setsockopt(const int sd, const int level, const int optname, void const * const optval, const ::socklen_t optlen)
{
    const int ret = ::setsockopt(sd, level, optname, optval, optlen);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "setsockopt failed");
    }

    return ret;
}
