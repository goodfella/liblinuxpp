#ifndef LIBLINUXPP_UNIQUE_FD_HPP
#define LIBLINUXPP_UNIQUE_FD_HPP

#include <unistd.h>
#include <type_traits>
#include <algorithm>
#include <utility>
#include <tuple>

#include "default_close.hpp"
#include "file_descriptor.hpp"

namespace linuxpp
{
    /** Provides unique ownership of a file descriptor
     *
     *  The unique_fd class uniquely owns a file descriptor and will
     *  close it upon destruction
     *
     *  @tparam Closer The function object or lvalue reference to
     *                 function or to function object, to be called
     *                 from the destructor
     */
    template <class Closer = default_close>
    class unique_fd
    {
        public:
        using closer_type = Closer;

        /** Default constructs a unique_fd
         *
         *  The internal file descriptor will be set to
         *  linuxpp::closed_fd
         */
        constexpr unique_fd() noexcept:
            members_(linuxpp::closed_fd,
                     closer_type())
        {}

        /** Constructs a unique_fd that manages the given file descriptor
         *
         *  @param fd The file descriptor to manage
         */
        explicit
        unique_fd(const int fd) noexcept:
            members_(fd,
                     closer_type())
            {}

        /** Constructs a unique_fd that manages the given file descriptor
         *
         *  @param fd The file descriptor to manage
         *
         *  @param closer A closer_type object to store as the closer
         */
        unique_fd(const int fd,
                  typename std::conditional<std::is_reference<closer_type>::value,
                  closer_type, const closer_type&>::type closer) noexcept:
            members_(fd,
                     closer)
            {}

        /** Constructs a unique_fd that manages the given file descriptor
         *
         *  @param fd The file descriptor to manage
         *
         *  @param closer A rvalue-reference to the closer_type
         */
        unique_fd(const int fd,
                  typename std::remove_reference<closer_type>::type&& closer) noexcept:
            members_(fd,
                     std::move(closer))
            {}

        /// Calls closer_type's void operator()(int fd) on the managed fd
        ~unique_fd();

        unique_fd(const unique_fd&) = delete;
        unique_fd& operator= (const unique_fd&) = delete;

        unique_fd(unique_fd&& src) noexcept;
        unique_fd& operator= (unique_fd&& src) noexcept;

        /** Releases ownership of the file descriptor
         *
         *  @return The previously owned file descriptor
         */
        int release() noexcept;

        /** Calls closer_type's void operator()(int fd) on the managed fd
         *
         *  @param fd The file descriptor to manage
         */
        void reset(const int fd = linuxpp::closed_fd) noexcept;

        void swap(unique_fd & other) noexcept;

        /// Returns the managed file descriptor
        int get() const noexcept;

        // Returns the closer
        closer_type & get_closer() noexcept;

        /// Returns the closer
        const closer_type & get_closer() const noexcept;

        /// Returns true if the managed file descriptor is not equal to linuxpp::closed_fd
        explicit operator bool() const noexcept;

        private:
        enum member_variable
        {
            fd_member,
            closer_member
        };
	
        using tuple_type = std::tuple<linuxpp::file_descriptor, closer_type>;
        tuple_type members_;
    };

    template <class Closer>
    unique_fd<Closer>::unique_fd(unique_fd&& src) noexcept = default;

    template <class Closer>
    unique_fd<Closer> & unique_fd<Closer>::operator=(unique_fd&& src) noexcept = default;

    template <class Closer>
    inline unique_fd<Closer>::~unique_fd()
    {
        this->reset();
    }

    template <class Closer>
    inline int unique_fd<Closer>::get() const noexcept
    {
        return std::get<fd_member>(members_).get();
    }

    template <class Closer>
    inline auto unique_fd<Closer>::get_closer() noexcept -> closer_type &
    {
        return std::get<closer_member>(members_);
    }

    template <class Closer>
    inline auto unique_fd<Closer>::get_closer() const noexcept -> const closer_type &
    {
        return std::get<closer_member>(members_);
    }

    template <class Closer>
    inline int unique_fd<Closer>::release() noexcept
    {
        const int fd = this->get();
        std::get<fd_member>(members_) = linuxpp::closed_fd;
        return fd;
    }

    template <class Closer>
    inline void unique_fd<Closer>::reset(const int fd) noexcept
    {
        const int old_fd = std::get<fd_member>(members_).get();
        std::get<fd_member>(members_).reset(fd);
        if (old_fd != linuxpp::closed_fd)
        {
                this->get_closer()(old_fd);
        }
    }

    template <class Closer>
    inline unique_fd<Closer>::operator bool () const noexcept
    {
        return this->get() != linuxpp::closed_fd;
    }

    template <class Closer>
    inline void unique_fd<Closer>::swap(unique_fd<Closer>& other) noexcept
    {
        std::swap(this->members_, other.members_);
    }

    template <class Closer>
    inline void swap(unique_fd<Closer>& lhs, unique_fd<Closer>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <class Closer>
    inline bool operator==(const unique_fd<Closer>& lhs, const unique_fd<Closer>& rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <class Closer>
    inline bool operator!=(const unique_fd<Closer>& lhs, const unique_fd<Closer>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class Closer>
    inline bool operator<(const unique_fd<Closer>& lhs, const unique_fd<Closer>& rhs)
    {
        return lhs.get() < rhs.get();
    }
}

#endif
