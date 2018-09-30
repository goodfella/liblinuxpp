#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <system_error>
#include <iostream>
#include <thread>

#include <gtest/gtest.h>

#include <libndgpp/error.hpp>
#include <liblinuxpp/subprocess/popen.hpp>

std::string test_path;

bool wait_for(linuxpp::subprocess::popen & bin, const std::chrono::seconds timeout)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);

    struct timespec timespec_timeout = {0, 0};

    const auto start = std::chrono::steady_clock::now();
    auto end = start;
    while (true)
    {
        const auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        if (elapsed_time >= timeout)
        {
            return false;
        }

        timespec_timeout.tv_sec = timeout.count() - elapsed_time.count();
        const int signal = ::sigtimedwait(&sigset, nullptr, &timespec_timeout);
        end = std::chrono::steady_clock::now();

        if (signal == -1)
        {
            return false;
        }

        if (signal != SIGCHLD)
        {
            continue;
        }

        const auto status = bin.poll();
        if (status)
        {
            return true;
        }
    }
}

TEST(ctor, bad_executable)
{
    const auto throws = [] () {
        linuxpp::subprocess::popen bin{{"/dev/zero", "--exit-code", 5},
                                       {}};
    };

    EXPECT_THROW(throws(), ndgpp::error<std::system_error>);
}

TEST(member_tests, wait_exit_code_non_zero)
{
    linuxpp::subprocess::popen bin{{test_path, "--exit-code", 5},
                                   {}};

    const auto status = bin.wait();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_FALSE(status.dumped());
    EXPECT_EQ(5, status.exit_code());
}

TEST(member_tests, wait_exit_code_zero)
{
    linuxpp::subprocess::popen bin{{test_path, "--exit-code", 0},
                                   {}};

    const auto status = bin.wait();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_TRUE(status.called_exit());
    EXPECT_FALSE(status.signaled());
    EXPECT_FALSE(status.dumped());
    EXPECT_EQ(0, status.exit_code());
}

TEST(member_tests, signal)
{
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    // Block SIGUSR1 in the parent to confirm that the child can still
    // receive it.
    EXPECT_EQ(0, pthread_sigmask(SIG_BLOCK, &sigset, nullptr));

    linuxpp::subprocess::popen bin{{test_path, "--signal"},
                                   {}};
    bin.signal(SIGUSR1);
    const auto status = bin.wait();
    EXPECT_TRUE(static_cast<bool>(status));
    EXPECT_TRUE(status.exited());
    EXPECT_FALSE(status.called_exit());
    EXPECT_TRUE(status.signaled());
    EXPECT_EQ(SIGUSR1, status.signal());
}

TEST(member_tests, poll_signaled)
{
    {
        linuxpp::subprocess::popen bin{{test_path, "--signal"},
                                       {}};

        {
            const auto status = bin.poll();
            ASSERT_FALSE(static_cast<bool>(status));
            ASSERT_FALSE(status.exited());
            ASSERT_FALSE(status.called_exit());
            ASSERT_FALSE(status.signaled());
        }

        bin.signal(SIGKILL);
        ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

        const auto status = bin.status();
        EXPECT_TRUE(static_cast<bool>(status));
        EXPECT_TRUE(status.exited());
        EXPECT_FALSE(status.called_exit());
        EXPECT_TRUE(status.signaled());
        EXPECT_EQ(SIGKILL, status.signal());
    }
}

TEST(member_tests, poll_exit_code_zero)
{
    {
        linuxpp::subprocess::popen bin{{test_path, "--exit-code", 0},
                                       {}};

        ASSERT_TRUE(wait_for(bin, std::chrono::seconds{5}));

        const auto status = bin.status();
        EXPECT_TRUE(static_cast<bool>(status));
        EXPECT_TRUE(status.exited());
        EXPECT_TRUE(status.called_exit());
        EXPECT_FALSE(status.signaled());
        EXPECT_EQ(0, status.exit_code());
    }
}

struct test_settings
{
    bool run_tests = true;
    int exit_code = 0;
    int wait_for_signal = false;
};

int main(int argc, char ** argv)
{
    test_path = argv[0];
    test_settings settings;
    for (int i = 1; argv[i] != nullptr; ++i)
    {
        if (strcmp(argv[i], "--exit-code") == 0)
        {
            if (argv[i + 1] != nullptr)
            {
                settings.exit_code = std::atoi(argv[i + 1]);
            }
            else
            {
                return 1;
            }

            settings.run_tests = false;
        }

        if (strcmp(argv[i], "--signal") == 0)
        {
            settings.wait_for_signal = true;
            settings.run_tests = false;
        }
    }

    if (settings.run_tests)
    {
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGCHLD);

        // Block SIGCHLD so sigtimedwait will work
        const int ret = pthread_sigmask(SIG_BLOCK, &sigset, nullptr);
        if (ret == -1)
        {
            std::cerr << "failed to block SIGCHLD\n";
            return 1;
        }

        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    if (settings.wait_for_signal)
    {
        ::pause();
    }

    return settings.exit_code;
}
