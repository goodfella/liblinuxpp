#include <liblinuxpp/net/bind.hpp>
#include <liblinuxpp/net/connect.hpp>
#include <liblinuxpp/net/tcp_socket.hpp>


linuxpp::net::tcp_socket::tcp_socket() noexcept = default;

linuxpp::net::tcp_socket::tcp_socket(tcp_socket &&) noexcept = default;
linuxpp::net::tcp_socket & linuxpp::net::tcp_socket::operator= (linuxpp::net::tcp_socket &&) noexcept = default;


linuxpp::net::tcp_socket::tcp_socket(const int domain):
    members_{linuxpp::unique_fd<>{linuxpp::net::socket(domain, SOCK_STREAM, 0)}}
{}

linuxpp::net::tcp_socket::tcp_socket(linuxpp::net::bind_socket_t do_bind,
                                     const ndgpp::net::ipv4_address address,
                                     const ndgpp::net::port port):
   linuxpp::net::tcp_socket(AF_INET)
{
    linuxpp::net::bind(this->descriptor(), address, port);
}

linuxpp::net::tcp_socket::tcp_socket(linuxpp::net::connect_socket_t do_connect,
                                     const ndgpp::net::ipv4_address address,
                                     const ndgpp::net::port port):
   linuxpp::net::tcp_socket(AF_INET)
{
    linuxpp::net::connect(this->descriptor(), address, port);
}

int linuxpp::net::tcp_socket::descriptor() const noexcept
{
    return std::get<sock_descriptor>(this->members_).get();
}

void linuxpp::net::tcp_socket::close() noexcept
{
    std::get<sock_descriptor>(this->members_).reset();
}

void linuxpp::net::tcp_socket::bind(const ndgpp::net::ipv4_address address,
                                    const ndgpp::net::port port)
{
    linuxpp::net::bind(this->descriptor(), address, port);
}

void linuxpp::net::tcp_socket::connect(const ndgpp::net::ipv4_address address,
                                       const ndgpp::net::port port)
{
    linuxpp::net::connect(this->descriptor(), address, port);
}
