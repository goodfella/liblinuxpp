#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <chrono>
#include <iostream>
#include <tuple>

#include <gtest/gtest.h>

#include <liblinuxpp/epoll.hpp>
#include <liblinuxpp/fcntl.hpp>
#include <liblinuxpp/net/ipv4_address.hpp>
#include <liblinuxpp/net/send.hpp>
#include <liblinuxpp/net/sockaddr.hpp>
#include <liblinuxpp/net/socket.hpp>
#include <liblinuxpp/net/sockopt.hpp>
#include <liblinuxpp/net/tcp_socket.hpp>

TEST(ctor, default_ctor)
{
    linuxpp::net::tcp_socket socket;
    EXPECT_FALSE(socket);
}

TEST(ctor, domain)
{
    linuxpp::net::tcp_socket socket{AF_INET};
    EXPECT_TRUE(socket);

    const auto domain = linuxpp::net::getsockopt<linuxpp::net::so::domain>(socket.descriptor());
    EXPECT_EQ(AF_INET, domain.option_value);
}

TEST(bind_ctor, bind_ipv4_addr_and_ephemeral_port)
{
    linuxpp::net::tcp_socket socket{linuxpp::net::bind_socket, linuxpp::net::inaddr_loopback};
    EXPECT_TRUE(socket);

    const auto domain = linuxpp::net::getsockopt<linuxpp::net::so::domain>(socket.descriptor());
    EXPECT_EQ(AF_INET, domain.option_value);

    const std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> sockaddr =
        linuxpp::net::getsockname_ipv4(socket.descriptor());

    EXPECT_EQ(linuxpp::net::inaddr_loopback, std::get<ndgpp::net::ipv4_address>(sockaddr));
    EXPECT_NE(ndgpp::net::port{0}, std::get<ndgpp::net::port>(sockaddr));
}

TEST(bind, ipv4_addr_ephemeral_port)
{
    linuxpp::net::tcp_socket socket{AF_INET};
    socket.bind(linuxpp::net::inaddr_loopback);

    const std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> sockaddr =
        linuxpp::net::getsockname_ipv4(socket.descriptor());

    EXPECT_EQ(linuxpp::net::inaddr_loopback, std::get<ndgpp::net::ipv4_address>(sockaddr));
    EXPECT_NE(ndgpp::net::port{0}, std::get<ndgpp::net::port>(sockaddr));
}

TEST(listen, test)
{
    linuxpp::net::tcp_socket socket{linuxpp::net::bind_socket, linuxpp::net::inaddr_loopback};

    socket.listen(1);

    const auto listening = linuxpp::net::getsockopt<linuxpp::net::so::accept_conn>(socket.descriptor());
    EXPECT_EQ(1, listening.option_value);
}

struct server_socket
{
    server_socket():
        socket{linuxpp::net::bind_socket, linuxpp::net::inaddr_loopback},
        sockaddr{linuxpp::net::getsockname_ipv4(socket.descriptor())}
    {
        socket.listen(1);
    }

    linuxpp::net::tcp_socket socket;
    std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> sockaddr;
};

struct connect_test: public ::testing::Test
{
    server_socket server;
};

TEST_F(connect_test, ctor)
{
    linuxpp::net::tcp_socket client_socket{linuxpp::net::connect_socket,
                                           ndgpp::net::ipv4_address{0x7f000001},
                                           std::get<ndgpp::net::port>(server.sockaddr)};
}

TEST_F(connect_test, member_function)
{
    linuxpp::net::tcp_socket client_socket{AF_INET};

    client_socket.connect(std::get<ndgpp::net::ipv4_address>(server.sockaddr),
                          std::get<ndgpp::net::port>(server.sockaddr));
}

struct accept_test: public ::testing::Test
{
    accept_test():
        client_socket{AF_INET}
    {
        epoll.add(server.socket.descriptor(), EPOLLIN);
        client_socket.connect(std::get<ndgpp::net::ipv4_address>(server.sockaddr),
                              std::get<ndgpp::net::port>(server.sockaddr));

        client_sockaddr = linuxpp::net::getsockname_ipv4(client_socket.descriptor());
    }

    server_socket server;
    linuxpp::net::tcp_socket client_socket;
    std::tuple<ndgpp::net::ipv4_address, ndgpp::net::port> client_sockaddr;
    linuxpp::epoll epoll;
};

