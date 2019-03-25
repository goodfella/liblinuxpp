#include <cerrno>

#include <stdexcept>

#include <unistd.h>
#include <gtest/gtest.h>

#include <libndgpp/error.hpp>

#include <liblinuxpp/syscall_return.hpp>

TEST(ctor, return_overload)
{
    linuxpp::syscall_return<int> ret{1};

    EXPECT_TRUE(ret);
    EXPECT_TRUE(ret.good());
    EXPECT_FALSE(ret.fail());

    EXPECT_EQ(1, ret.return_value());
    EXPECT_EQ(0, ret.errno_value());
}

TEST(ctor, errno_overload)
{
    linuxpp::syscall_return<int> ret{linuxpp::seterrno, EINVAL};

    EXPECT_FALSE(ret);
    EXPECT_FALSE(ret.good());
    EXPECT_TRUE(ret.fail());

    EXPECT_EQ(EINVAL, ret.errno_value());
}

TEST(ctor, bad_errno)
{
    auto throws = [] () {linuxpp::syscall_return<int> {linuxpp::seterrno, 0};};
    EXPECT_THROW(throws(), ndgpp::error<std::logic_error>);
}
