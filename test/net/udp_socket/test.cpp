#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <chrono>
#include <iostream>
#include <tuple>

#include <gtest/gtest.h>

#include <liblinuxpp/epoll.hpp>
#include <liblinuxpp/net/ipv4_address.hpp>
#include <liblinuxpp/net/send.hpp>
#include <liblinuxpp/net/sockaddr.hpp>
#include <liblinuxpp/net/socket.hpp>
#include <liblinuxpp/net/sockopt.hpp>
#include <liblinuxpp/net/udp_socket.hpp>

TEST(ctor, default_ctor)
{
    linuxpp::net::udp_socket socket;
    EXPECT_FALSE(socket);
}

TEST(ctor, domain)
{
    linuxpp::net::udp_socket socket{AF_INET};
    EXPECT_TRUE(socket);

    const auto domain = linuxpp::net::getsockopt<linuxpp::net::so::domain>(socket.descriptor());
    EXPECT_EQ(AF_INET, domain.option_value);
}

TEST(bind, ipv4_addr_ephemeral_port)
{
    linuxpp::net::udp_socket socket{AF_INET};
    socket.bind(linuxpp::net::inaddr_loopback);

    const std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> sockaddr = linuxpp::net::getsockname_ipv4(socket.descriptor());
    EXPECT_EQ(linuxpp::net::inaddr_loopback, std::get<ndgpp::net::ipv4_address>(sockaddr));

    const auto reuse_addr = linuxpp::net::getsockopt<linuxpp::net::so::reuse_addr>(socket.descriptor());
    const auto reuse_port = linuxpp::net::getsockopt<linuxpp::net::so::reuse_port>(socket.descriptor());
    EXPECT_EQ(0, reuse_addr.option_value);
    EXPECT_EQ(0, reuse_port.option_value);
}

TEST(bind, ipv4_addr_ephemeral_port_reuse_addr)
{
    linuxpp::net::udp_socket socket{AF_INET};
    socket.bind(linuxpp::net::inaddr_loopback, true);

    const std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> sockaddr = linuxpp::net::getsockname_ipv4(socket.descriptor());
    EXPECT_EQ(linuxpp::net::inaddr_loopback, std::get<ndgpp::net::ipv4_address>(sockaddr));

    const auto reuse_addr = linuxpp::net::getsockopt<linuxpp::net::so::reuse_addr>(socket.descriptor());
    const auto reuse_port = linuxpp::net::getsockopt<linuxpp::net::so::reuse_port>(socket.descriptor());
    EXPECT_EQ(1, reuse_addr.option_value);
    EXPECT_EQ(0, reuse_port.option_value);
}

struct send_recv_test: public ::testing::Test
{
    protected:

    send_recv_test():
        recv_socket {AF_INET},
        send_socket {AF_INET}
    {
        recv_socket.bind(linuxpp::net::inaddr_loopback, true);
        std::tie(receiver_addr, receiver_port) = linuxpp::net::getsockname_ipv4(recv_socket.descriptor());
        epoll.add(recv_socket.descriptor(), EPOLLIN);

        send_socket.bind(linuxpp::net::inaddr_loopback, true);
        std::tie(sender_addr, sender_port) = linuxpp::net::getsockname_ipv4(send_socket.descriptor());

        iovec = {{{&recv_buf[0], 1},
                  {&recv_buf[1], 1},
                  {&recv_buf[2], 1},
                  {&recv_buf[3], 1},
                  {&recv_buf[4], 1}}};
    }

    linuxpp::epoll epoll;
    linuxpp::net::udp_socket recv_socket;
    linuxpp::net::udp_socket send_socket;
    ndgpp::net::ipv4_address receiver_addr;
    ndgpp::net::port receiver_port;
    ndgpp::net::ipv4_address sender_addr;
    ndgpp::net::port sender_port;

    using buffer_type = std::array<unsigned char, 5>;
    buffer_type send_buf = {{1,2,3,4,5}};
    buffer_type recv_buf;
    std::array<struct ::iovec, 5> iovec;
};

