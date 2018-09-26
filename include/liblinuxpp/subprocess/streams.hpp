#ifndef LIBLINUXPP_SUBPROCESS_STREAMS_HPP
#define LIBLINUXPP_SUBPROCESS_STREAMS_HPP

namespace linuxpp
{
namespace subprocess
{
    class stream;

    struct stdin
    {
        stdin() = default;

        explicit
        stdin(const linuxpp::subprocess::stream& s):
            stream{&s}
        {}

        linuxpp::subprocess::stream const * stream = nullptr;
    };

    struct stdout
    {
        stdout() = default;

        explicit
        stdout(const linuxpp::subprocess::stream& s):
            stream{&s}
        {}

        linuxpp::subprocess::stream const * stream = nullptr;
    };

    struct stderr
    {
        stderr() = default;

        explicit
        stderr(const linuxpp::subprocess::stream& s):
            stream{&s}
        {}

        linuxpp::subprocess::stream const * stream = nullptr;
    };

    struct streams
    {
        streams() = default;

        streams(const stdin& in_):
            in {in_}
        {}
        streams(const stdout& out_):
            out {out_}
        {}
        streams(const stderr& error_):
            error {error_}
        {}

        streams(const stdin& in_,
                const stdout& out_):
            in {in_},
            out {out_}
        {}
        streams(const stdin& in_,
                const stderr& error_):
            in {in_},
            error {error_}
        {}
        streams(const stdout& out_,
                const stderr& error_):
            out {out_},
            error {error_}
        {}

        streams(const stdin& in_,
                const stdout& out_,
                const stderr& error_):
            in {in_},
            out {out_},
            error {error_}
        {}

        stdin in;
        stdout out;
        stderr error;
    };
}
}

#endif
