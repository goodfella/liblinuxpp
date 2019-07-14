# Introduction

This repository contains a C++14 library that acts as a C++ facade
around Linux system calls and concepts.  This library will make
programming for a Linux system much easier.

The library is composed of helper functions and objects including but
not limited to:

- [linuxpp::open](include/liblinuxpp/open.hpp)
- [linuxpp::unique_fd](include/liblinuxpp/unique_fd.hpp)
- [linuxpp::pipe](include/liblinuxpp/pipe.hpp)
- [linuxpp::subprocess::popen](include/liblinuxpp/subprocess/popen.hpp)
- [linuxpp::net::udp_socket](include/liblinuxpp/net/udp_socket.hpp)
- [linuxpp::net::tcp_socket](include/liblinuxpp/net/tcp_socket.hpp)
- [linuxpp::net::tcp_datagram_socket](include/liblinuxpp/net/tcp_datagram_socket.hpp)

#### linuxpp::open

Function overload set for the _open_ system call.

#### linuxpp::unique_fd

A resource owning class for int based descriptors such as file
descriptors and socket descriptors.  This class eliminates the need to
call close on file descriptors because its destructor will do it for
you.  It is heavily based on _std::unique_ptr_.

#### linuxpp::pipe

A resource owning class for pipes created with the pipe system call.
This class improves the usability of Linux pipes because it manages
the associated file descriptors, and provides member functions for
reading and writing POD types to the pipe.

#### linuxpp::subprocess::popen

A resource owning class that manages a subprocess.  The API of this
class was heavily inspired by Python's
[subprocess.Popen](https://docs.python.org/2/library/subprocess.html#subprocess.Popen)
class.  This class improves the usability of subprocesses in a given
program because it consolidates subprocess management functions into a
single object.

#### linuxpp::net::udp_socket

A resource owning class for UDP sockets.  This class provides a
receive member function, several send member functions, and other
socket related functions such as bind.

#### linuxpp::net::tcp_socket

A resource owning class for TCP sockets.  This class provides member
functions that facilitate implementing both TCP clients and servers.

#### linuxpp::net::tcp_datagram_socket

A wrapper around a TCP socket that faciliates sending and receiving of
"datagrams" instead of a stream of bytes.
