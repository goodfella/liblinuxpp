#include <memory>
#include <string>
#include <liblinuxpp/subprocess/argv.hpp>

static std::unique_ptr<char[]> copy(const std::string& str)
{
    std::unique_ptr<char[]> p(new char[str.length() + 1]);
    str.copy(p.get(), str.length());
    p[str.length()] = 0;
    return p;
}

void linuxpp::subprocess::argv::push_back(const std::string& arg)
{
    pointers_.push_back(copy(arg));
    data_.push_back(pointers_.back().get());
}

void linuxpp::subprocess::argv::push_back_t()
{}

linuxpp::subprocess::argv::argv()
{
    data_.push_back(nullptr);
}

linuxpp::subprocess::argv::argv(const std::string& executable):
    executable_(executable)
{
    this->push_back(executable);
    data_.push_back(nullptr);
}
