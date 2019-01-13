#ifndef LIBLINUXPP_NET_INTERFACE_HPP
#define LIBLINUXPP_NET_INTERFACE_HPP

namespace linuxpp { namespace net
{
    /** Returns the network interfaces index given its name
     *
     *  @param name The name of the interface whose index to get
     *
     *  @return The index of the interface
     *
     *  @throws ndgpp::error<std::system_error> if the index cannot be retrieved
     */
    unsigned int if_nametoindex(char const * const name);
}}

#endif
