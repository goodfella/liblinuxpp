#include <gtest/gtest.h>

#include <cstdint>

#include <chrono>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <utility>

#include <liblinuxpp/eventfd.hpp>
#include <liblinuxpp/ioloop.hpp>

#include <liblinuxpp/read.hpp>

class test_ioloop: public testing::Test
{
    public:

    ~test_ioloop()
    {
        this->stop_ioloop_thread();
    }

    void start_ioloop()
    {
        this->ioloop_start_promise.set_value();
        this->ioloop.start();
        this->ioloop_stop_promise.set_value();
    }

    void start_ioloop_thread()
    {
        auto future = this->ioloop_start_promise.get_future();
        ioloop_thread = std::thread(std::bind(&test_ioloop::start_ioloop, this));
        ioloop_thread.detach();

        const auto ret = future.wait_for(std::chrono::seconds{10});
        if (ret != std::future_status::ready)
        {
            throw std::runtime_error("ioloop was never ready");
        }

        this->ioloop_thread_started = true;
    }

    void stop_ioloop()
    {
        this->ioloop.stop();
    }

    void stop_ioloop_thread()
    {
        if (this->ioloop_thread_started == false)
        {
            return;
        }

        auto future = this->ioloop_stop_promise.get_future();
        this->ioloop.add_callback(std::bind(&test_ioloop::stop_ioloop, this));

        const auto ret = future.wait_for(std::chrono::seconds{10});
        if (ret != std::future_status::ready)
        {
            throw std::runtime_error("ioloop never stopped");
        }

        this->ioloop_thread_started = false;
    }

    linuxpp::ioloop ioloop;
    std::promise<void> ioloop_start_promise;
    std::promise<void> ioloop_stop_promise;
    bool ioloop_thread_started = false;
    std::thread ioloop_thread;
};

TEST_F(test_ioloop, add_handler)
{
    linuxpp::eventfd eventfd;

    std::promise<std::pair<int, uint32_t>> handler_called_promise;
    auto handler = [&handler_called_promise] (int fd, uint32_t events) {
        handler_called_promise.set_value(std::make_pair(fd, events));
        uint64_t value;
        linuxpp::read(fd, &value, sizeof(value));
    };

    this->ioloop.add_handler(eventfd.fd(),
                             linuxpp::ioloop::event_enum::read,
                             handler);

    this->start_ioloop_thread();
    eventfd.write();

    auto future = handler_called_promise.get_future();
    const auto ret = future.wait_for(std::chrono::seconds{10});
    ASSERT_EQ(ret, std::future_status::ready);
    const auto value = future.get();
    EXPECT_EQ(value.first, eventfd.fd());
    EXPECT_EQ(value.second, linuxpp::ioloop::event_enum::read);
}

TEST_F(test_ioloop, add_duplicate_handler)
{
    linuxpp::eventfd eventfd;
    auto handler = [] (int fd, uint32_t events) {};
    this->ioloop.add_handler(eventfd.fd(),
                             linuxpp::ioloop::event_enum::read,
                             handler);
    EXPECT_THROW(this->ioloop.add_handler(eventfd.fd(),
                                          linuxpp::ioloop::event_enum::read,
                                          handler),
                 ndgpp::error<std::runtime_error>);
}

TEST_F(test_ioloop, remove_handler)
{
    linuxpp::eventfd eventfd1;
    linuxpp::eventfd eventfd2;

    std::promise<void> handler_called_promise;
    auto handler = [&handler_called_promise] (int fd, uint32_t events) {
        uint64_t value;
        linuxpp::read(fd, &value, sizeof(value));
        handler_called_promise.set_value();
    };

    this->ioloop.add_handler(eventfd1.fd(),
                             linuxpp::ioloop::event_enum::read,
                             handler);

    this->ioloop.remove_handler(eventfd1.fd());

    this->ioloop.add_handler(eventfd2.fd(),
                             linuxpp::ioloop::event_enum::read,
                             handler);

    this->start_ioloop_thread();
    eventfd1.write();
    eventfd2.write();

    auto future = handler_called_promise.get_future();
    const auto ret = future.wait_for(std::chrono::seconds{10});
    ASSERT_EQ(ret, std::future_status::ready);
}

TEST_F(test_ioloop, remove_handler_in_handler_callback)
{
    linuxpp::eventfd eventfd;

    std::promise<void> handler_called_promise;
    auto handler = [&handler_called_promise, this] (int fd, uint32_t events) {
        uint64_t value;
        linuxpp::read(fd, &value, sizeof(value));
        this->ioloop.remove_handler(fd);
        handler_called_promise.set_value();
        this->ioloop.stop();
    };

    this->ioloop.add_handler(eventfd.fd(),
                             linuxpp::ioloop::event_enum::read,
                             handler);

    this->start_ioloop_thread();
    eventfd.write();

    auto future = handler_called_promise.get_future();
    const auto ret = future.wait_for(std::chrono::seconds{10});
    ASSERT_EQ(ret, std::future_status::ready);

    this->stop_ioloop_thread();
}

