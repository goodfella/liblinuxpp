#include <stdexcept>
#include <cstdint>

#include <libndgpp/error.hpp>

#include "stack.hpp"

linuxpp::subprocess::stack::stack(const std::size_t size):
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
    // Per the clone man page all stacks grown downward on systems
    // that Linux runs on except for the HP PA architecture
    return (this->memory_.get() + this->size_);
}
