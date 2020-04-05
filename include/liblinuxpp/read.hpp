#ifndef LIBLINUXPP_READ_HPP
#define LIBLINUXPP_READ_HPP

#include <unistd.h>

#include <cerrno>

#include <system_error>

#include <libndgpp/error.hpp>

namespace linuxpp
{
    /** Reads bytes from a file descriptor
     *
     *  @return The number of bytes read
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    inline std::size_t read(const int fd, void * const buf, const std::size_t length)
    {
        const ssize_t ret = ::read(fd, buf, length);
        if (ret == -1)
        {
            throw ndgpp_error(std::system_error,
                              std::error_code(errno, std::system_category()),
                              "read failed");
        }

        return static_cast<std::size_t>(ret);
    }
}

#endif
