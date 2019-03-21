#ifndef LIBLINUXPP_NET_SOCKOPT_HPP
#define LIBLINUXPP_NET_SOCKOPT_HPP

#include <sys/socket.h>
#include <sys/types.h>

#include <stdexcept>

#include <libndgpp/error.hpp>

#include <liblinuxpp/net/socket_options.hpp>

namespace linuxpp {
namespace net {

    /** Retrieves the given socket option
     *
     *  @param sd The socket descriptor
     *  @param level The option's level see man 2 getsockopt
     *  @param optname The option's name
     *  @param optval [out] The value of the requested option
     *  @param optlen [in & out] see man 2 getsockopt
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    int getsockopt(const int sd,
                   const int level,
                   const int optname,
                   void * const optval,
                   ::socklen_t * optlen);

    /** Retrieves the given socket option
     *
     *  @param sd The socket descriptor
     *  @param level The option's level see man 2 getsockopt
     *  @param optname The option's name
     *  @param optval The value of the option to set
     *  @param optlen See man 2 getsockopt
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    int setsockopt(const int sd,
                   const int level,
                   const int optname,
                   void const * const optval,
                   const ::socklen_t optlen);

    /** Structure that represents a retrieved socket option
     *
     *  @tparam Option The socket option to retreive
     */
    template <class Option>
    struct sockopt
    {
        using value_type = typename linuxpp::net::so::traits<Option>::value_type;

        /// The option value
        value_type option_value;

        /// The return code from getsockopt
        int return_value;
    };

    /** Retrieves the provided socket option
     *
     *  @tparam Option The socket option type
     *
     *  @param sd The socket descriptor whose option to retreive
     *
     *  @return A sockopt object instantiated based on the Option template arg
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    template <class Option>
    sockopt<Option> getsockopt(const int sd)
    {
        using traits = linuxpp::net::so::traits<Option>;
        typename traits::value_type value;
        ::socklen_t optlen = sizeof(value);
        const int ret = linuxpp::net::getsockopt(sd, traits::level(), traits::name, &value, &optlen);

        if (optlen != sizeof(value))
        {
            throw ndgpp_error(std::invalid_argument,
                              "invalid sockopt length");
        }

        return sockopt<Option> {value, ret};
    }

    /** Sets the provided socket option
     *
     *  @tparam Option The socket option type
     *
     *  @param sd The socket descriptor whose option to retreive
     *
     *  @param value The value to set the socket option to
     *
     *  @return The value returned by the setsockopt system call (see man 2 setsockopt)
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    template <class Option>
    inline int setsockopt(const int sd,
                          const typename linuxpp::net::so::traits<Option>::value_type value)
    {
        using traits = linuxpp::net::so::traits<Option>;
        ::socklen_t optlen = sizeof(value);
        return linuxpp::net::setsockopt(sd, traits::level(), traits::name, &value, optlen);
    }
}}

#endif
