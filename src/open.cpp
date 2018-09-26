#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/open.hpp>
#include <liblinuxpp/no_cloexec.hpp>

std::tuple<int, std::error_code>
linuxpp::open(char const * const path,
              const int flags,
              int mode,
              std::nothrow_t no_throw,
              linuxpp::no_cloexec_t)

{
    errno = 0;
    const int ret = ::open(path, flags, mode);
    return std::make_tuple(ret, std::error_code {errno, std::system_category ()});
}

int linuxpp::open(char const * const path,
                  const int flags,
                  int mode,
                  linuxpp::no_cloexec_t)
{
    const auto ret = linuxpp::open(path, flags, mode, std::nothrow, linuxpp::no_cloexec); 
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
linuxpp::open(char const * const path,
              const int flags,
              std::nothrow_t,
              linuxpp::no_cloexec_t)
{
    return linuxpp::open(path, flags, 0, std::nothrow, linuxpp::no_cloexec);
}

int
linuxpp::open(char const * const path,
              const int flags,
              linuxpp::no_cloexec_t)
{
    return linuxpp::open(path, flags, 0, linuxpp::no_cloexec);
}

int linuxpp::open(char const * const path, const int flags)
{
    return linuxpp::open(path, flags | O_CLOEXEC, linuxpp::no_cloexec);
}

std::tuple<int, std::error_code>
linuxpp::open(char const * const path, const int flags, std::nothrow_t)
{
    return linuxpp::open(path, flags | O_CLOEXEC, std::nothrow, linuxpp::no_cloexec);
}

int linuxpp::open(char const * const path, const int flags, const int mode)
{
    return linuxpp::open(path, flags | O_CLOEXEC, mode, linuxpp::no_cloexec);
}

std::tuple<int, std::error_code>
linuxpp::open(char const * const path,
              const int flags,
              const int mode,
              std::nothrow_t)
{
    return linuxpp::open(path, flags | O_CLOEXEC, mode, std::nothrow, linuxpp::no_cloexec);
}