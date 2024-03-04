#include <sys/eventfd.h>

#include <cerrno>

#include <algorithm>
#include <exception>
#include <iostream>
#include <mutex>
#include <new>
#include <stdexcept>
#include <utility>

#include <libndgpp/error.hpp>
#include <liblinuxpp/ioloop.hpp>

uint32_t epoll_events(uint32_t ioloop_events)
{
    return
        (ioloop_events & linuxpp::ioloop::event_enum::read ? EPOLLIN : 0) |
        (ioloop_events & linuxpp::ioloop::event_enum::write ? EPOLLOUT : 0) |
        (ioloop_events & linuxpp::ioloop::event_enum::error ? EPOLLERR : 0);
}

unsigned long long linuxpp::ioloop::timeout_handle::next_id_ = 0;

linuxpp::ioloop::timeout_handle::timeout_handle():
    id_(linuxpp::ioloop::timeout_handle::next_id_++)
{}


unsigned long long linuxpp::ioloop::periodic_timeout_handle::next_id_ = 0;

linuxpp::ioloop::periodic_timeout_handle::periodic_timeout_handle():
    id_(linuxpp::ioloop::periodic_timeout_handle::next_id_++)
{}

linuxpp::ioloop::ioloop():
    callbacks_eventfd_(EFD_NONBLOCK)
{
    this->add_handler(this->callbacks_eventfd_.fd(),
                      linuxpp::ioloop::event_enum::read,
                      std::bind(&linuxpp::ioloop::process_callbacks, this));

    this->add_handler(this->stop_eventfd_.fd(),
                      linuxpp::ioloop::event_enum::read,
                      std::bind(&linuxpp::ioloop::process_stop, this));

    this->add_handler(this->timeout_timerfd_.fd(),
                      linuxpp::ioloop::event_enum::read,
                      std::bind(&linuxpp::ioloop::process_timeouts, this));

    this->add_handler(this->periodic_timeout_timerfd_.fd(),
                      linuxpp::ioloop::event_enum::read,
                      std::bind(&linuxpp::ioloop::process_periodic_timeouts, this));
}

void
linuxpp::ioloop::process_stop()
{
    this->stop_eventfd_.read();
}

void
linuxpp::ioloop::add_callback(std::function<void ()> callback)
{
    std::lock_guard<std::mutex> lock(this->mutex_);
    this->new_callbacks_.push_back(callback);

    const auto ret = this->callbacks_eventfd_.write(std::nothrow);
    if (!ret && ret.errno_value() != EAGAIN)
    {
        // the eventfd write failed for a reason other than event
        // counter overflow
        throw ndgpp_error(std::system_error,
                          std::error_code{ret.errno_value(), std::system_category()},
                          "failed to write to ioloop's callback eventfd");
    }
}

void
linuxpp::ioloop::add_handler(const int fd,
                             const uint32_t events,
                             std::function<void (int, uint32_t)> callback)
{
    const auto ret =
        this->handlers_.emplace(fd,
                                linuxpp::ioloop::handler_callback {
                                        events,
                                        std::move(callback)});
    if (!ret.second)
    {
        throw ndgpp_error(std::runtime_error,
                          "failed to insert handler: fd is already handled");
    }

    try
    {
        this->epoll_.add(fd, ::epoll_events(events), &(*ret.first));
    }
    catch (...)
    {
        this->handlers_.erase(fd);
        std::throw_with_nested(ndgpp_error(std::runtime_error, "failed to add fd to epoll"));
    }
}

void
linuxpp::ioloop::remove_handler(const int fd)
{
    auto handler = this->handlers_.find(fd);
    if (handler == this->handlers_.end())
    {
        // nothing to do, handler is already gone
        return;
    }

    if (this->processing_handlers_)
    {
        // postpone removal until the ioloop is done processing the
        // handlers
        this->removed_handlers_.push_back(fd);
    }
    else
    {
        this->handlers_.erase(handler);
        const auto epoll_ret = this->epoll_.del(std::nothrow, fd);
        if (!epoll_ret)
        {
            if (epoll_ret.errno_value() != ENOENT)
            {
                // Something weird happened
                throw ndgpp_error(std::system_error,
                                  std::error_code{epoll_ret.errno_value(), std::system_category()},
                                  "linuxpp::epoll_.del(...) failed in linuxpp::ioloop::remove_handler");
            }

            // At this point the file descriptor was closed prior
            // to its handler being removed so the epoll_del error
            // is acceptable
        }
    }
}

