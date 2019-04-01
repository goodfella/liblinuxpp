#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/open.hpp>
#include <liblinuxpp/nocloexec.hpp>

linuxpp::syscall_return<int>
linuxpp::open(std::nothrow_t,
              linuxpp::nocloexec_t,
              char const * const path,
              const int flags,
              int mode)
{
    errno = 0;
    const int ret = ::open(path, flags, mode);
    return linuxpp::syscall_return<int>{errno, ret};
}

int linuxpp::open(linuxpp::nocloexec_t,
                  char const * const path,
                  const int flags,
                  int mode)
{
    const auto ret = linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags, mode);
    if (!ret)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{ret.errno_value(), std::system_category()},
                          path != nullptr ? path : "path is null");
    }

    return ret.return_value();
}

linuxpp::syscall_return<int>
linuxpp::open(std::nothrow_t,
              linuxpp::nocloexec_t,
              char const * const path,
              const int flags)
{
    return linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags, 0);
}

int
linuxpp::open(linuxpp::nocloexec_t,
              char const * const path,
              const int flags)
{
    return linuxpp::open(linuxpp::nocloexec, path, flags, 0);
}

int linuxpp::open(char const * const path, const int flags)
{
    return linuxpp::open(linuxpp::nocloexec, path, flags | O_CLOEXEC);
}

linuxpp::syscall_return<int>
linuxpp::open(std::nothrow_t, char const * const path, const int flags)
{
    return linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags | O_CLOEXEC);
}

int linuxpp::open(char const * const path, const int flags, const int mode)
{
    return linuxpp::open(linuxpp::nocloexec, path, flags | O_CLOEXEC, mode);
}

linuxpp::syscall_return<int>
linuxpp::open(std::nothrow_t,
              char const * const path,
              const int flags,
              const int mode)
{
    return linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags | O_CLOEXEC, mode);
}
