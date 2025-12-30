#pragma once

#ifndef __AGTB_DETAILS_MACROS_HPP__
#define __AGTB_DETAILS_MACROS_HPP__

#ifndef __GNUC__
#warning "This is a GNU C++ 23 lib"
#endif

#include <exception>
#include <source_location>
#include <format>
#include <sstream>
#include <iostream>

#define AGTB_BEGIN \
    namespace AGTB \
    {
#define AGTB_END }

#define AGTB_PHOTOGRAMMETRY_BEGIN \
    AGTB_BEGIN                    \
    namespace Photogrammetry      \
    {

#define AGTB_PHOTOGRAMMETRY_END \
    }                           \
    AGTB_END

#define AGTB_GEODESY_BEGIN \
    AGTB_BEGIN             \
    namespace Geodesy      \
    {

#define AGTB_GEODESY_END \
    }                    \
    AGTB_END

#define AGTB_APP_BEGIN    \
    AGTB_BEGIN            \
    namespace Application \
    {

#define AGTB_APP_END \
    }                \
    AGTB_END

#define AGTB_APP_CONSOLE_BEGIN \
    AGTB_APP_BEGIN             \
    namespace Console          \
    {

#define AGTB_APP_CONSOLE_END \
    }                        \
    AGTB_APP_END

#define AGTB_UTILS_BEGIN \
    AGTB_BEGIN           \
    namespace Utils      \
    {

#define AGTB_UTILS_END \
    }                  \
    AGTB_END

#define AGTB_ADJUSTMENT_BEGIN \
    AGTB_BEGIN                \
    namespace Adjustment      \
    {

#define AGTB_ADJUSTMENT_END \
    }                       \
    AGTB_END

#define AGTB_LINALG_BEGIN \
    AGTB_BEGIN            \
    namespace Linalg      \
    {

#define AGTB_LINALG_END \
    }                   \
    AGTB_END

#define AGTB_IO_BEGIN \
    AGTB_BEGIN        \
    namespace IO      \
    {

#define AGTB_IO_END \
    }               \
    AGTB_END

#define AGTB_CONTAINER_BEGIN \
    AGTB_BEGIN               \
    namespace Container      \
    {

#define AGTB_CONTAINER_END \
    }                      \
    AGTB_END

AGTB_BEGIN

namespace macros
{
    /**
     * @brief Styled error message
     *
     * @param msg Context message
     * @param location source location where you throw
     * @return std::string AGTB style message
     */
    std::string error_msg(const std::string_view msg, const std::source_location location)
    {
        return std::format("\n[ AGTB_THROW ]:\n> file: {}({}:{}) `{}`:\n> {}\n",
                           location.file_name(),
                           location.line(),
                           location.column(),
                           location.function_name(),
                           msg);
    }

    /**
     * @brief Noreturn throw exception
     *
     * @tparam error_type exception type with constructor(std::string)
     * @param msg error_type init param
     * @param location source location where you throw
     */
    template <typename error_type>
    [[noreturn]] void msg_throw(
        const std::string_view msg,
        const std::source_location location)
    {
        throw error_type(error_msg(msg, location));
    }
}

AGTB_END

#include "../Utils/Error.hpp"

#define __AGTB_MACROS ::AGTB::macros::
#define __AGTB_UTILS ::AGTB::Utils::

#define AGTB_ERROR_MSG(__msg) __AGTB_MACROS error_msg(__msg)

#define AGTB_THROW(__error_type, __msg) \
    __AGTB_MACROS msg_throw<__error_type>(__msg, std::source_location::current())

#define AGTB_STATIC_THROW(__msg) static_assert(false, __msg)

#define AGTB_NOT_IMPLEMENT() \
    AGTB_THROW(__AGTB_UTILS not_implement_error, "AGTB::[ Not implement this ]")

#define AGTB_TEMPLATE_NOT_SPECIFIED() \
    AGTB_STATIC_THROW("This template must be specified to implement")

#define AGTB_FILE_NOT_IMPLEMENT() \
    AGTB_STATIC_THROW("This header is not implemented, you should not include this")

#define AGTB_WARNING(__msg) [[deprecated(__msg)]]

#ifdef AGTB_ENABLE_DEBUG
#include <print>
#include <iostream>
#include <cassert>
#define AGTB_DEBUG true
#else
#define AGTB_DEBUG false
#endif
#ifndef AGTB_DEBUG
#define AGTB_DEBUG false
#endif

#define AGTB_WEAK_REQUIRE_MEMBER_TYPE(__Tp, __M, __RTp) {__Tp::__M}->std::convertible_to<__RTp>
#define AGTB_T_HAS_TYPED_MEMBER(__M, __RTp) AGTB_WEAK_REQUIRE_MEMBER_TYPE(T, __M, __RTp)
#define AGTB_WEAK_REQUIRE_MEMBER_RETURN_TYPE(__Tp, __M, __MCp, __RTp) {__Tp::__M(__MCp)}->std::convertible_to<__RTp>
#define AGTB_T_HAS_MEMBER_RETURN(__M, __MCp, __RTp) AGTB_WEAK_REQUIRE_MEMBER_RETURN_TYPE(T, __M, __MCp, __RTp)

#endif