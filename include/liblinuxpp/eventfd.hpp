#ifndef LIBLINUXPP_EVENTFD_HPP
#define LIBLINUXPP_EVENTFD_HPP

#include <unistd.h>

#include <cstdint>

#include <new>

#include <liblinuxpp/syscall_return.hpp>
#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
    class eventfd
    {
        public:

        eventfd(const unsigned int initval,
                const int flags);

        eventfd(const int flags);

        eventfd();

        eventfd(const eventfd &) = delete;
        eventfd & operator= (const eventfd &) = delete;

        eventfd(eventfd &&);
        eventfd & operator= (eventfd &&);

        void
        write();

        void
        write(const uint64_t value);

        linuxpp::syscall_return<::ssize_t>
        write(std::nothrow_t);

        linuxpp::syscall_return<::ssize_t>
        write(std::nothrow_t,
              const uint64_t value);

        uint64_t
        read();

        int
        fd() const;

        private:

        linuxpp::unique_fd<> fd_;
    };
}

#endif
