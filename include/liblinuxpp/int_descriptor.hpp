#ifndef LIBLINUXPP_INT_DESCRIPTOR_HPP
#define LIBLINUXPP_INT_DESCRIPTOR_HPP

#include <utility>
#include <type_traits>

#include <libndgpp/type_traits/is_class_defined.hpp>

namespace linuxpp
{
    template <class T>
    struct int_descriptor_traits;

    struct int_descriptor_default_traits
    {
        static constexpr int invalid_value = -1;
    };

    /// Represents an integer based descriptor
    template <class T>
    class int_descriptor
    {
        public:

        using type_traits = std::conditional_t<ndgpp::is_class_defined<int_descriptor_traits<T>>::value,
                                               int_descriptor_traits<T>,
                                               int_descriptor_default_traits>;

        constexpr int_descriptor(const int descriptor = type_traits::invalid_value) noexcept;

        int_descriptor(const int_descriptor&) = default;
        int_descriptor& operator=(const int_descriptor&) = default;

        int_descriptor(int_descriptor&&) noexcept;
        int_descriptor& operator=(int_descriptor&&) noexcept;

        int get() const noexcept;

        void swap(int_descriptor& other) noexcept;

        void reset(int fd = type_traits::invalid_value) noexcept;

        private:

        int descriptor_;
    };

    template <class T>
    inline void swap(int_descriptor<T>& lhs, int_descriptor<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <class T>
    inline constexpr int_descriptor<T>::int_descriptor(const int descriptor) noexcept:
        descriptor_(descriptor)
    {}

    template <class T>
    inline int_descriptor<T>::int_descriptor(int_descriptor<T>&& other) noexcept:
        descriptor_(other.descriptor_)
    {
        other.descriptor_ = type_traits::invalid_value;
    }

    template <class T>
    inline int_descriptor<T>& int_descriptor<T>::operator=(int_descriptor<T>&& other) noexcept
    {
        this->descriptor_ = other.descriptor_;
        other.descriptor_ = type_traits::invalid_value;
        return *this;
    }

    template <class T>
    inline void int_descriptor<T>::swap(int_descriptor<T>& other) noexcept
    {
        std::swap(this->descriptor_, other.descriptor_);
    }

    template <class T>
    inline void int_descriptor<T>::reset(const int descriptor) noexcept
    {
        this->descriptor_ = descriptor;
    }

    template <class T>
    inline int int_descriptor<T>::get() const noexcept
    {
        return this->descriptor_;
    }

    template <class T>
    inline bool operator==(const int_descriptor<T> lhs, const int_descriptor<T> rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <class T>
    inline bool operator!=(const int_descriptor<T> lhs, const int_descriptor<T> rhs)
    {
        return !(lhs == rhs);
    }

    template <class T>
    inline bool operator<(const int_descriptor<T> lhs, const int_descriptor<T> rhs)
    {
        return lhs.get() < rhs.get();
    }
}

#endif
