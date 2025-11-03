#pragma once

#ifndef AGTB_UTILS_ERROR_HPP
#define AGTB_UTILS_ERROR_HPP

#include <exception>

#include "../details/Macros.hpp"

AGTB_UTILS_BEGIN

class not_implement_error : public std::runtime_error
{
public:
    explicit not_implement_error(const std::string &message = "Function not implemented")
        : std::runtime_error(message) {}

    explicit not_implement_error(const char *message = "Function not implemented")
        : std::runtime_error(message) {}
};

class constructor_error : public std::invalid_argument
{
public:
    explicit constructor_error(const std::string &msg = "Construct failed")
        : std::invalid_argument(msg) {}
    explicit constructor_error(const char *msg = "Construct failed")
        : std::invalid_argument(msg) {}
};

AGTB_UTILS_END

#endif