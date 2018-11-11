#ifndef LIBLINUXPP_SUBPROCESS_PATH_STREAM_HPP
#define LIBLINUXPP_SUBPROCESS_PATH_STREAM_HPP

#include <string>

#include <liblinuxpp/subprocess/stream.hpp>

namespace linuxpp
{
namespace subprocess
{
    /// A process stream that is associated with a path
    class path_stream: public linuxpp::subprocess::stream
    {
        public:

        explicit
        path_stream(const std::string& path);

        virtual ~path_stream();

        const std::string & path() const noexcept;

        private:

        stream_descriptors open() const override;

        std::string path_;
    };
}
}

#endif
