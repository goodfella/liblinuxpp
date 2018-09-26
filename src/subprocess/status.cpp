#include <sys/wait.h>

#include <stdexcept>

#include <libndgpp/error.hpp>
#include <liblinuxpp/subprocess/status.hpp>

linuxpp::subprocess::status::status(const siginfo_t siginfo)
{
    if (siginfo.si_pid == 0)
    {
        throw ndgpp_error(std::invalid_argument,
                          "siginfo_t::si_pid is equal to 0");
    }

    this->exited_ = true;
    this->exit_code_raw_ = siginfo.si_status;
    this->called_exit_ = siginfo.si_code == CLD_EXITED;
    this->dumped_ = siginfo.si_code == CLD_DUMPED;
    this->signaled_ =
        siginfo.si_code == CLD_KILLED ||
        this->dumped_;
}

linuxpp::subprocess::status::operator bool() const noexcept
{
    return this->exited();
}

bool linuxpp::subprocess::status::exited() const noexcept
{
    return this->exited_;
}

bool linuxpp::subprocess::status::called_exit() const noexcept
{
    return this->called_exit_;
}

bool linuxpp::subprocess::status::signaled() const noexcept
{
    return this->signaled_;
}

bool linuxpp::subprocess::status::dumped() const noexcept
{
    return this->dumped_;
}

int linuxpp::subprocess::status::signal() const
{
    if (!this->signaled())
    {
        throw ndgpp_error(std::logic_error,
                          "subprocess was not signaled");
    }

    return this->exit_code_raw_;
}

int linuxpp::subprocess::status::exit_code() const
{
    if (!this->called_exit())
    {
        throw ndgpp_error(std::logic_error,
                          "subprocess does not have an exit code");
    }

    return this->exit_code_raw_;
}

int linuxpp::subprocess::status::exit_code_raw() const
{
    if (!this->exited_ && !this->signaled())
    {
        throw ndgpp_error(std::logic_error,
                          "subprocess does not have a raw exit code");
    }

    return this->exit_code_raw_;
}
