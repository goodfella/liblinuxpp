#ifndef LIBLINUXPP_SUBPROCESS_STACK_HPP
#define LIBLINUXPP_SUBPROCESS_STACK_HPP

#include <memory>

namespace linuxpp
{
namespace subprocess
{

    class stack
    {
        public:

        stack(const std::size_t size);

        void * get();

        private:

        bool grows_down_;
        std::size_t size_;
        std::unique_ptr<char[]> memory_;
    };
}
}

#endif
