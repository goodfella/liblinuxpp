#include <memory>
#include <string>
#include <liblinuxpp/subprocess/argv.hpp>

std::unique_ptr<char[]> linuxpp::subprocess::argv::copy(const std::string& str)
{
    std::unique_ptr<char[]> p(new char[str.length() + 1]);
    str.copy(p.get(), str.length());
    p[str.length()] = 0;
    return p;
}

