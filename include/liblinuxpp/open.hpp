#ifndef LIBLINUXPP_OPEN_HPP
#define LIBLINUXPP_OPEN_HPP

#include <cerrno>
#include <new>
#include <system_error>
#include <tuple>

#include <libndgpp/error.hpp>
#include <liblinuxpp/nocloexec.hpp>
#include <liblinuxpp/syscall_return.hpp>

namespace linuxpp
{
    /** Returns a file descriptor for the path, and flags provided
     *
     *  @param path The path to open
     *  @param nocloexec Do not add O_CLOEXEC to flags
     *  @param flags The flags parameter for the open system call
     *
     *  @return A valid file descriptor
     *
     *  @throws ndgpp::error<std::system_error> in the case of error
     */
    int open(linuxpp::nocloexec_t nocloexec,
             char const * const path,
             const int flags);


    /** Returns a file descriptor for the path, flags, and mode provided
     *
     *  @param nocloexec Do not add O_CLOEXEC to flags
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *
     *  @return A valid file descriptor
     *
     *  @throws ndgpp::error<std::system_error> in the case of error
     */
    int open(linuxpp::nocloexec_t nocloexec,
             char const * const path,
             const int flags,
             const int mode);


    /** Returns a file descriptor for the path, and flags provided
     *
     *  @param no_throw Do not throw an exception
     *  @param nocloexec Do not add O_CLOEXEC to flags
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *
     *  @return A syscall_return<int> that contains the file descriptor
     *
     *  @throws nothing
     */
    linuxpp::syscall_return<int>
    open(std::nothrow_t no_throw,
         linuxpp::nocloexec_t nocloexec,
         char const * const path,
         const int flags);


    /** Returns a file descriptor for the path, flags and mode provided
     *
     *  @param no_throw Do not throw an exception
     *  @param nocloexec Do not add O_CLOEXEC to flags
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *
     *  @return linuxpp::syscall_return<int> that contains the fd
     *
     *  @throws nothing
     */
    linuxpp::syscall_return<int>
    open(std::nothrow_t,
         linuxpp::nocloexec_t,
         char const * const path,
         const int flags,
         const int mode);


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
     *  @return linuxpp::syscall_return<int> with the file descriptor
     *
     *  @throws nothing
     */
    linuxpp::syscall_return<int>
    open(std::nothrow_t nothrow, char const * const path, const int flags);


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
     *  @param nothrow Do not throw an exception
     *  @param path The path to open
     *  @param flags The flags parameter for the open system call
     *  @param mode The mode parameter for the open system call
     *
     *  @return linuxpp::syscall_return<int> that contains the fd
     *
     *  @throws nothing
     */
    linuxpp::syscall_return<int>
    open(std::nothrow_t,
         char const * const path,
         const int flags,
         const int mode);
}

#endif
