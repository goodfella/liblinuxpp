#ifndef LIBLINUXPP_SUBPROCESS_STREAM_HPP
#define LIBLINUXPP_SUBPROCESS_STREAM_HPP

#include <array>
#include <liblinuxpp/file_descriptor.hpp>

namespace linuxpp
{
namespace subprocess
{
    class popen;
    class stream_descriptors;

    /// Base class that represents a standard stream for a process
    class stream
    {
        friend class popen;

        public:

        stream() = default;

        stream(const stream&) = delete;
        stream& operator=(const stream&) = delete;

        stream(stream&&) = delete;
        stream& operator=(stream&&) = delete;

        virtual ~stream();

        private:

        virtual stream_descriptors open() const = 0;
    };
}
}

#endif
