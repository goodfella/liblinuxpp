#include <system_error>

#include <libndgpp/error.hpp>
#include <liblinuxpp/signal_mutex.hpp>

linuxpp::signal_mutex::signal_mutex()
{
    sigfillset(&this->sigset_);
}

linuxpp::signal_mutex::signal_mutex(const sigset_t sigset):
    sigset_(sigset)
{}

void linuxpp::signal_mutex::lock()
{
    const int ret = pthread_sigmask(SIG_BLOCK, &this->sigset_, &this->prev_sigset_);
    if (ret != 0)
    {
        throw ndgpp_error(std::system_error,
                          std::error_code (errno, std::system_category()),
                          "failed to block signals");
    }
}

void linuxpp::signal_mutex::unlock()
{
    pthread_sigmask(SIG_SETMASK, &this->prev_sigset_, nullptr);
}
