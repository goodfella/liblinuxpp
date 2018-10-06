#ifndef LIBLINUXPP_SUBPROCESS_NULL_STREAM_HPP
#define LIBLINUXPP_SUBPROCESS_NULL_STREAM_HPP

#include "path_stream.hpp"

namespace linuxpp
{
namespace subprocess
{
    /// A process stream that is associated with /dev/null
    class null_stream final: public linuxpp::subprocess::path_stream
    {
        public:

        null_stream() = default;

        virtual ~null_stream();

        private:

        stream_descriptors open() const final override;
    };
}
}

#endif
