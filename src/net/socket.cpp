#include <sys/types.h>
#include <sys/socket.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/socket.hpp>

linuxpp::unique_fd<> linuxpp::net::socket(const int domain, const int type, const int protocol)
{
    linuxpp::unique_fd<> fd{::socket(domain, type | SOCK_CLOEXEC, protocol)};
    if (!fd)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to open UDP socket");
    }

    return fd;
}
