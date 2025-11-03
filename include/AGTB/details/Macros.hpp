#pragma once

#ifndef AGTB_DETAILS_MACROS_HPP
#define AGTB_DETAILS_MACROS_HPP

#ifndef __GNUC__
#warning "This is a GNU C++ 23 lib"
#endif

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

#define AGTB_DSM_BEGIN \
    AGTB_BEGIN         \
    namespace DSM      \
    {

#define AGTB_DSM_END \
    }                \
    AGTB_END

#define AGTB_UTILS_BEGIN \
    AGTB_BEGIN           \
    namespace Utils      \
    {

#define AGTB_UTILS_END \
    }                  \
    AGTB_END

AGTB_BEGIN

namespace macros
{
    std::string error_msg(const std::string_view msg, const std::source_location location)
    {
        return std::format("\n[ AGTB_THROW ]:\nfile: {}({}:{}) `{}`:{}\n",
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

#define AGTB_WARNING(__msg) [[deprecated(__msg)]]

#define AGTB_WEAK_REQUIRE_MEMBER_TYPE(__Tp, __M, __RTp) {__Tp::__M}->std::convertible_to<__RTp>
#define AGTB_T_HAS_TYPED_MEMBER(__M, __RTp) AGTB_WEAK_REQUIRE_MEMBER_TYPE(T, __M, __RTp)
#define AGTB_WEAK_REQUIRE_MEMBER_RETURN_TYPE(__Tp, __M, __MCp, __RTp) {__Tp::__M(__MCp)}->std::convertible_to<__RTp>
#define AGTB_T_HAS_MEMBER_RETURN(__M, __MCp, __RTp) AGTB_WEAK_REQUIRE_MEMBER_RETURN_TYPE(T, __M, __MCp, __RTp)

#endif