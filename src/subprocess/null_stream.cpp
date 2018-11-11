#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <liblinuxpp/open.hpp>

#include "stream_descriptors.hpp"
#include <liblinuxpp/subprocess/null_stream.hpp>

linuxpp::subprocess::null_stream::~null_stream() {}

linuxpp::subprocess::stream_descriptors linuxpp::subprocess::null_stream::open() const
{
    return linuxpp::subprocess::stream_descriptors {linuxpp::open("/dev/null",
                                                                  O_RDWR,
                                                                  linuxpp::no_cloexec)};
}
