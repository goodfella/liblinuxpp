#ifndef LIBLINUXPP_IOVEC_HPP
#define LIBLINUXPP_IOVEC_HPP

#include <sys/uio.h>

#include <array>
#include <utility>
#include <vector>

namespace linuxpp
{
    /** Returns a std::array of struct iovec
     *
     *  @note The buffers must non-const
     *  @tparam Args The buffers to use for iovec initialization
     */
    template <class ... Args>
    std::array<struct ::iovec, sizeof...(Args)> make_iovec_array(Args && ... args);

    /** Returns a std::array of struct iovec
     *
     *  @note The buffers must be const
     *  @tparam Args the buffers to use for iovec initialization
     */
    template <class ... Args>
    std::array<struct ::iovec, sizeof...(Args)> make_iovec_array_const(Args && ... args);

    /** @defgroup linuxpp::make_iovec linuxpp::make_iovec
     *
     *  Returns an iovec structure initialized based on the parameters
     *  passed.  All the "buffer" parameter types must be non-const.
     *
     *  @tparam T The type of objects the iovec buffer points to
     *
     *  @param elements A buffer of objects of a given type.  This
     *                  buffer is stored in iovec::iov_base.
     *
     *  @param size_elements The size of the elements parameter.  This
     *                       value multipled by sizeof(T) is stored in
     *                       iovec::iov_len.
     *
     *  @param buf A void buffer that's stored in iovec::iov_base
     *
     *  @param size_buf The number of bytes the buf parameter points
     *                  to.  This value is stored in iovec::iov_len.
     *
     *  @param vector A std::vector<T>.  The value of vector.data() is
     *                stored in iovec::iov_base and the value of
     *                vector.size() multipled by sizeof(T) is stored
     *                in iovec::iov_len
     *
     *  @param array A std::array<T>.  The value of array.data() is
     *               stored in iovec::iov_base and the alue of
     *               array.size() multiplied by sizeof(T) is stored in
     *               iovec::iov_len.
     *
     *  @param pair A std::pair of one of the following types: \code T
     *              * const, std::size_t \endcode , \code T *, std::size_t
     *              \endcode , \code void * const, std::size_t \endcode , \code
     *              void *, std::size_t \endcode.  The value of pair.first is
     *              stored in iovec::iov_base and the value of pair.second is
     *              stored in iovec::iov_len.
     *
     *  @{
     */

    template <class T>
    struct ::iovec make_iovec(T * const elements,
                              const std::size_t size_elements);

    template <>
    struct ::iovec make_iovec(void * buf,
                              const std::size_t size_buf);

    template <class T>
    struct ::iovec make_iovec(std::vector<T> & vector);

    template <class T>
    struct ::iovec make_iovec(std::pair<T *, std::size_t> pair);

    template <>
    struct ::iovec make_iovec(std::pair<void *, std::size_t> pair);

    template <class T>
    struct ::iovec make_iovec(std::pair<T * const, std::size_t> pair);

    template <>
    struct ::iovec make_iovec(std::pair<void * const, std::size_t> pair);

    template <class T, std::size_t N>
    struct ::iovec make_iovec(std::array<T, N> & array);
    /// @}


    /** @defgroup linuxpp::make_iovec_const linuxpp::make_iovec_const
     *
     *  Returns an iovec structure initialized based on the parameters
     *  passed.  All the "buffer" parameter types must be const.
     *
     *  @tparam T The type of objects the iovec buffer points to
     *
     *  @param elements A buffer of objects of a given type.  This
     *                  buffer is stored in iovec::iov_base.
     *
     *  @param size_elements The size of the elements parameter.  This
     *                       value multipled by sizeof(T) is stored in
     *                       iovec::iov_len.
     *
     *  @param buf A void buffer that's stored in iovec::iov_base
     *
     *  @param size_buf The number of bytes the buf parameter points
     *                  to.  This value is stored in iovec::iov_len.
     *
     *  @param vector A std::vector<T>.  The value of vector.data() is
     *                stored in iovec::iov_base and the value of
     *                vector.size() multipled by sizeof(T) is stored
     *                in iovec::iov_len
     *
     *  @param array A std::array<T>.  The value of array.data() is
     *               stored in iovec::iov_base and the alue of
     *               array.size() multiplied by sizeof(T) is stored in
     *               iovec::iov_len.
     *
     *  @param pair A std::pair of one of the following types: \code T
     *              * const, std::size_t \endcode , \code T *, std::size_t
     *              \endcode , \code void * const, std::size_t \endcode , \code
     *              void *, std::size_t \endcode.  The value of pair.first is
     *              stored in iovec::iov_base and the value of pair.second is
     *              stored in iovec::iov_len.
     *
     *  @{
     */