linuxpp::ioloop::timeout_handle
linuxpp::ioloop::insert_timeout(const linuxpp::ioloop::time_type timeout,
                                std::function<void ()> callback)
{
    const auto later_timeout = std::lower_bound(this->timeouts_.cbegin(),
                                                this->timeouts_.cend(),
                                                timeout);

    const auto entry = this->timeouts_.emplace(later_timeout,
                                               linuxpp::ioloop::timeout_handle {},
                                               timeout,
                                               std::move(callback));
    return entry->handle;
}

linuxpp::ioloop::timeout_handle
linuxpp::ioloop::add_timeout(const linuxpp::ioloop::time_type timeout,
                             std::function<void ()> callback)
{
    if (this->processing_timeouts_)
    {
        // timeouts are being processed so add them to the temporary
        // vector

        this->pending_timeout_additions_.emplace_back(linuxpp::ioloop::timeout_handle {},
                                                      timeout,
                                                      std::move(callback));

        return this->pending_timeout_additions_.back().handle;
    }

    if (this->timeouts_.empty())
    {
        this->timeouts_.emplace_back(linuxpp::ioloop::timeout_handle {},
                                     timeout,
                                     std::move(callback));
        this->timeout_timerfd_.set_oneshot(timeout);
        return this->timeouts_.back().handle;
    }

    // At this point we're not processing timeouts and timeouts
    // already exist, so see if we'll need to re-arm the timer based
    // on the new timeout

    const auto current_earliest_timeout = this->timeouts_.front().timeout;
    const auto handle = this->insert_timeout(timeout, callback);

    if (current_earliest_timeout > timeout)
    {
        // Re-arm the timer with the earlier timeout
        this->timeout_timerfd_.set_oneshot(timeout);
    }

    return handle;
}

void
linuxpp::ioloop::remove_timeout(const linuxpp::ioloop::timeout_handle handle)
{
    for (auto it = this->timeouts_.begin();
         it != this->timeouts_.end();
         ++it)
    {
        if (it->handle == handle)
        {
            if (this->processing_timeouts_)
            {
                // This timeout is being processed, so defer its
                // removal
                it->remove = true;
            }
            else
            {
                this->timeouts_.erase(it);
            }

            return;
        }
    }

    for (auto it = this->pending_timeout_additions_.begin();
         it != this->pending_timeout_additions_.end();
         ++it)
    {
        if (it->handle == handle)
        {
            this->pending_timeout_additions_.erase(it);
            return;
        }
    }
}

void
linuxpp::ioloop::process_timeouts()
{
    this->timeout_timerfd_.read();

    {
        ndgpp::bool_sentry sentry {this->processing_timeouts_};
        this->processing_timeouts_ = true;

        auto i = this->timeouts_.begin();
        const auto now = std::chrono::steady_clock::now();
        while (i != this->timeouts_.end())
        {
            auto & timeout_callback = *i;
            ++i;

            if (timeout_callback.remove)
            {
                continue;
            }

            if (timeout_callback.timeout <= now)
            {
                // timeout expired, so call its call back
                timeout_callback.callback();
            }
            else
            {
                // this timeout and the remaining timeouts have not
                // expired so stop calling timeout callbacks
                break;
            }
        }

        // remove the exipired timeouts
        this->timeouts_.erase(this->timeouts_.begin(), i);
    }

    // Remove timeouts that were removed from a timeout callback
    this->timeouts_.erase(std::remove_if(this->timeouts_.begin(),
                                         this->timeouts_.end(),
                                         [] (const linuxpp::ioloop::timeout_callback & timeout)
                                         {
                                             return timeout.remove == true;
                                         }),
                          this->timeouts_.end());

    // Add new timeouts
    for (auto & new_timeout : this->pending_timeout_additions_)
    {
        const auto later_timeout = std::lower_bound(this->timeouts_.cbegin(),
                                                    this->timeouts_.cend(),
                                                    new_timeout);
        this->timeouts_.insert(later_timeout,
                               std::move(new_timeout));
    }

    this->pending_timeout_additions_.clear();

    if (!this->timeouts_.empty())
    {
        this->timeout_timerfd_.set_oneshot(this->timeouts_.front().timeout);
    }
}

void
linuxpp::ioloop::remove_timeout(const linuxpp::ioloop::periodic_timeout_handle handle)
{
    for (auto timeout = this->periodic_timeouts_.begin();
         timeout != this->periodic_timeouts_.end();
         ++timeout)
    {
        if (timeout->handle == handle)
        {
            if (this->processing_periodic_timeouts_)
            {
                timeout->remove = true;
            }
            else
            {
                this->periodic_timeouts_.erase(timeout);
            }

            return;
        }
    }

    for (auto timeout = this->pending_periodic_timeout_additions_.begin();
         timeout != this->pending_periodic_timeout_additions_.end();
         ++timeout)
    {
        if (timeout->handle == handle)
        {
            this->periodic_timeouts_.erase(timeout);
            return;
        }
    }
}

