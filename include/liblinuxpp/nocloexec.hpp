#ifndef LIBLINUXPP_NO_CLOEXEC_HPP
#define LIBLINUXPP_NO_CLOEXEC_HPP

namespace linuxpp
{
    struct nocloexec_t {};
    static const nocloexec_t nocloexec {};
}

#endif
