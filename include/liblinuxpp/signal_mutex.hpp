#ifndef LIBLINUXPP_SIGNAL_MUTEX_HPP
#define LIBLINUXPP_SIGNAL_MUTEX_HPP

#include <signal.h>

namespace linuxpp
{
    class signal_mutex
    {
        public:

        signal_mutex();
        signal_mutex(const sigset_t sigset);

        void lock();
        void unlock();

        private:

        sigset_t sigset_;
        sigset_t prev_sigset_;
    };
}

#endif
