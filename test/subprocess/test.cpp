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

TEST(bad_executable, throws)
{
    const auto throws = [] () {
        linuxpp::subprocess::popen bin{{"/dev/zero", "--exit-code", 5},
                                       {}};
    };

    EXPECT_THROW(throws(), ndgpp::error<std::system_error>);
}

TEST(exit_code, non_zero)
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

TEST(exit_code, zero)
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

TEST(signals, signal_receiption)
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

TEST(poll, signaled)
{
    {
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGCHLD);

        // Block SIGCHLD so sigtimedwait will work
        ASSERT_EQ(0, pthread_sigmask(SIG_BLOCK, &sigset, nullptr));

        linuxpp::subprocess::popen bin{{test_path, "--signal"},
                                       {}};

        {
            const auto status = bin.poll();
            ASSERT_FALSE(static_cast<bool>(status));
            ASSERT_FALSE(status.exited());
            ASSERT_FALSE(status.called_exit());
            ASSERT_FALSE(status.signaled());
        }

        struct timespec timeout = {5, 0};
        bin.signal(SIGKILL);
        const int signal = ::sigtimedwait(&sigset, nullptr, &timeout);
        ASSERT_EQ(SIGCHLD, signal);

        const auto status = bin.poll();
        EXPECT_TRUE(static_cast<bool>(status));
        EXPECT_TRUE(status.exited());
        EXPECT_FALSE(status.called_exit());
        EXPECT_TRUE(status.signaled());
        EXPECT_EQ(SIGKILL, status.signal());
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
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    if (settings.wait_for_signal)
    {
        ::pause();
    }

    return settings.exit_code;
}
