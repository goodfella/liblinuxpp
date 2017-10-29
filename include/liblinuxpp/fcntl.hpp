#ifndef LIBLINUXPP_FCNTL_HPP
#define LIBLINUXPP_FCNTL_HPP

#include <unistd.h>
#include <fcntl.h>

#include <cerrno>
#include <utility>

#include <libndgpp/error.hpp>

namespace linuxpp
{
    template <class ... Args>
    int fcntl(const int fd, const int cmd, Args&& ... args);
}

template <class ... Args>
int linuxpp::fcntl(const int fd, const int cmd, Args&& ... args)
{
    const int ret = ::fcntl(fd, cmd, std::forward<Args>(args)...);
    if (ret == -1)
    {
        throw NDGPP_ERROR(std::system_error,
                          errno,
                          std::system_category(),
                          "fcntl failed");
    }

    return ret;
}

#endif
