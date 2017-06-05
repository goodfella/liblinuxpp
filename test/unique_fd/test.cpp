#include <unistd.h>
#include <gtest/gtest.h>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <liblinuxpp/unique_fd.hpp>

struct test_closer
{
    test_closer():
        check(-1)
        {}

    test_closer(int c):
        check(c)
        {}

    void operator () (const int val) noexcept
    {
        check = val;
    }

    int check;
};

test_closer closer{1};

class member_test: public testing::Test
{
    protected:

    void SetUp()
    {
        const auto ret = ::pipe(pipe);
        if (ret != 0) {
            throw std::runtime_error(std::strerror(errno));
        }

        fd1 = linuxpp::unique_fd<>(pipe[0]);
        fd2 = linuxpp::unique_fd<>(pipe[1]);
    }

    void TearDown() {}

    int pipe[2];
    linuxpp::unique_fd<> fd1;
    linuxpp::unique_fd<> fd2;
};

template <class C>
void check_ctor(linuxpp::unique_fd<C>& fd, int fd_val)
{
    EXPECT_EQ(fd.get(), fd_val);
    EXPECT_EQ(static_cast<bool>(fd), fd_val != linuxpp::closed_fd);
}

template <class C>
void check_closer_ctor(linuxpp::unique_fd<C>& fd, int fd_val)
{
    const linuxpp::unique_fd<C> & const_fd = fd;
    auto & fd_closer = fd.get_closer();
    auto & const_fd_closer = const_fd.get_closer();

    check_ctor(fd, fd_val);
    EXPECT_EQ(fd.get(), linuxpp::closed_fd);
    EXPECT_EQ(fd_closer.check, closer.check);
    EXPECT_EQ(const_fd_closer.check, closer.check);
}

template <class C>
void check_move(linuxpp::unique_fd<C>& dst, const linuxpp::unique_fd<C>& src, int fd_val)
{
    EXPECT_EQ(dst.get(), fd_val);
    EXPECT_EQ(src.get(), linuxpp::closed_fd);
    dst.release();
}

TEST(type, size)
{
    EXPECT_EQ(sizeof(linuxpp::unique_fd<>), sizeof(int));
}

TEST(ctor, default)
{
    linuxpp::unique_fd<> fd;
    check_ctor(fd, linuxpp::closed_fd);
}

TEST(ctor, non_default)
{
    linuxpp::unique_fd<> fd(5);
    check_ctor(fd, 5);
    const int val = fd.release();
    EXPECT_EQ(fd.get(), linuxpp::closed_fd);
    EXPECT_EQ(val, 5);
}

TEST(ctor, non_reference_closer)
{
    linuxpp::unique_fd<test_closer> fd(linuxpp::closed_fd, test_closer(closer));
    EXPECT_NE(&closer, &fd.get_closer());
    check_closer_ctor(fd, linuxpp::closed_fd);
}

TEST(ctor, reference_closer)
{
    linuxpp::unique_fd<test_closer&> fd(linuxpp::closed_fd, closer);
    EXPECT_EQ(&closer, &fd.get_closer());
    check_closer_ctor(fd, linuxpp::closed_fd);
}

TEST(move, ctor)
{
    linuxpp::unique_fd<> src(5);
    linuxpp::unique_fd<> dst(std::move(src));
    check_move(dst, src, 5);
}

TEST(move, assignment)
{
    linuxpp::unique_fd<> src(5);
    linuxpp::unique_fd<> dst;
    dst = std::move(src);
    check_move(dst, src, 5);
}

TEST_F(member_test, reset)
{
    const int fd2_fd = fd2.get();
    fd1.reset(fd2.release());
    EXPECT_EQ(fd1.get(), fd2_fd);

    linuxpp::unique_fd<> fd;
    fd.reset(fd1.release());
    EXPECT_EQ(fd.get(), fd2_fd);
    fd.release();
}

TEST(dtor, test)
{
    constexpr int fdInt = 5;
    test_closer closer;
    ASSERT_NE(closer.check, fdInt);

    {
        linuxpp::unique_fd<test_closer&> fd{fdInt, closer};
    }

    EXPECT_EQ(closer.check, fdInt);
}
