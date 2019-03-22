#include <unistd.h>

#include <cerrno>
#include <array>
#include <system_error>
#include <tuple>
#include <utility>

#include <liblinuxpp/pipe.hpp>
#include <liblinuxpp/unique_fd.hpp>

#include <libndgpp/source_location.hpp>
#include <libndgpp/error.hpp>

std::array<int, 2> linuxpp::pipe_fd(linuxpp::nocloexec_t, const int flags)
{
    std::array<int, 2> pipeFds = {};
    const int ret = ::pipe2(pipeFds.data(), flags);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          errno,
                          std::system_category(),
                          "pipe2 failed");
    }

    return pipeFds;
}

std::array<int, 2> linuxpp::pipe_fd(const int flags)
{
    return linuxpp::pipe_fd(linuxpp::nocloexec, flags | O_CLOEXEC);
}

std::array<linuxpp::unique_fd<>, 2> linuxpp::pipe_unique_fd(linuxpp::nocloexec_t, const int flags)
{
    const std::array<int, 2> pipeFds = linuxpp::pipe_fd( linuxpp::nocloexec, flags);
    return std::array<linuxpp::unique_fd<>, 2>{linuxpp::unique_fd<>{pipeFds[0]},
                                               linuxpp::unique_fd<>{pipeFds[1]}};
}

std::array<linuxpp::unique_fd<>, 2> linuxpp::pipe_unique_fd(const int flags)
{
    return linuxpp::pipe_unique_fd(linuxpp::nocloexec, flags | O_CLOEXEC);
}

linuxpp::pipe::pipe(const int flags):
    members_{linuxpp::pipe_unique_fd(flags)}
{}

linuxpp::pipe::pipe(linuxpp::nocloexec_t, const int flags):
    members_{linuxpp::pipe_unique_fd(linuxpp::nocloexec, flags)}
{}

linuxpp::pipe::pipe(pipe&&) noexcept = default;

linuxpp::pipe&
linuxpp::pipe::operator=(pipe&&) noexcept = default;

std::size_t linuxpp::pipe::write(void const * const buf, const std::size_t size, int& errno_val) noexcept
{
    const ssize_t ret = ::write(this->write_fd().get(), buf, size);
    if (ret == -1)
    {
        errno_val = errno;
        return 0;
    }

    return ret;
}

std::size_t linuxpp::pipe::write(void const * const buf, const std::size_t size)
{
    const std::tuple<int, std::size_t> ret = [&](){
        int errno_val = 0;
        const std::size_t ret = this->write(buf, size, errno_val);
        return std::make_tuple(errno_val, ret);
    }();

    if (std::get<0>(ret) != 0)
    {
        throw ndgpp_error(std::system_error,
                          std::get<0>(ret),
                          std::system_category(),
                          "write system call failed");
    }

    return std::get<1>(ret);
}

std::size_t linuxpp::pipe::read(void * const buf, const std::size_t size, int& errno_val) noexcept
{
    const ssize_t ret = ::read(this->read_fd().get(), buf, size);
    if (ret == -1)
    {
        errno_val = errno;
        return 0;
    }

    return ret;
}

std::size_t linuxpp::pipe::read(void * const buf, const std::size_t size)
{
    const std::tuple<int, std::size_t> ret = [&](){
        int errno_val = 0;
        const std::size_t ret = this->read(buf, size, errno_val);
        return std::make_tuple(errno_val, ret);
    }();

    if (std::get<0>(ret) != 0)
    {
        throw ndgpp_error(std::system_error,
                          std::get<0>(ret),
                          std::system_category(),
                          "read system call failed");
    }

    return std::get<1>(ret);
}