    template <class T>
    struct ::iovec make_iovec_const(T const * const elements,
                                    const std::size_t size_elements);

    template <>
    struct ::iovec make_iovec_const(void const * const buf,
                                    const std::size_t size_buf);

    template <class T>
    struct ::iovec make_iovec_const(const std::vector<T> & vector);

    template <class T>
    struct ::iovec make_iovec_const (std::pair<T const *, std::size_t> pair);

    template <>
    struct ::iovec make_iovec_const (std::pair<void const *, std::size_t> pair);

    template <class T>
    struct ::iovec make_iovec_const (std::pair<T const * const, std::size_t> pair);

    template <>
    struct ::iovec make_iovec_const (std::pair<void const * const, std::size_t> pair);

    template <class T, std::size_t N>
    struct ::iovec make_iovec_const(const std::array<T, N> & array);
    /// @}


    /** @defgroup linuxpp::fill_iovec linuxpp::fill_iovec
     *
     *  Returns an iovec structure initialized based on the parameters
     *  passed.
     *
     *  @tparam T The type of objects the iovec buffer points to
     *
     *  @param elements A buffer of objects of a given type.  This
     *                  buffer is stored in iovec::iov_base.
     *
     *  @param size_elements The size of the elements parameter.  This
     *                       value multipled by sizeof(T) is stored in
     *                       iovec::iov_len.
     *
     *  @param buf A void buffer that's stored in iovec::iov_base
     *
     *  @param size_buf The number of bytes the buf parameter points
     *                  to.  This value is stored in iovec::iov_len.
     *
     *  @param vector A std::vector<T>.  The value of vector.data() is
     *                stored in iovec::iov_base and the value of
     *                vector.size() multipled by sizeof(T) is stored
     *                in iovec::iov_len
     *
     *  @param array A std::array<T>.  The value of array.data() is
     *               stored in iovec::iov_base and the alue of
     *               array.size() multiplied by sizeof(T) is stored in
     *               iovec::iov_len.
     *
     *  @param pair A std::pair of one of the following types: \code T
     *              * const, std::size_t \endcode , \code T *, std::size_t
     *              \endcode , \code void * const, std::size_t \endcode , \code
     *              void *, std::size_t \endcode.  The value of pair.first is
     *              stored in iovec::iov_base and the value of pair.second is
     *              stored in iovec::iov_len.
     *
     *  @{
     */

    template <class T, std::size_t N>
    void fill_iovec(struct ::iovec & iovec,
                    std::array<T, N> & array);

    template <class T>
    void fill_iovec(struct ::iovec & iovec,
                    std::vector<T> & vector);

    template <class T>
    void fill_iovec(struct ::iovec & iovec,
                    std::pair<T*, std::size_t> pair);

    template <class T>
    void fill_iovec(struct ::iovec & iovec,
                    T * const elements,
                    const std::size_t size_elements);

    template <>
    void fill_iovec(struct ::iovec & iovec,
                    void * const buf,
                    const std::size_t size_buf);
    /// @}


