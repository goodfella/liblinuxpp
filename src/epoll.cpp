#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>
#include <liblinuxpp/epoll.hpp>

inline epoll_event make_epoll_event(const int fd, const uint32_t events)
{
    epoll_event event = {};
    event.events = events;
    event.data.fd = fd;
    return event;
}

inline epoll_event make_epoll_event(const int fd, const uint32_t events, void * const context)
{
    epoll_event event = {};
    event.events = events;
    event.data.ptr = context;
    return event;
}

inline epoll_event make_epoll_event(const int fd, const uint32_t events, const uint32_t context)
{
    epoll_event event = {};
    event.events = events;
    event.data.u32 = context;
    return event;
}

inline epoll_event make_epoll_event(const int fd, const uint32_t events, const uint64_t context)
{
    epoll_event event = {};
    event.events = events;
    event.data.u64 = context;
    return event;
}

linuxpp::epoll::epoll():
    members_(linuxpp::unique_fd<>{::epoll_create1(EPOLL_CLOEXEC)}, 0)
{
    if (!std::get<epoll_fd>(this->members_))
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "epoll_create failed");
    }
}

linuxpp::epoll::epoll(linuxpp::epoll &&) = default;
linuxpp::epoll & linuxpp::epoll::operator=(linuxpp::epoll &&) = default;

void linuxpp::epoll::add(const int fd, const uint32_t events)
{
    this->add(fd, make_epoll_event(fd, events));
}

void linuxpp::epoll::add(const int fd, const uint32_t events, void * const context)
{
    this->add(fd, make_epoll_event(fd, events, context));
}

void linuxpp::epoll::add(const int fd, const uint32_t events, const uint32_t context)
{
    this->add(fd, make_epoll_event(fd, events, context));
}

void linuxpp::epoll::add(const int fd, const uint32_t events, const uint64_t context)
{
    this->add(fd, make_epoll_event(fd, events, context));
}

linuxpp::syscall_return<int> linuxpp::epoll::del(std::nothrow_t,
                                                 const int fd)
{
    const int ret = ::epoll_ctl(std::get<epoll_fd>(this->members_).get(),  EPOLL_CTL_DEL, fd, nullptr);
    if (ret != 0)
    {
        return linuxpp::syscall_return<int> {errno, ret};
    }

    --std::get<size_events>(this->members_);
    return linuxpp::syscall_return<int>(0);
}

void linuxpp::epoll::del(const int fd)
{
    const auto ret = this->del(std::nothrow, fd);
    if (!ret)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{ret.errno_value(), std::system_category()},
                          "epoll_ctl::EPOLL_CTL_DEL failed");
    }
}


void linuxpp::epoll::mod(const int fd, const uint32_t events)
{
    this->mod(fd, make_epoll_event(fd, events));
}

void linuxpp::epoll::mod(const int fd, const uint32_t events, void * const context)
{
    this->mod(fd, make_epoll_event(fd, events, context));
}

void linuxpp::epoll::mod(const int fd, const uint32_t events, const uint32_t context)
{
    this->mod(fd, make_epoll_event(fd, events, context));
}

void linuxpp::epoll::mod(const int fd, const uint32_t events, const uint64_t context)
{
    this->mod(fd, make_epoll_event(fd, events, context));
}

void linuxpp::epoll::wait(std::vector<epoll_event>& events)
{
    this->wait(events, -1);
}

void linuxpp::epoll::wait(std::vector<epoll_event>& events, const std::chrono::milliseconds timeout)
{
    this->wait(events, timeout.count());
}

std::vector<epoll_event> linuxpp::epoll::wait()
{
    std::vector<epoll_event> events;
    this->wait(events, -1);
    return events;
}

std::vector<epoll_event> linuxpp::epoll::wait(const std::chrono::milliseconds timeout)
{
    std::vector<epoll_event> events;
    this->wait(events, timeout.count());
    return events;
}

void linuxpp::epoll::swap(linuxpp::epoll& other) noexcept
{
    std::swap(this->members_, other.members_);
}

void linuxpp::epoll::add(const int fd, epoll_event event)
{
    const int ret = ::epoll_ctl(std::get<epoll_fd>(this->members_).get(),
                                EPOLL_CTL_ADD,
                                fd,
                                &event);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "epoll_ctl::EPOLL_CTL_ADD failed");
    }

    ++std::get<size_events>(this->members_);
}

void linuxpp::epoll::mod(const int fd, epoll_event event)
{
    const int ret = ::epoll_ctl(std::get<epoll_fd>(this->members_).get(),
                                EPOLL_CTL_MOD,
                                fd,
                                &event);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "epoll_ctl::EPOLL_CTL_MOD failed");
    }
}

void linuxpp::epoll::wait(std::vector<epoll_event>& events, const int timeout)
{
    events.resize(std::get<size_events>(this->members_));
    const int ret = ::epoll_wait(std::get<epoll_fd>(this->members_).get(),
                                 events.data(),
                                 events.size(),
                                 timeout);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code{errno, std::system_category()},
                          "epoll_wait failed");
    }

    events.resize(static_cast<std::size_t>(ret));
}

linuxpp::syscall_return<int> linuxpp::epoll::wait(std::nothrow_t, std::vector<epoll_event> & events)
{
    events.resize(std::get<size_events>(this->members_));
    const int ret = ::epoll_wait(std::get<epoll_fd>(this->members_).get(),
                                 events.data(),
                                 events.size(),
                                 -1);
    if (ret == -1)
    {
        return linuxpp::syscall_return<int> {errno, ret};
    }

    events.resize(static_cast<std::size_t>(ret));
    return linuxpp::syscall_return<int> {ret};
}
