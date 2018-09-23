#ifndef LIBLINUXPP_OPEN_HPP
#define LIBLINUXPP_OPEN_HPP

#include <cerrno>
#include <new>
#include <system_error>
#include <tuple>

#include <libndgpp/error.hpp>
#include <liblinuxpp/no_cloexec.hpp>

namespace linuxpp
{
    /** Returns a file descriptor for the path, and flags provided
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param no_cloexec Do not add O_CLOEXEC to flags
     *
     *  @return A valid file descriptor
     *
     *  @throws ndgpp::error<std::system_error> in the case of error
     */
    int open(char const * const path,
             const int flags,
             linuxpp::no_cloexec_t no_cloexec);


    /** Returns a file descriptor for the path, flags, and mode provided
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *  @param no_cloexec Do not add O_CLOEXEC to flags
     *
     *  @return A valid file descriptor
     *
     *  @throws ndgpp::error<std::system_error> in the case of error
     */
    int open(char const * const path,
             const int flags,
             const int mode,
             linuxpp::no_cloexec_t no_cloexec);


    /** Returns a file descriptor for the path, and flags provided
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param no_throw Do not throw an exception
     *  @param no_cloexec Do not add O_CLOEXEC to flags
     *
     *  @return A std::tuple containing a file descriptor and an errno value
     *
     *  @throws nothing
     */
    std::tuple<int, std::error_code>
    open(char const * const path,
         const int flags,
         std::nothrow_t no_throw,
         linuxpp::no_cloexec_t no_cloexec);


    /** Returns a file descriptor for the path, flags and mode provided
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *  @param no_throw Do not throw an exception
     *  @param no_cloexec Do not add O_CLOEXEC to flags
     *
     *  @return std::tuple containing a file descriptor and an errno value
     *
     *  @throws nothing
     */
    std::tuple<int, std::error_code>
    open(char const * const path,
         const int flags,
         const int mode,
         std::nothrow_t no_throw,
         linuxpp::no_cloexec_t no_cloexec);


    /** Returns a file descriptor for the path, and flags + O_CLOEXEC
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *
     *  @return A valid file descriptor
     *
     *  @throws ndgpp::error<std::system_error> in the case of error
     */
    int open(char const * const path, const int flags);


    /** Returns a file descriptor for the path, and flags + O_CLOEXEC
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param errno_value The value of errno when an error is encountered
     *
     *  @return std::tuple containing a file descriptor and an errno value
     *
     *  @throws nothing
     */
    std::tuple<int, std::error_code>
    open(char const * const path, const int flags, std::nothrow_t nothrow);


    /** Returns a file descriptor for the path, flags + O_CLOEXEC, and mode provided
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *
     *  @return A valid file descriptor
     *
     *  @throws ndgpp::error<std::system_error> in the case of error
     */
    int open(char const * const path, const int flags, int mode);


    /** Returns a file descriptor for the path, flags + O_CLOEXEC, and mode provided
     *
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *  @param nothrow Do not throw an exception
     *
     *  @return std::tuple containing a file descriptor and an errno value
     *
     *  @throws nothing
     */
    std::tuple<int, std::error_code> open(char const * const path,
                                          const int flags,
                                          const int mode,
                                          std::nothrow_t nothrow);
}

#endif
