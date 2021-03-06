#ifndef LIBLINUXPP_FILE_DESCRIPTOR_HPP
#define LIBLINUXPP_FILE_DESCRIPTOR_HPP

#include <liblinuxpp/int_descriptor.hpp>

namespace linuxpp
{
    constexpr int closed_fd = -1;
    namespace detail
    {
        struct file_descriptor_tag;
    }

    using file_descriptor = int_descriptor<detail::file_descriptor_tag>;

    static_assert(std::is_same<int_descriptor_default_traits, file_descriptor::type_traits>::value,
                  "bad file_descriptor::type_traits");
}

#endif
