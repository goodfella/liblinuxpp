#include "stream_descriptors.hpp"

linuxpp::subprocess::stream_descriptors::stream_descriptors(const int fd):
    read_fd_(fd),
    write_fd_(fd)
{}

linuxpp::subprocess::stream_descriptors::stream_descriptors(const std::array<int, 2>& pipe):
    read_fd_(pipe[0]),
    write_fd_(pipe[1])
{}

linuxpp::subprocess::stream_descriptors::operator bool() const noexcept
{
    return (this->read_fd_ != linuxpp::closed_fd) && (this->write_fd_ != linuxpp::closed_fd);
}

int linuxpp::subprocess::stream_descriptors::read_fd() const noexcept
{
    return this->read_fd_;
}

int linuxpp::subprocess::stream_descriptors::write_fd() const noexcept
{
    return this->write_fd_;
}
