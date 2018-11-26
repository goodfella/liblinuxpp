#ifndef LIBLINUXPP_EPOLL_HPP
#define LIBLINUXPP_EPOLL_HPP

#include <sys/epoll.h>

#include <cstdint>

#include <chrono>
#include <tuple>
#include <vector>

#include <liblinuxpp/unique_fd.hpp>

namespace linuxpp
{
    /** Manages an epoll file descriptor
     *
     *  @par Copy Semantics Non-copyable
     */
    class epoll
    {
        public:

        /// Constructs an epoll object using EPOLL_CLOEXEC
        epoll();

        epoll(const epoll&) = delete;
        epoll& operator= (const epoll&) = delete;

        /** Adds a file descriptor to the epoll fd
         *
         *  Assigns epoll_event::fd to fd
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void add(const int fd, const uint32_t events);

        /** Adds a file descriptor to the epoll fd
         *
         *  Assigns epoll_event::ptr to context
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void add(const int fd, const uint32_t events, void * const context);

        /** Adds a file descriptor to the epoll fd
         *
         *  Assigns epoll_event::u32 to context
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void add(const int fd, const uint32_t events, const uint32_t context);

        /** Adds a file descriptor to the epoll fd
         *
         *  Assigns epoll_event::u64 to context
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void add(const int fd, const uint32_t events, const uint64_t context);

        /** Deletes fd from the epoll fd
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void del(const int fd);

        /** Modifies a file descriptor's epoll_event object
         *
         *  Assigns epoll_event::fd to fd
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void mod(const int fd, const uint32_t events);

        /** Modifies a file descriptor's epoll_event object
         *
         *  Assigns epoll_event::ptr to context
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void mod(const int fd, const uint32_t events, void * const context);

        /** Modifies a file descriptor's epoll_event object
         *
         *  Assigns epoll_event::u32 to context
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void mod(const int fd, const uint32_t events, const uint32_t context);

        /** Modifies a file descriptor's epoll_event object
         *
         *  Assigns epoll_event::u64 to context
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void mod(const int fd, const uint32_t events, const uint64_t context);

        /** Waits an unlimited amount of time for an event on the monitored file descriptors
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        std::vector<epoll_event> wait();

        /** Waits for an event on the monitored file descriptors
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        std::vector<epoll_event> wait(const std::chrono::milliseconds timeout);

        /** Waits an unlimited amount of time for an event on the monitored file descriptors
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void wait(std::vector<epoll_event>& events);

        /** Waits for an event on the monitored file descriptors
         *
         *  @throws ndgpp::error<std::system_error> if an error is encountered
         */
        void wait(std::vector<epoll_event>& events,
                  const std::chrono::milliseconds timeout);

        /// Swaps this with the provided epoll object
        void swap(linuxpp::epoll& other);

        private:

        void add(const int fd, epoll_event event);
        void mod(const int fd, epoll_event event);
        void wait(std::vector<epoll_event>& events, const int timeout);

        enum members
        {
            epoll_fd,
            size_events
        };

        std::tuple<linuxpp::unique_fd<>, std::size_t> members_;
    };
}

#endif
