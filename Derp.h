#pragma once
#include <cstdint>
#include <string>

// VS2013 does not support noexcept so we simulate it here
#define noexcept throw()

namespace Derp
{

    struct syntax_error : std::runtime_error
    {
        explicit syntax_error(const char* what) : std::runtime_error(what) {}
        explicit syntax_error(const std::string& what) : std::runtime_error(what) {}
    };



}
