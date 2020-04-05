#ifndef LIBLINUXPP_IOLOOP_HPP
#define LIBLINUXPP_IOLOOP_HPP

#include <signal.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <libndgpp/bool_sentry.hpp>
#include <liblinuxpp/epoll.hpp>
#include <liblinuxpp/eventfd.hpp>
#include <liblinuxpp/monotonic_timerfd.hpp>

namespace linuxpp
{
    /** An object that facilitates asynchronous I/O programming
     *
     *  The linuxpp::ioloop class supports monitoring file
     *  descriptors, arbitrary callbacks, and timer based callbacks in
     *  a single thread
     */
    class ioloop
    {
        public:

        using time_type = std::chrono::steady_clock::time_point;

        class timeout_handle;
        class periodic_timeout_handle;

        struct event_enum
        {
            enum type
            {
                read = 1,
                write = 2,
                error = 4,
            };
        };

        ioloop();

        ioloop(const ioloop &) = delete;
        ioloop & operator= (const ioloop &) = delete;

        ioloop(ioloop &&) = delete;
        ioloop & operator= (ioloop &&) = delete;

        /** Add a file descriptor handler
         *
         *  The handler will be called when one of the events
         *  specified has occured for the given file descriptor
         *
         *  @param fd The file descriptor to monitor
         *
         *  @param events The events to monitor, must be one of the
         *                events defined in ioloop::event_enum
         *
         *  @param callback The function to call when the specified
         *         event has occured on the file descriptor
         */
        void
        add_handler(const int fd,
                    const uint32_t events,
                    std::function<void (int fd, uint32_t events)> callback);

        /** Removes a file descriptor handler
         *
         *  @param fd The file descriptor who's handler to remove
         */
        void
        remove_handler(const int fd);

        linuxpp::ioloop::timeout_handle
        add_timeout(const ioloop::time_type timeout,
                    std::function<void ()> callback);

        template <class Rep, class Period>
        linuxpp::ioloop::timeout_handle
        add_timeout(const std::chrono::duration<Rep, Period> delay,
                    std::function<void ()> callback);

        template <class Rep, class Period>
        linuxpp::ioloop::periodic_timeout_handle
        add_periodic_timeout(const std::chrono::duration<Rep, Period> delay,
                             std::function<void ()> callback);

        void
        remove_timeout(const linuxpp::ioloop::timeout_handle handle);

        void
        remove_timeout(const linuxpp::ioloop::periodic_timeout_handle);

        void
        add_callback(std::function<void ()> callback);

        void
        start();

        void
        stop();


        private:

        void
        process_timeouts();

        void
        process_periodic_timeouts();

        void
        process_callbacks();

        void
        process_stop();

        linuxpp::ioloop::timeout_handle
        insert_timeout(const ioloop::time_type timeout,
                       std::function<void ()> callback);

        template <class Rep, class Period>
        linuxpp::ioloop::periodic_timeout_handle
        insert_periodic_timeout(const std::chrono::duration<Rep, Period> period,
                                std::function<void ()> callback);

        linuxpp::eventfd stop_eventfd_;

        // Timeout related data members

        struct timeout_callback;

        friend
        bool
        operator < (const timeout_callback &,
                    const timeout_callback &);

        friend
        bool
        operator < (const linuxpp::ioloop::timeout_callback &,
                    const linuxpp::ioloop::time_type);

        std::vector<linuxpp::ioloop::timeout_callback> timeouts_;
        bool processing_timeouts_ = false;
        std::vector<linuxpp::ioloop::timeout_callback> pending_timeout_additions_;
        linuxpp::monotonic_timerfd timeout_timerfd_;

        // Periodic timeout related data members

        struct periodic_timeout_callback;

        friend
        bool
        operator < (const linuxpp::ioloop::periodic_timeout_callback &,
                    const linuxpp::ioloop::periodic_timeout_callback &);

        friend
        bool
        operator < (const linuxpp::ioloop::periodic_timeout_callback &,
                    const linuxpp::ioloop::time_type);

