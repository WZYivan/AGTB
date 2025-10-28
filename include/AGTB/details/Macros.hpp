#pragma once

#ifndef AGTB_DETAILS_MACROS_HPP
#define AGTB_DETAILS_MACROS_HPP

#include <exception>
#include <source_location>
#include <format>

#define AGTB_BEGIN \
    namespace AGTB \
    {
#define AGTB_END }

#define AGTB_PHOTOGRAPHIC_BEGIN

#define AGTB_PHOTOGRAPHIC_BEGIN \
    AGTB_BEGIN                  \
    namespace Photographic      \
    {

#define AGTB_PHOTOGRAPHIC_END \
    }                         \
    AGTB_END

#define AGTB_GEODESY_BEGIN \
    AGTB_BEGIN             \
    namespace Geodesy      \
    {

#define AGTB_GEODESY_END \
    }                    \
    AGTB_END

AGTB_BEGIN

namespace macros
{
    std::string error_msg(const std::string_view msg, const std::source_location location)
    {
        return std::format("file: {}({}:{}) `{}`:{}\n",
                           location.file_name(),
                           location.line(),
                           location.column(),
                           location.function_name(),
                           msg);
    }

    template <typename error_type>
    [[noreturn]] void msg_throw(const std::string_view msg, const std::source_location location)
    {
        throw error_type(error_msg(msg, location));
    }
}

AGTB_END

#include "../Utils/Error.hpp"

#define AGTB_MACROS ::AGTB::macros::
#define AGTB_UTILS ::AGTB::Utils::

#define AGTB_ERROR_MSG(__msg) AGTB_MACROS error_msg(__msg)

#define AGTB_THROW(__error_type, __msg) AGTB_MACROS msg_throw<__error_type>(__msg, std::source_location::current())

#define AGTB_NOT_IMPLEMENT() \
    AGTB_THROW(AGTB_UTILS not_implement_error, "AGTB::Not implement this")

#endif