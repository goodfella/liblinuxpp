#include <liblinuxpp/pipe.hpp>

#include "stream_descriptors.hpp"
#include <liblinuxpp/subprocess/pipe_stream.hpp>

linuxpp::subprocess::pipe_stream::~pipe_stream() {}

linuxpp::subprocess::stream_descriptors linuxpp::subprocess::pipe_stream::open() const
{
    return linuxpp::subprocess::stream_descriptors {linuxpp::pipe_fd(0)};
}