        std::vector<linuxpp::ioloop::periodic_timeout_callback> periodic_timeouts_;
        bool processing_periodic_timeouts_ = false;
        std::vector<linuxpp::ioloop::periodic_timeout_callback> pending_periodic_timeout_additions_;
        linuxpp::monotonic_timerfd periodic_timeout_timerfd_;


        // Callback related members

        // protects new_callbacks_
        std::mutex mutex_;
        std::vector<std::function<void ()>> new_callbacks_;
        linuxpp::eventfd callbacks_eventfd_;

        // File descriptor related members

        struct handler_callback
        {
            handler_callback(const uint32_t events,
                             std::function<void (int, uint32_t)> callback):
                events(events),
                callback(std::move(callback))
            {}

            uint32_t events;
            std::function<void (int, uint32_t)> callback;
        };

        using handler_map_type = std::unordered_map<int, handler_callback>;
        handler_map_type handlers_;
        bool processing_handlers_ = false;

        std::vector<int> removed_handlers_;

        std::vector<epoll_event> epoll_events_;
        linuxpp::epoll epoll_;

        volatile sig_atomic_t keep_running_ = 1;
    };

    class ioloop::timeout_handle
    {
        public:

        timeout_handle();

        timeout_handle(const timeout_handle &) = default;
        timeout_handle & operator =(const timeout_handle &) = default;

        timeout_handle(timeout_handle &&) = default;
        timeout_handle & operator= (timeout_handle &&) = default;

        friend
        bool
        operator == (const timeout_handle lhs,
                     const timeout_handle rhs);

        friend
        bool
        operator != (const timeout_handle lhs,
                     const timeout_handle rhs);

        private:

        static unsigned long long next_id_;
        unsigned long long id_;
    };


    struct ioloop::timeout_callback
    {
        friend
        bool
        operator < (const timeout_callback &,
                    const timeout_callback &);

        friend
        bool
        operator < (const timeout_callback &,
                    const ioloop::time_type);

        timeout_callback(linuxpp::ioloop::timeout_handle h,
                         const ioloop::time_type to,
                         std::function<void ()> cb):
            handle(h),
            timeout(to),
            callback(cb)
        {}

        linuxpp::ioloop::timeout_handle handle;
        ioloop::time_type timeout;
        std::function<void ()> callback;
        bool remove = false;
    };


    class ioloop::periodic_timeout_handle
    {
        friend
        bool
        operator == (const periodic_timeout_handle lhs,
                     const periodic_timeout_handle rhs);

        friend
        bool
        operator != (const periodic_timeout_handle lhs,
                     const periodic_timeout_handle rhs);

        friend
        bool
        operator < (const periodic_timeout_callback &,
                    const periodic_timeout_callback &);

        friend
        bool
        operator < (const periodic_timeout_callback &,
                    const linuxpp::ioloop::time_type);
      public:

        periodic_timeout_handle();

      private:

        static unsigned long long next_id_;
        unsigned long long id_;
    };

    struct ioloop::periodic_timeout_callback
    {
        template <class Rep, class Period>
            periodic_timeout_callback(const linuxpp::ioloop::periodic_timeout_handle h,
                                      const std::chrono::duration<Rep, Period> p,
                                      const ioloop::time_type to,
                                      std::function<void ()> cb):
            handle(h),
            period(std::chrono::duration_cast<std::chrono::nanoseconds>(p)),
            timeout(to),
            callback(std::move(cb))
        {}

        void
            reset(const linuxpp::ioloop::time_type now)
        {
            this->timeout = now + this->period;
            this->processed = false;
        }

        void invoke()
        {
            this->processed = true;
            this->callback();
        }

        linuxpp::ioloop::periodic_timeout_handle handle;
        typename std::chrono::nanoseconds period;
        linuxpp::ioloop::time_type timeout;
        std::function<void ()> callback;
        bool remove = false;
        bool processed = false;
    };

    template <class Rep, class Period>
    inline
    linuxpp::ioloop::timeout_handle
    ioloop::add_timeout(const std::chrono::duration<Rep, Period> delay,
                        std::function<void ()> callback)
    {
        return this->add_timeout(std::chrono::steady_clock::now() + delay,
                                 callback);
    }

