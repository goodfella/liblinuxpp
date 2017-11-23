#ifndef LIBLINUXPP_FILE_DESCRIPTOR_HPP
#define LIBLINUXPP_FILE_DESCRIPTOR_HPP

#include <utility>

namespace linuxpp
{
    /// Represents a closed / invalid file descriptor
    constexpr int closed_fd = -1;

    /** Represents a file descriptor
     *
     *  This class provides move semantics for a file descriptor so
     *  other classes like sockets and unique_fd don't have to
     */
    class file_descriptor
    {
        public:

        constexpr file_descriptor(const int fd = closed_fd) noexcept;

        file_descriptor(const file_descriptor&) = default;
        file_descriptor& operator=(const file_descriptor&) = default;

        file_descriptor(file_descriptor&&) noexcept;
        file_descriptor& operator=(file_descriptor&&) noexcept;

        int get() const noexcept;

        void swap(file_descriptor& other) noexcept;

        void reset(int fd = closed_fd) noexcept;

        private:

        int fd_;
    };

    inline void swap(file_descriptor& lhs, file_descriptor& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    inline constexpr file_descriptor::file_descriptor(const int fd) noexcept:
        fd_(fd)
    {}

    inline file_descriptor::file_descriptor(file_descriptor&& other) noexcept:
        fd_(other.fd_)
    {
        other.fd_ = closed_fd;
    }

    inline file_descriptor& file_descriptor::operator=(file_descriptor&& other) noexcept
    {
        this->fd_ = other.fd_;
        other.fd_ = closed_fd;
        return *this;
    }

    inline void file_descriptor::swap(file_descriptor& other) noexcept
    {
        std::swap(this->fd_, other.fd_);
    }

    inline void file_descriptor::reset(const int fd) noexcept
    {
        this->fd_ = fd;
    }

    inline int file_descriptor::get() const noexcept
    {
        return this->fd_;
    }

    inline bool operator==(const file_descriptor lhs, const file_descriptor rhs)
    {
        return lhs.get() == rhs.get();
    }

    inline bool operator!=(const file_descriptor lhs, const file_descriptor rhs)
    {
        return !(lhs == rhs);
    }

    inline bool operator<(const file_descriptor lhs, const file_descriptor rhs)
    {
        return lhs.get() < rhs.get();
    }
}

#endif