TEST_F(accept_test, with_addr_info)
{
    const auto ret = epoll.wait(std::chrono::seconds(5));
    ASSERT_EQ(1U, ret.size());

    ndgpp::net::ipv4_address client_addr;
    ndgpp::net::port client_port;

    const int client_fd = server.socket.accept(client_addr, client_port);

    ASSERT_NE(-1, client_fd);

    const int fd_flags = linuxpp::fcntl(client_fd, F_GETFD);
    EXPECT_TRUE(fd_flags & FD_CLOEXEC);

    EXPECT_EQ(std::get<ndgpp::net::ipv4_address>(client_sockaddr), client_addr);
    EXPECT_EQ(std::get<ndgpp::net::port>(client_sockaddr), client_port);
}

TEST_F(accept_test, without_addr_info)
{
    const auto ret = epoll.wait(std::chrono::seconds(5));
    ASSERT_EQ(1U, ret.size());

    const int client_fd = server.socket.accept();

    ASSERT_NE(-1, client_fd);

    const int fd_flags = linuxpp::fcntl(client_fd, F_GETFD);
    EXPECT_TRUE(fd_flags & FD_CLOEXEC);
}

TEST_F(accept_test, with_flags)
{
    const auto ret = epoll.wait(std::chrono::seconds(5));
    ASSERT_EQ(1U, ret.size());

    const int client_fd = server.socket.accept(SOCK_NONBLOCK);
    ASSERT_NE(-1, client_fd);

    const int fd_flags = linuxpp::fcntl(client_fd, F_GETFD);
    EXPECT_TRUE(fd_flags & FD_CLOEXEC);

    const int status_flags = linuxpp::fcntl(client_fd, F_GETFL);
    EXPECT_TRUE(status_flags & O_NONBLOCK);
}

struct send_recv_test: public ::testing::Test
{
    protected:

    send_recv_test():
        server_socket {AF_INET},
        client_socket {AF_INET}
    {
        server_socket.bind(linuxpp::net::inaddr_loopback);
        server_socket.listen(1);

        std::tie(server_addr, server_port) = linuxpp::net::getsockname_ipv4(server_socket.descriptor());

        client_socket.connect(server_addr, server_port);
        server_client_sd = server_socket.accept();

        epoll.add(server_socket.descriptor(), EPOLLIN);
        epoll.add(client_socket.descriptor(), EPOLLIN);

        std::tie(client_addr, client_port) = linuxpp::net::getsockname_ipv4(client_socket.descriptor());

        iovec = {{{&recv_buf[0], 1},
                  {&recv_buf[1], 1},
                  {&recv_buf[2], 1},
                  {&recv_buf[3], 1},
                  {&recv_buf[4], 1}}};
    }

    void SetUp() override
    {
        ASSERT_NE(-1, server_client_sd);
        epoll.add(server_client_sd, EPOLLIN);
    }

    linuxpp::epoll epoll;
    linuxpp::net::tcp_socket server_socket;
    linuxpp::net::tcp_socket client_socket;
    int server_client_sd = -1;
    ndgpp::net::ipv4_address server_addr;
    ndgpp::net::port server_port;
    ndgpp::net::ipv4_address client_addr;
    ndgpp::net::port client_port;

    using buffer_type = std::array<unsigned char, 5>;
    buffer_type send_buf = {{1,2,3,4,5}};
    buffer_type recv_buf;
    std::array<struct ::iovec, std::tuple_size<buffer_type>::value> iovec;
};

TEST_F(send_recv_test, recv_test_buffer)
{
    linuxpp::net::send(server_client_sd, send_buf.data(), send_buf.size());
    const auto events = epoll.wait(std::chrono::seconds(5));

    ASSERT_EQ(1U, events.size());
    ASSERT_EQ(client_socket.descriptor(), events[0].data.fd);

    const std::size_t ret = client_socket.recv(recv_buf.data(), recv_buf.size(), MSG_WAITALL);
    ASSERT_EQ(recv_buf.size(), ret);
    EXPECT_EQ(send_buf, recv_buf);
}

TEST_F(send_recv_test, recv_test_iovec)
{
    linuxpp::net::send(server_client_sd, send_buf.data(), send_buf.size());
    const auto events = epoll.wait(std::chrono::seconds(5));

    ASSERT_EQ(1U, events.size());
    ASSERT_EQ(client_socket.descriptor(), events[0].data.fd);

    const std::size_t ret = client_socket.recv(iovec.data(), iovec.size(), MSG_WAITALL);
    ASSERT_EQ(recv_buf.size(), ret);
    EXPECT_EQ(send_buf, recv_buf);
}

// TEST_F(send_recv_test, recv_test1)
// {
//     // Tests recv that does not return the source port and address

//     const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
//                                                       send_buf.data(),
//                                                       send_buf.size(),
//                                                       receiver_addr,
//                                                       receiver_port);
//     ASSERT_EQ(send_buf.size(), bytes_sent);
//     const auto events = epoll.wait(std::chrono::seconds{5});
//     EXPECT_FALSE(events.empty());

