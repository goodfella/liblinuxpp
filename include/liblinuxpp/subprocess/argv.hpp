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

        argv(const std::string& executable);

        template <class T, class ... Args>
        argv(const std::string& executable,
             T&& t,
             Args&& ... args);

        argv(const argv&) = delete;
        argv& operator=(const argv&) = delete;

        argv(argv&&) = default;
        argv& operator=(argv&&) = default;

        char * const * data() const noexcept;

        const std::string& executable() const noexcept;

        private:

        void push_back(const std::string& arg);

        template <class T>
        void push_back_t(T&& t);


        template <class T, class ... Args>
        void push_back_t(T&& t, Args&& ... args);

        void push_back_t();

        std::string executable_;
        std::vector<std::unique_ptr<char[]>> pointers_;
        std::vector<char *> data_;
    };

    template <class T>
    void argv::push_back_t(T&& t)
    {
        std::ostringstream ss;
        ss << std::forward<T>(t);
        this->push_back(ss.str());
    }

    template <class T, class ... Args>
    argv::argv(const std::string& executable,
               T&& t,
               Args&& ... args):
        executable_(executable)
    {
        this->push_back(executable);
        this->push_back_t(std::forward<T>(t));
        this->push_back_t(std::forward<Args>(args)...);
        data_.push_back(nullptr);
    }

    inline char * const * argv::data() const noexcept
    {
        return data_.data();
    }

    inline const std::string& argv::executable() const noexcept
    {
        return executable_;
    }
}
}



#endif
