#pragma once

#ifndef AGTB_UTILS_ERROR_HPP
#define AGTB_UTILS_ERROR_HPP

#include <exception>

#include "../details/Macros.hpp"

AGTB_BEGIN

namespace Utils
{
    class not_implement_error : public std::runtime_error
    {
    public:
        explicit not_implement_error(const std::string &message = "Function not implemented")
            : std::runtime_error(message) {}

        explicit not_implement_error(const char *message = "Function not implemented")
            : std::runtime_error(message) {}
    };
}

AGTB_END

#endif