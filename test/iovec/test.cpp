#include <unistd.h>
#include <gtest/gtest.h>
#include <cstring>

#include <array>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <liblinuxpp/iovec.hpp>

class make_iovec_array: public testing::Test
{
    public:
    make_iovec_array():
        vector(5 + array.size(), 'a'),
        pair_vector(5 + vector.size(), 'b'),
        pair(std::make_pair(pair_vector.data(), pair_vector.size()))
    {}

    std::array<int, 5> array;
    std::vector<int> vector;
    std::vector<int> pair_vector;
    std::pair<int *, std::size_t> pair;
};

TEST_F(make_iovec_array, pair)
{
    auto iovec_array = linuxpp::make_iovec_array(pair);
    EXPECT_EQ(1U, iovec_array.size());
    EXPECT_EQ(sizeof(std::remove_pointer_t<decltype(pair)::first_type>) * pair.second, iovec_array[0].iov_len);
}

TEST_F(make_iovec_array, vector)
{
    auto iovec_array = linuxpp::make_iovec_array(vector);
    EXPECT_EQ(1U, iovec_array.size());
    EXPECT_EQ(sizeof(decltype(vector)::value_type) * vector.size(), iovec_array[0].iov_len);
}

TEST_F(make_iovec_array, array)
{
    auto iovec_array = linuxpp::make_iovec_array(array);
    EXPECT_EQ(1U, iovec_array.size());
    EXPECT_EQ(sizeof(decltype(array)::value_type) * array.size(), iovec_array[0].iov_len);
}

TEST_F(make_iovec_array, all_types)
{
    {
        auto iovec_array = linuxpp::make_iovec_array(array, pair, vector);

        EXPECT_EQ(3U, iovec_array.size());

        EXPECT_EQ(array.data(), iovec_array[0].iov_base);
        EXPECT_EQ(sizeof(decltype(array)::value_type) * array.size(), iovec_array[0].iov_len);

        EXPECT_EQ(pair.first, iovec_array[1].iov_base);
        EXPECT_EQ(sizeof(std::remove_pointer_t<decltype(pair)::first_type>) * pair.second, iovec_array[1].iov_len);

        EXPECT_EQ(vector.data(), iovec_array[2].iov_base);
        EXPECT_EQ(sizeof(decltype(vector)::value_type) * vector.size(), iovec_array[2].iov_len);
    }

    {
        auto iovec_array = linuxpp::make_iovec_array(array, vector, pair);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array(vector, array, pair);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array(vector, pair, array);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array(pair, vector, array);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array(pair, array, vector);
        EXPECT_EQ(3U, iovec_array.size());
    }
}

class make_iovec_array_const: public testing::Test
{
    public:
    make_iovec_array_const():
        vector(5 + array.size(), 'a'),
        pair_vector(5 + vector.size(), 'b'),
        pair(std::make_pair(pair_vector.data(), pair_vector.size()))
    {}

    std::array<int, 5> array;
    std::vector<int> vector;
    std::vector<int> pair_vector;
    std::pair<int const *, std::size_t> pair;
};

TEST_F(make_iovec_array_const, pair)
{
    auto iovec_array = linuxpp::make_iovec_array_const(pair);
    EXPECT_EQ(1U, iovec_array.size());
    EXPECT_EQ(sizeof(std::remove_pointer_t<decltype(pair)::first_type>) * pair.second, iovec_array[0].iov_len);
}

TEST_F(make_iovec_array_const, vector)
{
    auto iovec_array = linuxpp::make_iovec_array_const(vector);
    EXPECT_EQ(1U, iovec_array.size());
    EXPECT_EQ(sizeof(decltype(vector)::value_type) * vector.size(), iovec_array[0].iov_len);
}

TEST_F(make_iovec_array_const, array)
{
    auto iovec_array = linuxpp::make_iovec_array_const(array);
    EXPECT_EQ(1U, iovec_array.size());
    EXPECT_EQ(sizeof(decltype(array)::value_type) * array.size(), iovec_array[0].iov_len);
}

TEST_F(make_iovec_array_const, all_types)
{
    {
        auto iovec_array = linuxpp::make_iovec_array_const(array, pair, vector);
        EXPECT_EQ(3U, iovec_array.size());

        EXPECT_EQ(array.data(), iovec_array[0].iov_base);
        EXPECT_EQ(sizeof(decltype(array)::value_type) * array.size(), iovec_array[0].iov_len);

        EXPECT_EQ(pair.first, iovec_array[1].iov_base);
        EXPECT_EQ(sizeof(std::remove_pointer_t<decltype(pair)::first_type>) * pair.second, iovec_array[1].iov_len);

        EXPECT_EQ(vector.data(), iovec_array[2].iov_base);
        EXPECT_EQ(sizeof(decltype(vector)::value_type) * vector.size(), iovec_array[2].iov_len);
    }

    {
        auto iovec_array = linuxpp::make_iovec_array_const(array, vector, pair);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array_const(vector, array, pair);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array_const(vector, pair, array);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array_const(pair, vector, array);
        EXPECT_EQ(3U, iovec_array.size());
    }

    {
        auto iovec_array = linuxpp::make_iovec_array_const(pair, array, vector);
        EXPECT_EQ(3U, iovec_array.size());
    }
}

