#ifndef LIBNDGPP_LENGTH_PREFIXED_MESSAGE_HPP
#define LIBNDGPP_LENGTH_PREFIXED_MESSAGE_HPP

#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>
#include <vector>

#include <libndgpp/network_byte_order.hpp>
#include <liblinuxpp/iovec.hpp>

namespace linuxpp
{
    template <class T>
    class length_prefixed_message_buffer
    {
        public:

        explicit
        length_prefixed_message_buffer(void const * buf, const T length) noexcept;

        std::array<struct ::iovec, 2> & iovec() noexcept;

        const std::array<struct ::iovec, 2> & iovec() const noexcept;

        private:

        ndgpp::network_byte_order<T> length_;
        std::array<struct ::iovec, 2> iovec_;
    };

    template <class T>
    class length_prefixed_message_vector
    {
        public:

        length_prefixed_message_vector();

        explicit
        length_prefixed_message_vector(struct ::iovec const * const buffers,
                                       const std::size_t size_buffers);

        std::vector<struct ::iovec> & iovec() noexcept;

        const std::vector<struct ::iovec> & iovec() const noexcept;

        void fill(struct ::iovec const * const buffers,
                  const std::size_t size_buffers);

        private:

        ndgpp::network_byte_order<T> length_;
        std::vector<struct ::iovec> iovec_;
    };

    template <class T, std::size_t N>
    class length_prefixed_message_array
    {
        public:

        explicit
        length_prefixed_message_array(struct ::iovec const * const buffers);

        const std::array<struct ::iovec, N + 1> & iovec() const noexcept;
        std::array<struct ::iovec, N + 1> & iovec() noexcept;

        private:

        ndgpp::network_byte_order<T> length_;
        std::array<struct ::iovec, N + 1> iovec_;
    };

    template <class T>
    inline
    length_prefixed_message_buffer<T>::length_prefixed_message_buffer(void const * buf, const T length) noexcept:
        length_{length},
        iovec_(linuxpp::make_iovec_array_const(std::make_pair(&this->length_, std::size_t{1U}),
                                               std::make_pair(buf, static_cast<std::size_t>(length))))
    {}

    template <class T>
    inline
    std::array<struct ::iovec, 2> & length_prefixed_message_buffer<T>::iovec() noexcept
    {
        return this->iovec_;
    }

    template <class T>
    inline
    const std::array<struct ::iovec, 2> & length_prefixed_message_buffer<T>::iovec() const noexcept
    {
        return this->iovec_;
    }

    template <class T>
    inline
    length_prefixed_message_vector<T>::length_prefixed_message_vector() = default;

    template <class T>
    inline
    length_prefixed_message_vector<T>::length_prefixed_message_vector(struct ::iovec const * const buffers,
                                                                      const std::size_t size_buffers):
        length_{static_cast<T>(linuxpp::accumulate(buffers, buffers + size_buffers))},
        iovec_{size_buffers + 1}
    {
        this->iovec_.front() =
            linuxpp::make_iovec_const(std::make_pair(&this->length_, std::size_t{1U}));
        std::copy(buffers, buffers + size_buffers, this->iovec_.begin() + 1);
    }

    template <class T>
    inline
    void length_prefixed_message_vector<T>::fill(struct ::iovec const * const buffers,
                                                 const std::size_t size_buffers)
    {
        this->length_ = static_cast<T>(linuxpp::accumulate(buffers, buffers + size_buffers));
        this->iovec_.resize(size_buffers + 1);
        this->iovec_.front() =
            linuxpp::make_iovec_const(std::make_pair(&this->length_, std::size_t{1U}));
        std::copy(buffers, buffers + size_buffers, this->iovec_.begin() + 1);
    }

    template <class T>
    inline
    std::vector<struct ::iovec> & length_prefixed_message_vector<T>::iovec() noexcept
    {
        return this->iovec_;
    }

    template <class T>
    inline
    const std::vector<struct ::iovec> & length_prefixed_message_vector<T>::iovec() const noexcept
    {
        return this->iovec_;
    }


    template <class T, std::size_t N>
    inline
    length_prefixed_message_array<T, N>::length_prefixed_message_array(struct ::iovec const * const buffers):
        length_{static_cast<T>(linuxpp::accumulate(buffers, buffers + N))}
    {
        this->iovec_.front() =
            linuxpp::make_iovec_const(std::make_pair(&this->length_, std::size_t{1U}));
        std::copy(buffers, buffers + N, this->iovec.begin());
    }

    template <class T, std::size_t N>
    inline
    std::array<struct ::iovec, N + 1> & length_prefixed_message_array<T, N>::iovec() noexcept
    {
        return this->iovec_;
    }

    template <class T, std::size_t N>
    inline
    const std::array<struct ::iovec, N + 1> & length_prefixed_message_array<T, N>::iovec() const noexcept
    {
        return this->iovec_;
    }
}

#endif
