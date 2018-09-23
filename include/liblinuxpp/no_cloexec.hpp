#ifndef LIBLINUXPP_NO_CLOEXEC_HPP
#define LIBLINUXPP_NO_CLOEXEC_HPP

namespace linuxpp
{
    struct no_cloexec_t {};
    static const no_cloexec_t no_cloexec {};
}

#endif
