#ifndef LIBLINUXPP_SUBPROCESS_STATUS_HPP
#define LIBLINUXPP_SUBPROCESS_STATUS_HPP

#include <sys/types.h>
#include <sys/wait.h>

namespace linuxpp
{
namespace subprocess
{
    /// Represents the status of a subprocess
    class status
    {
        public:

        constexpr status() = default;

        /** Constructs a status object
         *
         *  @throws If siginfo.si_pid == 0, then
         *          ndgpp::error<std::invalid_argument> is thrown
         */
        explicit
        status(const siginfo_t siginfo);

        /// Returns true if the subprocess has exited, false otherwise
        explicit operator bool() const noexcept;

        /// Returns true if the subprocess has exited, false otherwise
        bool exited() const noexcept;

        /** Returns true if the subprocess exited normally, false otherwise
         *
         *  A normal exit occurs when the subprocess either called
         *  _exit(2), exit(3), or exited via the return statement.
         */
        bool called_exit() const noexcept;

        /// Returns true if the subprocess was killed by a signal
        bool signaled() const noexcept;

        /// Returns true if the subprocess was killed by a signal and dumped a core
        bool dumped() const noexcept;

        /** Returns the signal value that killed the subprocess
         *
         *  @throws If the subprocess was not killed by a signal, then
         *          ndgpp::error<std::logic_error> is thrown.
         */
        int signal() const;

        /** Returns the least significant 8 bits of the subprocess's exit code
         *
         *  @throws If the subprocess has not called _exit(2),
         *          exit(3), or did not exit via the return statement,
         *          then ndgpp::error<std::logic_error> is thrown.
         */
        int exit_code() const;

        /** Returns the exit code of the subprocess
         *
         *  @throws If the subprocess has not exited, or was not
         *          killed by a signal, then
         *          ndgpp::error<std::logic_error> is thrown.
         */
        int exit_code_raw() const;

        private:

        int exit_code_raw_ = 0;
        bool exited_ = false;
        bool called_exit_ = false;
        bool signaled_ = false;
        bool dumped_ = false;
    };
}
}

#endif
