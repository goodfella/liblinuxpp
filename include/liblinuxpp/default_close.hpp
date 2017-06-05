#ifndef LIBLINUXPP_DEFAULT_CLOSE_HPP
#define LIBLINUXPP_DEFAULT_CLOSE_HPP

#include <unistd.h>

namespace linuxpp
{
    struct default_close
    {
	constexpr default_close() noexcept = default;

	void operator()(const int fd) const noexcept;
    };

    void default_close::operator()(const int fd) const noexcept
    {
	::close(fd);
    }
}

#endif
