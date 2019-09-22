#include <stdexcept>
#include <cstdint>

#include <libndgpp/error.hpp>

#include "stack.hpp"

namespace detail
{
    static bool stack_grows_down(std::intptr_t caller)
    {
        char value;
        const std::intptr_t ptr_value = reinterpret_cast<std::intptr_t>(&value);
        if (ptr_value == caller)
        {
            throw ndgpp_error(std::logic_error, "cannot determine stack growth direction");
        }

        return ptr_value < caller;
    }

    static bool stack_grows_down()
    {
        char value;
        const std::intptr_t ptr_value = reinterpret_cast<std::intptr_t>(&value);
        return stack_grows_down(ptr_value);
    }
}

linuxpp::subprocess::stack::stack(const std::size_t size):
    grows_down_(detail::stack_grows_down()),
    size_(size),
    memory_(new char[size_])
{
    if (this->size_ == 0)
    {
        throw ndgpp_error(std::invalid_argument, "zero sized stacks are not allowed");
    }
}

void * linuxpp::subprocess::stack::get()
{
    return this->memory_.get() + (this->grows_down_ ? this->size_ - 1 : 0U);
}
