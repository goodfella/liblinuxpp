#ifndef LIBLINUXPP_SUBPROCESS_ARGV_HPP
#define LIBLINUXPP_SUBPROCESS_ARGV_HPP

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace linuxpp
{
namespace subprocess
{
    class argv
    {
        public:

        argv();

        template <class T, class ... Args>
        argv(T&& t,
             Args&& ... args);

        argv(const argv&) = delete;
        argv& operator=(const argv&) = delete;

        argv(argv&&) = default;
        argv& operator=(argv&&) = default;

        std::vector<std::unique_ptr<char[]>>::const_iterator cbegin() const;
        std::vector<std::unique_ptr<char[]>>::const_iterator cend() const;

        std::size_t size() const;
        bool empty() const;

        template <class T>
        void push_back(T&& t);

        template <class T, class ... Args>
        void push_back(T&& t, Args&& ... args);

        void push_back(const std::string& arg);

        private:

        void push_back();
        std::unique_ptr<char[]> copy(const std::string& str);

        std::vector<std::unique_ptr<char[]>> pointers_;
    };

    template <class T, class ... Args>
    inline void argv::push_back(T&& t, Args&& ... args)
    {
        this->push_back(std::forward<T>(t));
        this->push_back(std::forward<Args>(args)...);
    }

    template <class T>
    inline void argv::push_back(T&& t)
    {
        std::ostringstream ss;
        ss << std::forward<T>(t);
        const std::string arg = ss.str();
        this->push_back(arg);
    }

    inline void argv::push_back(const std::string& arg)
    {
        this->pointers_.push_back(this->copy(arg));
    }

    inline void argv::push_back()
    {}

    inline argv::argv()
    {}

    template <class T, class ... Args>
    inline argv::argv(T&& t, Args&& ... args)
    {
        this->push_back(std::forward<T>(t));
        this->push_back(std::forward<Args>(args)...);
    }

    inline std::vector<std::unique_ptr<char[]>>::const_iterator argv::cbegin() const
    {
        return pointers_.cbegin();
    }

    inline std::vector<std::unique_ptr<char[]>>::const_iterator argv::cend() const
    {
        return pointers_.cend();
    }

    inline std::size_t argv::size() const {
        return pointers_.size();
    }

    inline bool argv::empty() const {
        return pointers_.empty();
    }
}
}

#endif