//     const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size());

//     EXPECT_EQ(bytes_sent, bytes_recv);
//     EXPECT_EQ(send_buf, recv_buf);
// }

// TEST_F(send_recv_test, recv_test2)
// {
//     // Tests recv that does return the source address and port

//     const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
//                                                       send_buf.data(),
//                                                       send_buf.size(),
//                                                       receiver_addr,
//                                                       receiver_port);

//     ASSERT_EQ(send_buf.size(), bytes_sent);
//     const auto events = epoll.wait(std::chrono::seconds{5});
//     ASSERT_FALSE(events.empty());

//     ndgpp::net::ipv4_address source_addr;
//     ndgpp::net::port source_port;

//     const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size(), source_addr, source_port);

//     EXPECT_EQ(bytes_sent, bytes_recv);
//     EXPECT_EQ(send_buf, recv_buf);

//     EXPECT_EQ(sender_addr, source_addr);
//     EXPECT_EQ(sender_port, source_port);
// }

// TEST_F(send_recv_test, recv_test3)
// {
//     // Tests recv that does return the source address and port as a sockaddr_in

//     const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
//                                                       send_buf.data(),
//                                                       send_buf.size(),
//                                                       receiver_addr,
//                                                       receiver_port);

//     ASSERT_EQ(send_buf.size(), bytes_sent);
//     const auto events = epoll.wait(std::chrono::seconds{5});
//     ASSERT_FALSE(events.empty());

//     struct ::sockaddr_in sockaddr;

//     const std::size_t bytes_recv = recv_socket.recv(recv_buf.data(), recv_buf.size(), sockaddr);

//     ndgpp::net::ipv4_address source_addr;
//     ndgpp::net::port source_port;

//     std::tie(source_addr, source_port) = linuxpp::net::parse_sockaddr(sockaddr);

//     EXPECT_EQ(bytes_sent, bytes_recv);
//     EXPECT_EQ(send_buf, recv_buf);

//     EXPECT_EQ(sender_addr, source_addr);
//     EXPECT_EQ(sender_port, source_port);
// }

// TEST_F(send_recv_test, recv_test4)
// {
//     // Tests iovec recv that doesn't return the source IP address

//     const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
//                                                       send_buf.data(),
//                                                       send_buf.size(),
//                                                       receiver_addr,
//                                                       receiver_port);

//     ASSERT_EQ(send_buf.size(), bytes_sent);
//     const auto events = epoll.wait(std::chrono::seconds{5});
//     ASSERT_FALSE(events.empty());

//     const auto bytes_recv = recv_socket.recv(iovec.data(), iovec.size());

//     ASSERT_EQ(bytes_sent, bytes_recv);
//     EXPECT_EQ(send_buf, recv_buf);
// }

// TEST_F(send_recv_test, recv_test5)
// {
//     // Tests iovec recv that does return the source address and port

//     const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
//                                                       send_buf.data(),
//                                                       send_buf.size(),
//                                                       receiver_addr,
//                                                       receiver_port);

//     ASSERT_EQ(send_buf.size(), bytes_sent);
//     const auto events = epoll.wait(std::chrono::seconds{5});
//     ASSERT_FALSE(events.empty());

//     ndgpp::net::ipv4_address addr;
//     ndgpp::net::port port;
//     const auto bytes_recv = recv_socket.recv(iovec.data(), iovec.size(), addr, port);

//     ASSERT_EQ(bytes_sent, bytes_recv);
//     EXPECT_EQ(send_buf, recv_buf);
//     EXPECT_EQ(sender_addr, addr);
//     EXPECT_EQ(sender_port, port);
// }

// TEST_F(send_recv_test, recv_test6)
// {
//     // Tests iovec recv that does return a sockaddr_in

//     const std::size_t bytes_sent = linuxpp::net::send(send_socket.descriptor(),
//                                                       send_buf.data(),
//                                                       send_buf.size(),
//                                                       receiver_addr,
//                                                       receiver_port);

//     ASSERT_EQ(send_buf.size(), bytes_sent);
//     const auto events = epoll.wait(std::chrono::seconds{5});
//     ASSERT_FALSE(events.empty());

//     struct ::sockaddr_in sockaddr;
//     const auto bytes_recv = recv_socket.recv(iovec.data(), iovec.size(), sockaddr);

//     ASSERT_EQ(bytes_sent, bytes_recv);
//     EXPECT_EQ(send_buf, recv_buf);

//     ndgpp::net::ipv4_address addr;
//     ndgpp::net::port port;

//     std::tie(addr, port) = linuxpp::net::parse_sockaddr(sockaddr);
//     EXPECT_EQ(sender_addr, addr);
//     EXPECT_EQ(sender_port, port);
// }