TEST(make_iovec, vector)
{
    std::vector<int> vec = {1,2,3};
    auto iovec = linuxpp::make_iovec(vec);
    EXPECT_EQ(sizeof(int) * vec.size(), iovec.iov_len);
    EXPECT_EQ(vec.data(), iovec.iov_base);
}

TEST(make_iovec, type_pointer_pair)
{
    int * const p = nullptr;
    std::pair<int *, std::size_t> pair = std::make_pair(p, 5);
    auto iovec = linuxpp::make_iovec(pair);
    EXPECT_EQ(sizeof(int) * pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec, void_pointer_pair)
{
    int * const p = nullptr;
    std::pair<void *, std::size_t> pair = std::make_pair(p, sizeof(int) * 5);
    auto iovec = linuxpp::make_iovec(pair);
    EXPECT_EQ(pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec, type_const_pointer_pair)
{
    int * const p = nullptr;
    std::pair<int * const, std::size_t> pair = std::make_pair(p, 5);
    auto iovec = linuxpp::make_iovec(pair);
    EXPECT_EQ(sizeof(int) * pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec, void_const_pointer_pair)
{
    int * const p = nullptr;
    std::pair<void * const, std::size_t> pair = std::make_pair(p, sizeof(int) * 5);
    auto iovec = linuxpp::make_iovec(pair);
    EXPECT_EQ(pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec, array)
{
    std::array<int, 5> array;
    auto iovec = linuxpp::make_iovec(array);
    EXPECT_EQ(sizeof(int) * array.size(), iovec.iov_len);
    EXPECT_EQ(array.data(), iovec.iov_base);
}

TEST(make_iovec, type_pointer)
{
    int * buf = nullptr;
    auto iovec = linuxpp::make_iovec(buf, 5);
    EXPECT_EQ(sizeof(int) * 5, iovec.iov_len);
    EXPECT_EQ(buf, iovec.iov_base);
}

TEST(make_iovec, void_pointer)
{
    void * buf = nullptr;
    auto iovec = linuxpp::make_iovec(buf, 5);
    EXPECT_EQ(5U, iovec.iov_len);
    EXPECT_EQ(buf, iovec.iov_base);
}

TEST(make_iovec_const, vector)
{
    std::vector<int> vec = {1,2,3};
    auto iovec = linuxpp::make_iovec_const(vec);
    EXPECT_EQ(sizeof(int) * vec.size(), iovec.iov_len);
    EXPECT_EQ(vec.data(), iovec.iov_base);
}

TEST(make_iovec_const, type_pointer_pair)
{
    int * const p = nullptr;
    std::pair<int const *, std::size_t> pair = std::make_pair(p, 5);
    auto iovec = linuxpp::make_iovec_const(pair);
    EXPECT_EQ(sizeof(int) * pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec_const, void_pointer_pair)
{
    int * const p = nullptr;
    std::pair<void const *, std::size_t> pair = std::make_pair(p, sizeof(int) * 5);
    auto iovec = linuxpp::make_iovec_const(pair);
    EXPECT_EQ(pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}


TEST(make_iovec_const, type_const_pointer_pair)
{
    int * const p = nullptr;
    std::pair<int const * const, std::size_t> pair = std::make_pair(p, 5);
    auto iovec = linuxpp::make_iovec_const(pair);
    EXPECT_EQ(sizeof(int) * pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec_const, void_const_pointer_pair)
{
    int * const p = nullptr;
    std::pair<void const * const, std::size_t> pair = std::make_pair(p, sizeof(int) * 5);
    auto iovec = linuxpp::make_iovec_const(pair);
    EXPECT_EQ(pair.second, iovec.iov_len);
    EXPECT_EQ(pair.first, iovec.iov_base);
}

TEST(make_iovec_const, array)
{
    std::array<int, 5> array {};
    auto iovec = linuxpp::make_iovec_const(array);
    EXPECT_EQ(sizeof(int) * array.size(), iovec.iov_len);
    EXPECT_EQ(array.data(), iovec.iov_base);
}

TEST(make_iovec_const, type_pointer)
{
    int const * buf = nullptr;
    auto iovec = linuxpp::make_iovec_const(buf, 5);
    EXPECT_EQ(sizeof(int) * 5, iovec.iov_len);
    EXPECT_EQ(buf, iovec.iov_base);
}

TEST(make_iovec_const, void_pointer)
{
    void const * buf = nullptr;
    auto iovec = linuxpp::make_iovec_const(buf, 5);
    EXPECT_EQ(5U, iovec.iov_len);
    EXPECT_EQ(buf, iovec.iov_base);
}
