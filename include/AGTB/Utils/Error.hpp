#pragma once

#ifndef __AGTB_UTILS_ERROR_HPP__
#define __AGTB_UTILS_ERROR_HPP__

#include <exception>

#include "../details/Macros.hpp"

AGTB_UTILS_BEGIN

/**
 * @brief Represent functions that exist but are not implemented.
 *
 */
class not_implement_error : public std::runtime_error
{
public:
    explicit not_implement_error(const std::string &message = "Function not implemented")
        : std::runtime_error(message) {}

    explicit not_implement_error(const char *message = "Function not implemented")
        : std::runtime_error(message) {}
};

/**
 * @brief Exception should only throw in constructor
 *
 */
class constructor_error : public std::invalid_argument
{
public:
    explicit constructor_error(const std::string &msg = "Construct failed")
        : std::invalid_argument(msg) {}
    explicit constructor_error(const char *msg = "Construct failed")
        : std::invalid_argument(msg) {}
};

/**
 * @brief Represent unsupported template parameter
 *
 */
class unknown_template_parameter_error : public std::invalid_argument
{
public:
    explicit unknown_template_parameter_error(const std::string &msg = "Unknown template parameter")
        : std::invalid_argument(msg) {}
    explicit unknown_template_parameter_error(const char *msg = "Unknown template parameter")
        : std::invalid_argument(msg) {}
};

AGTB_UTILS_END

#endif