    /** @defgroup linuxpp::fill_iovec_const linuxpp::fill_iovec_const
     *
     *  Returns an iovec structure initialized based on the parameters
     *  passed.  All the "buffer" parameters must be const.
     *
     *  @tparam T The type of objects the iovec buffer points to
     *
     *  @param elements A buffer of objects of a given type.  This
     *                  buffer is stored in iovec::iov_base.
     *
     *  @param size_elements The size of the elements parameter.  This
     *                       value multipled by sizeof(T) is stored in
     *                       iovec::iov_len.
     *
     *  @param buf A void buffer that's stored in iovec::iov_base
     *
     *  @param size_buf The number of bytes the buf parameter points
     *                  to.  This value is stored in iovec::iov_len.
     *
     *  @param vector A std::vector<T>.  The value of vector.data() is
     *                stored in iovec::iov_base and the value of
     *                vector.size() multipled by sizeof(T) is stored
     *                in iovec::iov_len
     *
     *  @param array A std::array<T>.  The value of array.data() is
     *               stored in iovec::iov_base and the alue of
     *               array.size() multiplied by sizeof(T) is stored in
     *               iovec::iov_len.
     *
     *  @param pair A std::pair of one of the following types: \code T
     *              * const, std::size_t \endcode , \code T *, std::size_t
     *              \endcode , \code void * const, std::size_t \endcode , \code
     *              void *, std::size_t \endcode.  The value of pair.first is
     *              stored in iovec::iov_base and the value of pair.second is
     *              stored in iovec::iov_len.
     *
     *  @{
     */

    template <class T, std::size_t N>
    void fill_iovec_const(struct ::iovec & iovec,
                          const std::array<T, N> & array);

    template <class T>
    void fill_iovec_const(struct ::iovec & iovec,
                          const std::vector<T> & vector);

    template <class T>
    void fill_iovec_const(struct ::iovec & iovec,
                          std::pair<T const *, std::size_t> pair);

    template <>
    void fill_iovec_const(struct ::iovec & iovec,
                          std::pair<void const *, std::size_t> pair);

    template <class T>
    void fill_iovec_const(struct ::iovec & iovec,
                          T const * const elements,
                          const std::size_t size_elements);

    template <>
    void fill_iovec_const(struct ::iovec & iovec,
                          void const * const buf,
                          const std::size_t size_buf);
    /// @}
}


// Public function definitions follow

template <class ... Args>
std::array<struct ::iovec, sizeof...(Args)>
linuxpp::make_iovec_array(Args && ... args)
{
    std::array<struct ::iovec, sizeof...(args)> array = {linuxpp::make_iovec(std::forward<Args>(args))...};
    return array;
}

template <class ... Args>
std::array<struct ::iovec, sizeof...(Args)>
linuxpp::make_iovec_array_const(Args && ... args)
{
    std::array<struct ::iovec, sizeof...(args)> array = {linuxpp::make_iovec_const(std::forward<Args>(args))...};
    return array;
}

template <class T>
inline void linuxpp::fill_iovec(struct ::iovec & iovec,
                                T * const buf,
                                const std::size_t size_buf)
{
    linuxpp::fill_iovec(iovec, static_cast< void * const>(buf), sizeof(T) * size_buf);
}

template <>
inline void linuxpp::fill_iovec(struct ::iovec & iovec,
                                void * const buf,
                                const std::size_t size_buf)
{
    iovec.iov_base = buf;
    iovec.iov_len = size_buf;
}

template <class T>
inline void linuxpp::fill_iovec(struct ::iovec & iovec,
                                std::vector<T> & vector)
{
    linuxpp::fill_iovec(iovec, vector.data(), vector.size());
}

template <class T, std::size_t N>
inline void linuxpp::fill_iovec(struct ::iovec & iovec,
                                std::array<T, N> & array)
{
    linuxpp::fill_iovec(iovec, array.data(), array.size());
}

template <class T>
inline void linuxpp::fill_iovec(struct ::iovec & iovec,
                                std::pair<T *, std::size_t> pair)
{
    linuxpp::fill_iovec(iovec, pair.first, pair.second);
}


template <class T>
inline void linuxpp::fill_iovec_const(struct ::iovec & iovec,
                                      const T * const buf,
                                      const std::size_t size_buf)
{
    linuxpp::fill_iovec_const(iovec, static_cast<void const * const>(buf), sizeof(T) * size_buf);
}

template <>
inline void linuxpp::fill_iovec_const(struct ::iovec & iovec,
                                      void const * const buf,
                                      const std::size_t size_buf)
{
    iovec.iov_base = const_cast<void * const>(buf);
    iovec.iov_len = size_buf;
}