TEST_F(test_ioloop, add_callback)
{
    std::promise<void> promise;
    auto callback = [&promise] () {promise.set_value();};

    this->start_ioloop_thread();

    this->ioloop.add_callback(callback);
    auto future = promise.get_future();
    const auto ret = future.wait_for(std::chrono::seconds{10});
    EXPECT_EQ(ret, std::future_status::ready);
}

TEST_F(test_ioloop, add_time_point_timeout)
{
    unsigned int counter = 0;
    std::promise<decltype(counter)> promise;

    auto timeout_callback1 = [this, &counter, &promise] () {
        promise.set_value(counter++);
    };

    const auto now = std::chrono::steady_clock::now();
    this->ioloop.add_timeout(now, timeout_callback1);

    this->start_ioloop_thread();

    {
        auto future = promise.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 0U);
    }
}

TEST_F(test_ioloop, add_time_point_timeout_in_order)
{
    unsigned int counter = 0;
    std::promise<decltype(counter)> promise1;
    std::promise<decltype(counter)> promise2;

    auto timeout_callback1 = [this, &counter, &promise1] () {
        promise1.set_value(counter++);
    };

    auto timeout_callback2 = [this, &counter, &promise2] () {
        promise2.set_value(counter++);
    };

    const auto now = std::chrono::steady_clock::now();
    this->ioloop.add_timeout(now, timeout_callback1);
    this->ioloop.add_timeout(now + std::chrono::nanoseconds {1}, timeout_callback2);

    this->start_ioloop_thread();

    {
        auto future = promise1.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 0U);
    }

    {
        auto future = promise2.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 1U);
    }
}

TEST_F(test_ioloop, add_time_point_timeout_out_of_order)
{
    unsigned int counter = 0;
    std::promise<decltype(counter)> promise1;
    std::promise<decltype(counter)> promise2;

    auto timeout_callback1 = [this, &counter, &promise1] () {
        promise1.set_value(counter++);
    };

    auto timeout_callback2 = [this, &counter, &promise2] () {
        promise2.set_value(counter++);
    };

    const auto now = std::chrono::steady_clock::now();
    this->ioloop.add_timeout(now + std::chrono::nanoseconds{2}, timeout_callback2);
    this->ioloop.add_timeout(now + std::chrono::nanoseconds{1}, timeout_callback1);

    this->start_ioloop_thread();

    {
        auto future = promise1.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 0U);
    }

    {
        auto future = promise2.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 1U);
    }
}

TEST_F(test_ioloop, add_minute_duration_timeout_in_order)
{
    unsigned int counter = 0;
    std::promise<decltype(counter)> promise1;
    std::promise<decltype(counter)> promise2;

    auto timeout_callback1 = [this, &counter, &promise1] () {
        promise1.set_value(counter++);
    };

    auto timeout_callback2 = [this, &counter, &promise2] () {
        promise2.set_value(counter++);
    };

    using duration_type = std::chrono::nanoseconds;

    this->ioloop.add_timeout(duration_type {0}, timeout_callback1);
    this->ioloop.add_timeout(duration_type {1}, timeout_callback2);

    this->start_ioloop_thread();

    {
        auto future = promise1.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 0U);
    }

    {
        auto future = promise2.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        ASSERT_EQ(ret, std::future_status::ready);
        EXPECT_EQ(future.get(), 1U);
    }
}

TEST_F(test_ioloop, remove_timeout)
{
    std::promise<void> promise;
    auto timeout_callback = [&promise] () {
        promise.set_value();
    };

    const auto now = std::chrono::steady_clock::now();
    const auto timeout_handle = this->ioloop.add_timeout(now, timeout_callback);
    this->ioloop.add_timeout(now + std::chrono::nanoseconds {1}, timeout_callback);
    this->ioloop.remove_timeout(timeout_handle);

    this->start_ioloop_thread();

    {
        auto future = promise.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        EXPECT_EQ(ret, std::future_status::ready);
    }
}

TEST_F(test_ioloop, remove_timeout_from_timeout_callback)
{
    const auto now = std::chrono::steady_clock::now();

    std::promise<void> promise;

    auto removed_callback = [&promise] () {
        // This will throw an exception if the removal fails
        promise.set_value();
    };

    const auto timeout_handle =
        this->ioloop.add_timeout(now + std::chrono::nanoseconds {1},
                                 removed_callback);

    auto timeout_callback = [this, timeout_handle, &promise] () {
        promise.set_value();
        this->ioloop.remove_timeout(timeout_handle);
    };

    this->ioloop.add_timeout(now, timeout_callback);

    this->start_ioloop_thread();

    {
        auto future = promise.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        EXPECT_EQ(ret, std::future_status::ready);
    }
}

TEST_F(test_ioloop, add_time_point_timeout_from_timeout_callback)
{
    const auto now = std::chrono::steady_clock::now();

    std::promise<void> promise;

    auto timeout_callback = [&promise] () {
        promise.set_value();
    };

    auto legal_callback = [this, timeout_callback] () {
        this->ioloop.add_timeout(std::chrono::steady_clock::now(), timeout_callback);
    };

    this->ioloop.add_timeout(now, legal_callback);
    this->start_ioloop_thread();

    {
        auto future = promise.get_future();
        const auto ret = future.wait_for(std::chrono::seconds{10});
        EXPECT_EQ(ret, std::future_status::ready);
    }
}
