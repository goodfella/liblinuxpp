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
        recv_socket.bind(ndgpp::net::ipv4_address {"127.0.0.1"}, true);
        std::tie(addr, port) = linuxpp::net::getsockname_ipv4(recv_socket.descriptor());
        epoll.add(recv_socket.descriptor(), EPOLLIN);
    }

    linuxpp::epoll epoll;
    linuxpp::net::udp_socket recv_socket;
    linuxpp::net::udp_socket send_socket;
    ndgpp::net::ipv4_address addr;
    ndgpp::net::port port;

    using buffer_type = std::array<unsigned char, 5>;
    buffer_type send_buf = {{1,2,3,4,5}};
};

TEST_F(send_recv_test, test1)
{
    const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
                                                      send_buf.data(),
                                                      send_buf.size(),
                                                      ndgpp::net::ipv4_address {"127.0.0.1"},
                                                      port);
    ASSERT_EQ(send_buf.size(), bytes_sent);
    const auto events = epoll.wait(std::chrono::seconds{5});
    EXPECT_FALSE(events.empty());

    buffer_type recv_buf;
    const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size());

    EXPECT_EQ(bytes_sent, bytes_recv);
    EXPECT_EQ(send_buf, recv_buf);
}
