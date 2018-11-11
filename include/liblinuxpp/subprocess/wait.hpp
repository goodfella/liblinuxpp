#ifndef LIBLINUXPP_SUBPROCESS_WAIT_HPP
#define LIBLINUXPP_SUBPROCESS_WAIT_HPP

#include <sys/types.h>
#include <sys/wait.h>

namespace linuxpp
{
namespace subprocess
{
    /** Wrapper around the waitid system call
     *
     *  @returns true if a child designated by idtype and id exited,
     *           and false otherwise
     *
     *  @throws ndgpp::error<std::system_error> when an error is encountered
     */
    bool waitid(const idtype_t idtype,
                const id_t id,
                siginfo_t& siginfo,
                const int options);
}
}

#endif
