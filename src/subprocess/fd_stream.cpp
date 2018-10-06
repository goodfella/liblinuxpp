#include <array>

#include <liblinuxpp/subprocess/fd_stream.hpp>
#include "stream_descriptors.hpp"

linuxpp::subprocess::fd_stream::fd_stream(const int fd):
    read_fd_(fd),
    write_fd_(fd)
{}

linuxpp::subprocess::fd_stream::fd_stream(const int read_fd, const int write_fd):
    read_fd_(read_fd),
    write_fd_(write_fd)
{}

linuxpp::subprocess::stream_descriptors
linuxpp::subprocess::fd_stream::open() const
{
    return linuxpp::subprocess::stream_descriptors (std::array<int, 2> {this->read_fd_.get(), this->write_fd_.get()});
}
