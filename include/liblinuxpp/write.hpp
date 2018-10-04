#ifndef LIBLINUXPP_WRITE_HPP
#define LIBLINUXPP_WRITE_HPP

#include <unistd.h>

#include <cerrno>

#include <system_error>

#include <libndgpp/error.hpp>

namespace linuxpp
{
    /** Writes bytes to a file descriptor
     *
     *  @return The number of bytes written
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    inline std::size_t write(const int fd, char const * const buf, const std::size_t length)
    {
        const ssize_t ret = ::write(fd, buf, length);
        if (ret == -1)
        {
            throw ndgpp_error(std::system_error,
                              std::error_code(errno, std::system_category()),
                              "write failed");
        }

        return static_cast<std::size_t>(ret);
    }
}

#endif
