#ifndef LIBLINUXPP_SUBPROCESS_PIPE_STREAM_HPP
#define LIBLINUXPP_SUBPROCESS_PIPE_STREAM_HPP

#include <liblinuxpp/subprocess/stream.hpp>

namespace linuxpp
{
namespace subprocess
{
    /// Represents a stream created by the pipe system call
    class pipe_stream: public linuxpp::subprocess::stream
    {
        public:

        pipe_stream() = default;

        virtual ~pipe_stream();

        private:

        stream_descriptors open() const override;
    };
}
}

#endif
