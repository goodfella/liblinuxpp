#ifndef LIBLINUXPP_NET_SOCKOPT_TRAITS_HPP
#define LIBLINUXPP_NET_SOCKOPT_TRAITS_HPP

#include <sys/socket.h>
#include <sys/types.h>

namespace linuxpp {
namespace net {
namespace so {

    /** Base template class for socket option traits
     *
     *  A valid specialization of this class provides the following members
     *
     *  - name: A static constexpr int member variable that represents
     *    the option's name i.e. SO_REUSEADDR
     *
     *  - value_type: A type alias to the option's type
     *
     *  - level: A static function that returns the options level as an int (see
     *    man 2 getsockopt for details)
     *
     *  @tparam T The socket option type
     */
    template <class T>
    struct traits;
}}}

#endif
