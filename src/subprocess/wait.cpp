#include <cerrno>
#include <system_error>

#include <libndgpp/error.hpp>
#include <liblinuxpp/subprocess/wait.hpp>

bool
linuxpp::subprocess::waitid(const idtype_t idtype,
                            const id_t id,
                            siginfo_t& siginfo,
                            const int options)
{
    siginfo.si_pid = 0;
    const int ret = ::waitid(idtype, id, &siginfo, options);
    if (ret == -1)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "waitid failed");
    }

    return siginfo.si_pid != 0;
}