TEST_F(send_recv_test, recv_test1)
{
    // Tests recv that does not return the source port and address

    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      receiver_addr,
                                                      receiver_port);
    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    EXPECT_FALSE(events.empty());

    const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size());

    EXPECT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);
}

TEST_F(send_recv_test, recv_test2)
{
    // Tests recv that does return the source address and port

    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      receiver_addr,
                                                      receiver_port);

    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    ASSERT_FALSE(events.empty());

    ndgpp::net::ipv4_address source_addr;
    ndgpp::net::port source_port;

    const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size(), source_addr, source_port);

    EXPECT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);

    EXPECT_EQ(sender_addr, source_addr);
    EXPECT_EQ(sender_port, source_port);
}

TEST_F(send_recv_test, recv_test3)
{
    // Tests recv that does return the source address and port as a sockaddr_in

    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      receiver_addr,
                                                      receiver_port);

    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    ASSERT_FALSE(events.empty());

    struct ::sockaddr_in sockaddr;

    const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size(), sockaddr);

    ndgpp::net::ipv4_address source_addr;
    ndgpp::net::port source_port;

    std::tie(source_addr, source_port) = linuxpp::net::parse_sockaddr(sockaddr);

    EXPECT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);

    EXPECT_EQ(sender_addr, source_addr);
    EXPECT_EQ(sender_port, source_port);
}

TEST_F(send_recv_test, recv_test4)
{
    // Tests iovec recv that doesn't return the source IP address

    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      receiver_addr,
                                                      receiver_port);

    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    ASSERT_FALSE(events.empty());

    const auto bytes_recv = recv_socket.recv(iovec.data(), iovec.size());

    ASSERT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);
}

TEST_F(send_recv_test, recv_test5)
{
    // Tests iovec recv that does return the source address and port

    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      receiver_addr,
                                                      receiver_port);

    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    ASSERT_FALSE(events.empty());

    ndgpp::net::ipv4_address addr;
    ndgpp::net::port port;
    const auto bytes_recv = recv_socket.recv(iovec.data(), iovec.size(), addr, port);

    ASSERT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);
    EXPECT_EQ(sender_addr, addr);
    EXPECT_EQ(sender_port, port);
}

TEST_F(send_recv_test, recv_test6)
{
    // Tests iovec recv that does return a sockaddr_in

    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      receiver_addr,
                                                      receiver_port);

    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    ASSERT_FALSE(events.empty());

    struct ::sockaddr_in sockaddr;
    const auto bytes_recv = recv_socket.recv(iovec.data(), iovec.size(), sockaddr);

    ASSERT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);

    ndgpp::net::ipv4_address addr;
    ndgpp::net::port port;

    std::tie(addr, port) = linuxpp::net::parse_sockaddr(sockaddr);
    EXPECT_EQ(sender_addr, addr);
    EXPECT_EQ(sender_port, port);
}


struct join_group_test: public ::testing::Test
{
    protected:

    join_group_test():
        socket {AF_INET}
    {}

    linuxpp::net::udp_socket socket;
};

TEST_F(join_group_test, invalid_interface)
{
    const auto throws = [this] () {
        this->socket.join_group(ndgpp::net::multicast_ipv4_address {"239.1.2.3"},
                                "invalid interface");
    };

    using exception_type = ndgpp::error<std::system_error>;
    EXPECT_THROW(throws(), exception_type);
}

TEST_F(join_group_test, valid_interface)
{
    const auto func = [this] () {
        this->socket.join_group(ndgpp::net::multicast_ipv4_address {"239.1.2.3"},
                                "lo");
    };

    EXPECT_NO_THROW(func());
}

TEST_F(join_group_test, interface_address)
{
    const auto func = [this] () {
        this->socket.join_group(ndgpp::net::multicast_ipv4_address {"239.1.2.3"},
                                linuxpp::net::inaddr_loopback);
    };

    EXPECT_NO_THROW(func());
}
