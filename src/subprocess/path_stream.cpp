#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>

#include <libndgpp/source_location.hpp>
#include <libndgpp/error.hpp>

#include "stream_descriptors.hpp"
#include <liblinuxpp/subprocess/path_stream.hpp>
#include <liblinuxpp/file_descriptor.hpp>
#include <liblinuxpp/open.hpp>
#include <liblinuxpp/no_cloexec.hpp>

linuxpp::subprocess::path_stream::path_stream(const std::string& path):
    path_(path)
{}

linuxpp::subprocess::path_stream::~path_stream() {}

const std::string& linuxpp::subprocess::path_stream::path() const noexcept
{
    return this->path_;
}

linuxpp::subprocess::stream_descriptors linuxpp::subprocess::path_stream::open() const
{
    return linuxpp::subprocess::stream_descriptors {linuxpp::open(linuxpp::no_cloexec,
                                                                  path_.c_str(),
                                                                  O_RDWR)};
}