template <class T>
inline void linuxpp::fill_iovec_const(struct ::iovec & iovec,
                                      const std::vector<T> & vector)
{
    linuxpp::fill_iovec_const(iovec, vector.data(), vector.size());
}

template <class T, std::size_t N>
inline void linuxpp::fill_iovec_const(struct ::iovec & iovec,
                                      const std::array<T, N> & array)
{
    linuxpp::fill_iovec_const(iovec, array.data(), array.size());
}

template <class T>
inline void linuxpp::fill_iovec_const(struct ::iovec & iovec,
                                      std::pair<T const *, std::size_t> pair)
{
    linuxpp::fill_iovec_const(iovec, pair.first, pair.second);
}

template <>
inline void linuxpp::fill_iovec_const(struct ::iovec & iovec,
                                      std::pair<void const *, std::size_t> pair)
{
    linuxpp::fill_iovec_const(iovec, pair.first, pair.second);
}

template <>
inline struct ::iovec linuxpp::make_iovec(void * buf,
                                          const std::size_t size_buf)
{
    struct ::iovec iovec;
    linuxpp::fill_iovec(iovec, buf, size_buf);
    return iovec;
}

template <class T>
inline struct ::iovec linuxpp::make_iovec(T * const buf,
                                          const std::size_t size_buf)
{
    return linuxpp::make_iovec(static_cast<void *>(buf), sizeof(T) * size_buf);
}

template <class T>
inline struct ::iovec linuxpp::make_iovec(std::vector<T> & buf)
{
    return linuxpp::make_iovec(buf.data(), buf.size());
}

template <>
inline struct ::iovec linuxpp::make_iovec(std::pair<void *, std::size_t> pair)
{
    return linuxpp::make_iovec(pair.first, pair.second);
}

template <class T>
inline struct ::iovec linuxpp::make_iovec(std::pair<T *, std::size_t> pair)
{
    return linuxpp::make_iovec(pair.first, pair.second);
}

template <>
inline struct ::iovec linuxpp::make_iovec(std::pair<void * const, std::size_t> pair)
{
    return linuxpp::make_iovec(pair.first, pair.second);
}

template <class T>
inline struct ::iovec linuxpp::make_iovec(std::pair<T * const, std::size_t> pair)
{
    return linuxpp::make_iovec(pair.first, pair.second);
}

template <class T, std::size_t N>
inline struct ::iovec linuxpp::make_iovec(std::array<T, N> & array)
{
    return linuxpp::make_iovec(array.data(), array.size());
}

template <>
inline struct ::iovec linuxpp::make_iovec_const(void const * const buf,
                                                const std::size_t size_buf)
{
    struct ::iovec iovec;
    linuxpp::fill_iovec_const(iovec, buf, size_buf);
    return iovec;
}

template <class T>
inline struct ::iovec linuxpp::make_iovec_const(T const * const buf,
                                                const std::size_t size_buf)
{
    return linuxpp::make_iovec_const(static_cast<void const * const>(buf), sizeof(T) * size_buf);
}

template <class T>
inline struct ::iovec linuxpp::make_iovec_const(const std::vector<T> & vector)
{
    return linuxpp::make_iovec_const(vector.data(), vector.size());
}

template <class T>
inline struct ::iovec linuxpp::make_iovec_const(std::pair<T const *, std::size_t> pair)
{
    return linuxpp::make_iovec_const(pair.first, pair.second);
}

template <>
inline struct ::iovec linuxpp::make_iovec_const(std::pair<void const *, std::size_t> pair)
{
    return linuxpp::make_iovec_const(pair.first, pair.second);
}

template <class T>
inline struct ::iovec linuxpp::make_iovec_const(std::pair<T const * const, std::size_t> pair)
{
    return linuxpp::make_iovec_const(pair.first, pair.second);
}

template <>
inline struct ::iovec linuxpp::make_iovec_const(std::pair<void const * const, std::size_t> pair)
{
    return linuxpp::make_iovec_const(pair.first, pair.second);
}

template <class T, std::size_t N>
inline struct ::iovec linuxpp::make_iovec_const(const std::array<T, N> & array)
{
    return linuxpp::make_iovec_const(array.data(), array.size());
}

#endif
