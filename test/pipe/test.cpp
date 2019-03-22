#include <unistd.h>
#include <fcntl.h>

#include <gtest/gtest.h>

#include <liblinuxpp/fcntl.hpp>
#include <liblinuxpp/pipe.hpp>

TEST(ctor, default)
{
    linuxpp::pipe pipe;
    {
        const int ret = linuxpp::fcntl(pipe.read_fd().get(), F_GETFD);
        EXPECT_EQ(ret, FD_CLOEXEC);
    }

    {
        const int ret = linuxpp::fcntl(pipe.write_fd().get(), F_GETFD);
        EXPECT_EQ(ret, FD_CLOEXEC);
    }
}

TEST(ctor, non_default)
{
    linuxpp::pipe pipe(0);
    {
        const int ret = linuxpp::fcntl(pipe.read_fd().get(), F_GETFD);
        EXPECT_EQ(ret, FD_CLOEXEC);
    }

    {
        const int ret = linuxpp::fcntl(pipe.write_fd().get(), F_GETFD);
        EXPECT_EQ(ret, FD_CLOEXEC);
    }
}

TEST(ctor, nocloexec)
{
    linuxpp::pipe pipe(linuxpp::nocloexec, 0);
    {
        const int ret = linuxpp::fcntl(pipe.read_fd().get(), F_GETFD);
        EXPECT_NE(ret, FD_CLOEXEC);
    }

    {
        const int ret = linuxpp::fcntl(pipe.write_fd().get(), F_GETFD);
        EXPECT_NE(ret, FD_CLOEXEC);
    }
}

TEST(dtor, test)
{
    int read_fd = -1;
    int write_fd = -1;

    {
        linuxpp::pipe pipe;
        read_fd = pipe.read_fd().get();
        write_fd = pipe.write_fd().get();
    }

    EXPECT_ANY_THROW(linuxpp::fcntl(read_fd, F_GETFD));
    EXPECT_ANY_THROW(linuxpp::fcntl(write_fd, F_GETFD));
}
