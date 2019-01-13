#include <net/if.h>

#include <stdexcept>

#include <libndgpp/error.hpp>
#include <liblinuxpp/net/interface.hpp>

unsigned int linuxpp::net::if_nametoindex(char const * const name)
{
    const unsigned int index = ::if_nametoindex(name);
    if (index == 0)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed get interface's index");
    }

    return index;
}
