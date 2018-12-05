#ifndef LIBLINUXPP_INT_DESCRIPTOR_HPP
#define LIBLINUXPP_INT_DESCRIPTOR_HPP

#include <utility>
#include <type_traits>

#include <libndgpp/type_traits/is_class_defined.hpp>

namespace linuxpp
{
    template <class Tag, class T = int>
    struct int_descriptor_traits;

    struct int_descriptor_default_traits
    {
        static constexpr int invalid_value = -1;
    };

    /// Represents an integer based descriptor
    template <class Tag, class T = int>
    class int_descriptor
    {
        public:

        using type_traits = std::conditional_t<ndgpp::is_class_defined<int_descriptor_traits<Tag, T>>::value,
                                               int_descriptor_traits<Tag, T>,
                                               int_descriptor_default_traits>;
        using value_type = T;

        constexpr int_descriptor() noexcept;

        constexpr int_descriptor(const T invalid_value) noexcept;

        int_descriptor(const int_descriptor&) noexcept;
        int_descriptor& operator=(const int_descriptor&) noexcept;

        int_descriptor(int_descriptor&&) noexcept;
        int_descriptor& operator=(int_descriptor&&) noexcept;

        T get() const noexcept;

        void swap(int_descriptor& other) noexcept;

        void reset(T fd = type_traits::invalid_value) noexcept;

        T release() noexcept;

        explicit operator bool() const noexcept;

        private:

        T descriptor_ = type_traits::invalid_value;
    };

    template <class Tag, class T>
    constexpr int_descriptor<Tag, T>::int_descriptor() noexcept = default;

    template <class Tag, class T>
    int_descriptor<Tag, T>::int_descriptor(const int_descriptor&) noexcept = default;

    template <class Tag, class T>
    int_descriptor<Tag, T>& int_descriptor<Tag, T>::operator=(const int_descriptor&) noexcept = default;

    template <class Tag, class T>
    inline void swap(int_descriptor<Tag, T>& lhs, int_descriptor<Tag, T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <class Tag, class T>
    inline constexpr int_descriptor<Tag, T>::int_descriptor(const T descriptor) noexcept:
        descriptor_(descriptor)
    {}

    template <class Tag, class T>
    inline int_descriptor<Tag, T>::int_descriptor(int_descriptor&& other) noexcept:
        descriptor_(other.descriptor_)
    {
        other.descriptor_ = type_traits::invalid_value;
    }

    template <class Tag, class T>
    inline int_descriptor<Tag, T>& int_descriptor<Tag, T>::operator=(int_descriptor&& other) noexcept
    {
        this->descriptor_ = other.descriptor_;
        other.descriptor_ = type_traits::invalid_value;
        return *this;
    }

    template <class Tag, class T>
    inline void int_descriptor<Tag, T>::swap(int_descriptor& other) noexcept
    {
        std::swap(this->descriptor_, other.descriptor_);
    }

    template <class Tag, class T>
    inline void int_descriptor<Tag, T>::reset(const T descriptor) noexcept
    {
        this->descriptor_ = descriptor;
    }

    template <class Tag, class T>
    inline T int_descriptor<Tag, T>::release() noexcept
    {
        const auto old = this->descriptor_;
        this->descriptor_ = type_traits::invalid_value;
        return old;
    }

    template <class Tag, class T>
    inline T int_descriptor<Tag, T>::get() const noexcept
    {
        return this->descriptor_;
    }

    template <class Tag, class T>
    int_descriptor<Tag, T>::operator bool() const noexcept
    {
        return this->descriptor_ != type_traits::invalid_value;
    }

    template <class Tag, class T>
    inline bool operator==(const int_descriptor<Tag, T> lhs, const int_descriptor<Tag, T> rhs)
    {
        return lhs.get() == rhs.get();
    }

    template <class Tag, class T>
    inline bool operator!=(const int_descriptor<Tag, T> lhs, const int_descriptor<Tag, T> rhs)
    {
        return !(lhs == rhs);
    }

    template <class Tag, class T>
    inline bool operator<(const int_descriptor<Tag, T> lhs, const int_descriptor<Tag, T> rhs)
    {
        return lhs.get() < rhs.get();
    }
}

#endif
