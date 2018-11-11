#ifndef LIBLINUXPP_SUBPROCESS_STREAM_DESCRIPTORS_HPP
#define LIBLINUXPP_SUBPROCESS_STREAM_DESCRIPTORS_HPP

#include <array>
#include <liblinuxpp/file_descriptor.hpp>

namespace linuxpp
{
namespace subprocess
{
    /// Class that contains a stream's read and write descriptors
    class stream_descriptors
    {
        public:

        stream_descriptors() = default;
        stream_descriptors(const int fd);
        stream_descriptors(const std::array<int, 2>& pipe);

        explicit
        operator bool() const noexcept;

        /// File descriptor that represents the read end of the stream
        int read_fd() const noexcept;

        /// File descriptor that represents the write end of the stream
        int write_fd() const noexcept;

        private:

        int read_fd_ = linuxpp::closed_fd;
        int write_fd_ = linuxpp::closed_fd;
    };
}
}

#endif
