#ifndef LIBLINUXPP_SUBPROCESS_FD_STREAM_HPP
#define LIBLINUXPP_SUBPROCESS_FD_STREAM_HPP

#include <liblinuxpp/subprocess/stream.hpp>
#include <liblinuxpp/file_descriptor.hpp>

namespace linuxpp
{
namespace subprocess
{
    // A process stream that is associated with file descriptors
    class fd_stream final: public linuxpp::subprocess::stream
    {
        public:

        fd_stream(const int fd);
        fd_stream(const int read_fd, const int write_fd);

        private:

        virtual stream_descriptors open() const final override;

        linuxpp::file_descriptor read_fd_;
        linuxpp::file_descriptor write_fd_;
    };
}
}

#endif
