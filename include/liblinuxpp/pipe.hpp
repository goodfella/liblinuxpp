#ifndef LIBLINUXPP_PIPE_HPP
#define LIBLINUXPP_PIPE_HPP

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <tuple>
#include <type_traits>
#include <system_error>

#include "unique_fd.hpp"

#include <libndgpp/source_location.hpp>
#include <libndgpp/error.hpp>

namespace linuxpp
{
    /** Returns a Linux pipe array
     *
     *  @param flags The flags to pass to the pipe2 system call
     *
     *  @return A std::array<int, 2> with the same semantics as the
     *          int array returned from pipe2
     *
     *  @throw std::exception if an error is encountered
     */
    std::array<int, 2> pipe_fd(int flags = O_CLOEXEC);

    /** Returns a Linux pipe array
     *
     *  @param flags The flags to pass to the pipe2 system call
     *
     *  @return A std::array<linuxpp::unique_fd<>, 2>
     *
     *  @throw std::exception if an error is encountered
     */
    std::array<unique_fd<>, 2> pipe_unique_fd(int flags = O_CLOEXEC);

    /// Represents a Linux pipe
    class pipe
    {
        public:

        pipe(const int flags = O_CLOEXEC);
        ~pipe() = default;

        pipe(const pipe&) = delete;
        pipe& operator=(const pipe&) = delete;

        pipe(pipe&& other) = default;
        pipe& operator=(pipe&& other) = default;

        void swap(pipe& other) noexcept;

        const unique_fd<>& read_fd() const noexcept;
        const unique_fd<>& write_fd() const noexcept;

        unique_fd<>& read_fd() noexcept;
        unique_fd<>& write_fd() noexcept;

        std::size_t write(void const * const buf, const std::size_t size, int& errno_val) noexcept;
        std::size_t write(void const * const buf, const std::size_t size);

        std::size_t read(void * const buf, const std::size_t size, int& errno_val) noexcept;
        std::size_t read(void * const buf, const std::size_t size);

        template <class T>
        auto write(const T& obj, int& errno_value) noexcept -> typename std::enable_if<std::is_pod<T>::value>::type;

        template <class T>
        auto write(const T& obj) -> typename std::enable_if<std::is_pod<T>::value>::type;


        template <class T>
        auto read(int& errno_value) noexcept -> typename std::enable_if<std::is_pod<T>::value, T>::type;

        template <class T>
        auto read() -> typename std::enable_if<std::is_pod<T>::value, T>::type;

        private:

        enum member_variable
        {
            read_fd_member,
            write_fd_member,
        };

        std::array<linuxpp::unique_fd<>, 2> members_;
    };

    void swap(pipe& lhs, pipe& rhs) noexcept;
}

inline const linuxpp::unique_fd<>& linuxpp::pipe::read_fd() const noexcept
{
    return std::get<read_fd_member>(this->members_);
}

inline linuxpp::unique_fd<>& linuxpp::pipe::read_fd() noexcept
{
    return std::get<read_fd_member>(this->members_);
}

inline const linuxpp::unique_fd<>& linuxpp::pipe::write_fd() const noexcept
{
    return std::get<write_fd_member>(this->members_);
}

inline linuxpp::unique_fd<>& linuxpp::pipe::write_fd() noexcept
{
    return std::get<write_fd_member>(this->members_);
}

template <class T>
auto linuxpp::pipe::write(const T& obj, int& errno_val) noexcept -> typename std::enable_if<std::is_pod<T>::value>::type
{
    const std::size_t ret = this->write(&obj, sizeof(obj), errno_val);
    if (errno_val != 0)
    {
        // If an error was encountered return its errno value
        return;
    }

    if (ret != sizeof(obj))
    {
        // an error wasn't encountered, but for some reason the full
        // object was not written
        errno_val = EAGAIN;
    }
}

template <class T>
auto linuxpp::pipe::write(const T& obj) -> typename std::enable_if<std::is_pod<T>::value>::type
{
    const int errno_val = [&](){
        int errno_val = 0;
        this->write(obj, errno_val);
        return errno_val;
    }();

    if (errno_val != 0)
    {
        throw ndgpp_error(std::system_error,
                          errno_val,
                          std::system_category(),
                          "object write failed");
    }
}

template <class T>
auto linuxpp::pipe::read(int& errno_val) noexcept -> typename std::enable_if<std::is_pod<T>::value, T>::type
{
    const std::tuple<T, std::size_t> ret = [&](){
        T t;
        const std::size_t ret = this->read(&t, sizeof(t), errno_val);
        return std::make_tuple(t, ret);
    }();

    if (errno_val != 0)
    {
        return std::get<1>(ret);
    }

    if (std::get<1>(ret) != sizeof(T))
    {
        errno_val = EAGAIN;
    }

    return std::get<1>(ret);
}

template <class T>
inline auto linuxpp::pipe::read() -> typename std::enable_if<std::is_pod<T>::value, T>::type
{
    const std::tuple<int, T> ret = [&](){
        int errno_val = 0;
        const T t = this->read<T>(errno_val);
        return std::make_tuple(errno_val, t);
    }();

    if (std::get<0>(ret) != 0)
    {
        throw ndgpp_error(std::system_error,
                          std::get<0>(ret),
                          std::system_category(),
                          "object read failed");
    }

    return std::get<1>(ret);
}

inline void linuxpp::pipe::swap(linuxpp::pipe& other) noexcept
{
    std::swap(this->members_, other.members_);
}

inline void linuxpp::swap(linuxpp::pipe& lhs, linuxpp::pipe& rhs) noexcept
{
    lhs.swap(rhs);
}

#endif
