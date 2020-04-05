#include <sys/eventfd.h>
#include <unistd.h>

#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>

#include <liblinuxpp/eventfd.hpp>
#include <liblinuxpp/write.hpp>

linuxpp::eventfd::eventfd(const unsigned int initvalue,
                          const int flags):
    fd_(::eventfd(initvalue, flags | EFD_CLOEXEC))
{
    if (!this->fd_)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "eventfd system call failed");
    }
}

linuxpp::eventfd::eventfd(const int flags):
    eventfd(0, flags)
{}

linuxpp::eventfd::eventfd():
    eventfd(0, 0)
{}

void
linuxpp::eventfd::write()
{
    constexpr uint64_t value = 1;
    linuxpp::write(this->fd_.get(), &value, sizeof(value));
}

void
linuxpp::eventfd::write(const uint64_t value)
{
    linuxpp::write(this->fd_.get(), &value, sizeof(value));
}

linuxpp::syscall_return<::ssize_t>
linuxpp::eventfd::write(std::nothrow_t)
{
    constexpr uint64_t value = 1;
    return linuxpp::write(std::nothrow, this->fd_.get(), &value, sizeof(value));;
}

linuxpp::syscall_return<::ssize_t>
linuxpp::eventfd::write(std::nothrow_t,
                        const uint64_t value)
{
    return linuxpp::write(std::nothrow, this->fd_.get(), &value, sizeof(value));
}

uint64_t
linuxpp::eventfd::read()
{
    uint64_t value;
    const int ret = ::read(this->fd_.get(), &value, sizeof(value));
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "eventfd read faild");
    }

    return value;
}

int
linuxpp::eventfd::fd() const
{
    return this->fd_.get();
}
