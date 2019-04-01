#ifndef LIBLINUXPP_PIPE_HPP
#define LIBLINUXPP_PIPE_HPP

#include <fcntl.h>
#include <unistd.h>

#include <array>
#include <new>
#include <tuple>
#include <type_traits>
#include <system_error>

#include "unique_fd.hpp"
#include "nocloexec.hpp"
#include <libndgpp/source_location.hpp>
#include <libndgpp/error.hpp>

#include <liblinuxpp/syscall_return.hpp>

namespace linuxpp
{
    /** Returns a Linux pipe array with the O_CLOEXEC flag set
     *
     *  @param flags The flags to pass to the pipe2 system call.
     *               O_CLOEXEC is bitwise or'd to this parameter.
     *
     *  @return A std::array<int, 2> with the same semantics as the
     *          int array returned from pipe2
     *
     *  @throw std::exception if an error is encountered
     */
    std::array<int, 2> pipe_fd(int flags);

    /** Returns a Linux pipe array
     *
     *  @param flags The flags to pass to the pipe2 system call.
     *
     *  @return A std::array<int, 2> with the same semantics as the
     *          int array returned from pipe2
     *
     *  @throw std::exception if an error is encountered
     */
    std::array<int, 2> pipe_fd(linuxpp::nocloexec_t, int flags);


    /** Returns a Linux pipe array with the O_CLOEXEC flag set
     *
     *  @param flags The flags to pass to the pipe2 system call.
     *               O_CLOEXEC is bitwised or'd to to this parameter.
     *
     *  @return A std::array<linuxpp::unique_fd<>, 2>
     *
     *  @throw std::exception if an error is encountered
     */
    std::array<unique_fd<>, 2> pipe_unique_fd(int flags);

    /** Returns a Linux pipe array
     *
     *  @param flags The flags to pass to the pipe2 system call.
     *
     *  @return A std::array<linuxpp::unique_fd<>, 2>
     *
     *  @throw std::exception if an error is encountered
     */
    std::array<unique_fd<>, 2> pipe_unique_fd(linuxpp::nocloexec_t, int flags);

    /// Represents a Linux pipe
    class pipe final
    {
        public:

        explicit
        pipe(const int flags = 0);

        explicit
        pipe(linuxpp::nocloexec_t, const int flags);

        ~pipe() = default;

        pipe(const pipe&) = delete;
        pipe& operator=(const pipe&) = delete;

        pipe(pipe&& other) noexcept;
        pipe& operator=(pipe&& other) noexcept;

        void swap(pipe& other) noexcept;

        const unique_fd<>& read_fd() const noexcept;
        const unique_fd<>& write_fd() const noexcept;

        unique_fd<>& read_fd() noexcept;
        unique_fd<>& write_fd() noexcept;

        linuxpp::syscall_return<ssize_t>
        write(std::nothrow_t, void const * const buf, const std::size_t size) noexcept;

        std::size_t write(void const * const buf, const std::size_t size);

        linuxpp::syscall_return<ssize_t>
        read(std::nothrow_t, void * const buf, const std::size_t size) noexcept;

        std::size_t read(void * const buf, const std::size_t size);

        template <class T>
        auto
        write(std::nothrow_t, const T& obj) noexcept ->
            typename std::enable_if<std::is_pod<T>::value, linuxpp::syscall_return<ssize_t>>::type;

        template <class T>
        auto write(const T& obj) -> typename std::enable_if<std::is_pod<T>::value>::type;


        template <class T>
        auto
        read(std::nothrow_t) noexcept ->
            typename std::enable_if<std::is_pod<T>::value, linuxpp::syscall_return<T>>::type;

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
auto linuxpp::pipe::write(std::nothrow_t, const T& obj) noexcept ->
    typename std::enable_if<std::is_pod<T>::value, linuxpp::syscall_return<ssize_t>>::type
{
    const auto ret = this->write(std::nothrow, obj, sizeof(obj));
    if (!ret || ret.return_value() != sizeof(obj))
    {
        return linuxpp::syscall_return<ssize_t>(EAGAIN, ret.return_value());
    }

    return linuxpp::syscall_return<ssize_t>(0, ret.return_value());
}

template <class T>
auto linuxpp::pipe::write(const T& obj) -> typename std::enable_if<std::is_pod<T>::value>::type
{
    const auto ret = this->write(std::nothrow, obj);
    if (!ret)
    {
        throw ndgpp_error(std::system_error,
                          ret.errno_value(),
                          std::system_category(),
                          "object write failed");
    }

    if (ret.return_value() != sizeof(obj))
    {
        throw ndgpp_error(std::system_error,
                          EAGAIN,
                          std::system_category(),
                          "partial object write");
    }
}

template <class T>
auto linuxpp::pipe::read(std::nothrow_t) noexcept ->
    typename std::enable_if<std::is_pod<T>::value, linuxpp::syscall_return<T>>::type
{
    T t;
    const auto ret = this->read(std::nothrow, &t, sizeof(t));
    if (!ret)
    {
        return linuxpp::syscall_return<T>(linuxpp::seterrno, ret.errno_value());
    }

    if (ret.return_value() != sizeof(t))
    {
        return linuxpp::syscall_return<T>(linuxpp::seterrno, ENODATA);
    }

    return ret;
}

template <class T>
inline auto linuxpp::pipe::read() -> typename std::enable_if<std::is_pod<T>::value, T>::type
{
    const auto ret = this->read<T>(std::nothrow);
    if (!ret)
    {
        throw ndgpp_error(std::system_error,
                          ret.errno_value(),
                          std::system_category(),
                          "failed to read object");
    }

    return ret.return_value();
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
