#ifndef LIBLINUXPP_INT_DESCRIPTOR_HPP
#define LIBLINUXPP_INT_DESCRIPTOR_HPP

#include <utility>
#include <type_traits>

#include <libndgpp/type_traits/is_class_defined.hpp>

namespace linuxpp
{
    /** A class that defines traits associated with an int_descriptor Tag
     *
     *  A specialization of this class should be defined if any of the
     *  following statements are true:
     *
     *  1. The int_descriptor's integer type is not an int
     *  2. The invalid value is not equal to -1
     *
     *  A specialization of this class shall contain the following:
     *
     * - A static constexpr member variable named 'invalid_value'
     *   whose type is the int_descriptor's integer type and whose
     *   value is equal the "invalid value" for the int_descriptor Tag
     *
     *  The specialization shall be defined in the linuxpp namespace
     */
    template <class Tag>
    struct int_descriptor_traits;

    /// The int_descriptor_traits type to use when a specialization
    /// for the int_descriptor_traits class does not exist
    struct int_descriptor_default_traits
    {
        static constexpr int invalid_value = -1;
    };

    /** Represents an integer based descriptor
     *
     *  @tparam Tag A type used to differentiate int_descriptors with the same integer type
     *  @tparam T The integer type of the descriptor
     */
    template <class Tag, class T = int>
    class int_descriptor final
    {
        public:

        using type_traits = std::conditional_t<ndgpp::is_class_defined<linuxpp::int_descriptor_traits<Tag>>::value,
                                               linuxpp::int_descriptor_traits<Tag>,
                                               linuxpp::int_descriptor_default_traits>;
        using value_type = T;

        /// Constructs an int_descriptor object assigned to type_traits::invalid_value
        constexpr int_descriptor() noexcept;

        /** Constructs an int_descriptor object assigned to the provided value
         *
         *  @param value The value the object is to be assigned to
         */
        explicit
        constexpr int_descriptor(const T value) noexcept;

        int_descriptor(const int_descriptor&) noexcept;
        int_descriptor& operator=(const int_descriptor&) noexcept;

        int_descriptor(int_descriptor&&) noexcept;
        int_descriptor& operator=(int_descriptor&&) noexcept;

        /// Returns the underlying descriptor value
        T get() const noexcept;

        /// Swaps two int_descriptors
        void swap(int_descriptor& other) noexcept;

        /** Sets the underlying descriptor to the provided value
         *
         *  @param fd The value the object is to be to assigned to
         */
        void reset(T fd = type_traits::invalid_value) noexcept;

        /** Sets the underlying descriptor to type_traits::invalid_value
         *
         *  @return The previously stored descriptor value
         */
        T release() noexcept;

        /// Returns true if the underlying descriptor is not equal to type_traits::invalid_value
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
    inline constexpr int_descriptor<Tag, T>::int_descriptor(const T value) noexcept:
        descriptor_(value)
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
