#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/open.hpp>
#include <liblinuxpp/nocloexec.hpp>

std::tuple<int, std::error_code>
linuxpp::open(std::nothrow_t,
              linuxpp::nocloexec_t,
              char const * const path,
              const int flags,
              int mode)
{
    errno = 0;
    const int ret = ::open(path, flags, mode);
    return std::make_tuple(ret, std::error_code {errno, std::system_category ()});
}

int linuxpp::open(linuxpp::nocloexec_t,
                  char const * const path,
                  const int flags,
                  int mode)
{
    const auto ret = linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags, mode);
    const std::error_code error_code = std::get<std::error_code>(ret);
    if (error_code)
    {
        throw ndgpp_error(std::system_error,
                          error_code,
                          path != nullptr ? path : "path is null");
    }

    return std::get<int>(ret);
}

std::tuple<int, std::error_code>
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

std::tuple<int, std::error_code>
linuxpp::open(std::nothrow_t, char const * const path, const int flags)
{
    return linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags | O_CLOEXEC);
}

int linuxpp::open(char const * const path, const int flags, const int mode)
{
    return linuxpp::open(linuxpp::nocloexec, path, flags | O_CLOEXEC, mode);
}

std::tuple<int, std::error_code>
linuxpp::open(std::nothrow_t,
              char const * const path,
              const int flags,
              const int mode)
{
    return linuxpp::open(std::nothrow, linuxpp::nocloexec, path, flags | O_CLOEXEC, mode);
}