void
linuxpp::ioloop::process_periodic_timeouts()
{
    this->periodic_timeout_timerfd_.read();

    {
        ndgpp::bool_sentry sentry {this->processing_periodic_timeouts_};
        this->processing_periodic_timeouts_ = true;

        const auto now = std::chrono::steady_clock::now();
        for(auto & timeout : this->periodic_timeouts_)
        {
            if (timeout.remove == true)
            {
                continue;
            }

            if (timeout.timeout <= now)
            {
                // timeout expired, so invoke its call back
                timeout.invoke();
            }
            else
            {
                // the rest of the timeouts have not expired, so we're
                // done
                break;
            }
        }
    }

    // Erase all "removed" timeouts

    this->periodic_timeouts_.erase(std::remove_if(this->periodic_timeouts_.begin(),
                                                  this->periodic_timeouts_.end(),
                                                  [] (const linuxpp::ioloop::periodic_timeout_callback & timeout)
                                                  {
                                                      return timeout.remove == true;
                                                  }),
                                   this->periodic_timeouts_.end());


    // Reset timeout values

    const auto now = std::chrono::steady_clock::now();
    for (auto & timeout : this->periodic_timeouts_)
    {
        if (timeout.processed)
        {
            timeout.reset(now);
        }
        else
        {
            // The remaining timeouts have not been processed, so
            // there's nothing else to do
            break;
        }
    }


    // Re-sort the timeouts to insure that the expired timeouts are
    // placed in their proper position
    std::sort(this->periodic_timeouts_.begin(),
              this->periodic_timeouts_.end());


    // Add the pending periodic timeouts

    for (auto & pending_timeout : this->pending_periodic_timeout_additions_)
    {
        const auto later_timeout =
            std::lower_bound(this->periodic_timeouts_.cbegin(),
                             this->periodic_timeouts_.cend(),
                             pending_timeout);

        this->periodic_timeouts_.insert(later_timeout,
                                        std::move(pending_timeout));
    }

    this->pending_periodic_timeout_additions_.clear();


    // Re-arm the timer

    if (!this->periodic_timeouts_.empty())
    {
        this->periodic_timeout_timerfd_.set_oneshot(this->periodic_timeouts_.front().timeout);
    }
}

void
linuxpp::ioloop::process_callbacks()
{
    this->callbacks_eventfd_.read();

    std::unique_lock<std::mutex> lock(this->mutex_);

    // Move the stored callbacks to a local vector so they can be
    // called without the mutex being held
    std::vector<std::function<void()>> callbacks = std::move(this->new_callbacks_);

    lock.unlock();

    for (auto & callback : callbacks)
    {
        callback();
    }
}

void
linuxpp::ioloop::start()
{
    this->keep_running_ = 1;

    while (this->keep_running_)
    {
        // process the handlers
        const auto ret = this->epoll_.wait(std::nothrow, this->epoll_events_);
        if (! ret)
        {
            if (ret.errno_value() == EINTR)
            {
                // epoll_wait was interrupted, so try again
                continue;
            }
        }

        {
            ndgpp::bool_sentry sentry {this->processing_handlers_};
            this->processing_handlers_ = true;

            for (const auto & event: this->epoll_events_)
            {
                auto handler = static_cast<linuxpp::ioloop::handler_map_type::value_type *> (event.data.ptr);
                handler->second.callback(handler->first, handler->second.events);
            }
        }

        if (this->removed_handlers_.empty())
        {
            continue;
        }

        // prune any handlers marked for removal
        for (auto handler : this->removed_handlers_)
        {
            const auto epoll_ret = this->epoll_.del(std::nothrow, handler);
            if (!epoll_ret)
            {
                if (epoll_ret.errno_value() != ENOENT)
                {
                    // Something weird happened
                    throw ndgpp_error(std::system_error,
                                      std::error_code{epoll_ret.errno_value(), std::system_category()},
                                      "linuxpp::ioloop::epoll_.del(...) failed in linuxpp::ioloop::start");
                }

                // At this point the file descriptor was closed prior
                // to its handler being removed so the epoll_del error
                // is acceptable
            }

            this->handlers_.erase(handler);
        }

        this->removed_handlers_.clear();
    }
}

void
linuxpp::ioloop::stop()
{
    this->keep_running_ = 0;
    this->stop_eventfd_.write();
}
