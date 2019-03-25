#ifndef LINUXPP_SYSCALL_RETURN_HPP
#define LINUXPP_SYSCALL_RETURN_HPP

#include <tuple>

namespace linuxpp
{
    struct seterrno_t {};
    constexpr seterrno_t seterrno {};

    /** Represents the return from a system call
     *
     *  @tparam T The system call return type
     */
    template <class T>
    class syscall_return
    {
        public:

        using value_type = T;

        /// Constructs a syscall_return object given its return value
        syscall_return(const T return_value) noexcept;

        /// Constructs a syscall_return object given the errno value
        syscall_return(seterrno_t, int errno_value) noexcept;

        syscall_return(const syscall_return &) noexcept;
        syscall_return(syscall_return &&) noexcept;

        syscall_return & operator= (const syscall_return &) noexcept;
        syscall_return & operator= (syscall_return &&) noexcept;

        /// Returns true if the syscall_return object was not constructed with an errno value
        explicit
        operator bool() const noexcept;

        /// See bool operator
        bool good() const noexcept;

        /// Returns true if the syscall_return object was constructed with an errno value
        bool fail() const noexcept;

        /// Returns the errno value stored in the syscall_return object
        int errno_value() const noexcept;

        /// Returns the return value of the system call
        value_type return_value() const noexcept;

        private:

        enum members
        {
            errno_member,
            return_member
        };

        using tuple_type = std::tuple<int, value_type>;

        tuple_type members_ = {};
    };

    template <class T>
    syscall_return<T>::syscall_return(const T return_value) noexcept:
        members_{std::make_tuple(0, return_value)}
    {}

    template <class T>
    syscall_return<T>::syscall_return(seterrno_t, int errno_value) noexcept:
        members_{std::make_tuple(errno_value, T{})}
    {}

    template <class T>
    syscall_return<T>::syscall_return(const syscall_return &) noexcept = default;

    template <class T>
    syscall_return<T>::syscall_return(syscall_return &&) noexcept = default;

    template <class T>
    syscall_return<T> & syscall_return<T>::operator= (const syscall_return &) noexcept = default;

    template <class T>
    syscall_return<T> & syscall_return<T>::operator= (syscall_return &&) noexcept = default;

    template <class T>
    syscall_return<T>::operator bool() const noexcept
    {
        return std::get<errno_member>(this->members_) == 0;
    }

    template <class T>
    bool syscall_return<T>::good() const noexcept
    {
        return static_cast<bool>(*this) == true;
    }

    template <class T>
    bool syscall_return<T>::fail() const noexcept
    {
        return static_cast<bool>(*this) == false;
    }

    template <class T>
    T syscall_return<T>::return_value() const noexcept
    {
        return std::get<return_member>(this->members_);
    }

    template <class T>
    int syscall_return<T>::errno_value() const noexcept
    {
        return std::get<errno_member>(this->members_);
    }
}

#endif