    template <class Rep, class Period>
    linuxpp::ioloop::periodic_timeout_handle
    ioloop::insert_periodic_timeout(const std::chrono::duration<Rep, Period> period,
                                    std::function<void ()> callback)
    {
        const auto timeout = std::chrono::steady_clock::now() + period;
        const auto later_timeout = std::lower_bound(this->periodic_timeouts_.cbegin(),
                                                    this->periodic_timeouts_.cend(),
                                                    timeout);

        auto entry = this->periodic_timeouts_.emplace(later_timeout,
                                                      linuxpp::ioloop::periodic_timeout_handle {},
                                                      period,
                                                      timeout,
                                                      callback);
        return entry->handle;
    }

    template <class Rep, class Period>
    linuxpp::ioloop::periodic_timeout_handle
    ioloop::add_periodic_timeout(const std::chrono::duration<Rep, Period> period,
                                 std::function<void ()> callback)
    {
        if (this->processing_periodic_timeouts_)
        {
            // Periodic timeouts are currently being processed, so add
            // the element to the pending vector
            this->pending_periodic_timeout_additions_.emplace_back(linuxpp::ioloop::periodic_timeout_handle {},
                                                                   period,
                                                                   std::chrono::steady_clock::now() + period,
                                                                   callback);

            return this->pending_periodic_timeout_additions_.back().handle;
        }

        if (this->periodic_timeouts_.empty())
        {
            this->periodic_timeouts_.emplace_back(linuxpp::ioloop::periodic_timeout_handle {},
                                                  period,
                                                  std::chrono::steady_clock::now() + period,
                                                  std::move(callback));

            this->periodic_timeout_timerfd_.set_oneshot(this->periodic_timeouts_.front().timeout);
            return this->pending_periodic_timeout_additions_.front().handle;
        }

        // At this point we're not processing periodic timeouts, and
        // timeouts already exist, so see if we'll need to re-arm the
        // timer based on the new timeout

        const auto current_earliest_timeout = this->periodic_timeouts_.front().timeout;
        const auto handle = this->insert_periodic_timeout(period, callback);

        if (this->periodic_timeouts_.front().timeout < current_earliest_timeout)
        {
            this->periodic_timeout_timerfd_.set_oneshot(this->periodic_timeouts_.front().timeout);
        }

        return handle;
    }

    inline
    bool
    operator == (const ioloop::ioloop::timeout_handle lhs,
                 const ioloop::ioloop::timeout_handle rhs)
    {
        return lhs.id_ == rhs.id_;
    }

    inline
    bool
    operator != (const linuxpp::ioloop::timeout_handle lhs,
                 const linuxpp::ioloop::timeout_handle rhs)
    {
        return !(lhs == rhs);
    }

    inline
    bool
    operator == (const linuxpp::ioloop::periodic_timeout_handle lhs,
                 const linuxpp::ioloop::periodic_timeout_handle rhs)
    {
        return lhs.id_ == rhs.id_;
    }

    inline
    bool
    operator != (const linuxpp::ioloop::periodic_timeout_handle lhs,
                 const linuxpp::ioloop::periodic_timeout_handle rhs)
    {
        return !(lhs == rhs);
    }

    inline
    bool
    operator < (const linuxpp::ioloop::timeout_callback & lhs,
                const linuxpp::ioloop::timeout_callback & rhs)
    {
        return lhs.timeout < rhs.timeout;
    }

    inline
    bool
    operator < (const linuxpp::ioloop::timeout_callback & lhs,
                const linuxpp::ioloop::time_type timeout)
    {
        return lhs.timeout < timeout;
    }

    inline
    bool
    operator < (const linuxpp::ioloop::periodic_timeout_callback & lhs,
                const linuxpp::ioloop::periodic_timeout_callback & rhs)
    {
        return lhs.timeout < rhs.timeout;
    }

    inline
    bool
    operator < (const linuxpp::ioloop::periodic_timeout_callback & lhs,
                const linuxpp::ioloop::time_type timeout)
    {
        return lhs.timeout < timeout;
    }
}

#endif
