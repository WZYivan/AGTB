#include <exception>
#include <source_location>
#include <format>
#include <sstream>
#include <iostream>

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
        return std::format("\n[ AGTB_THROW ]\n[ LOCATION ]\n< file >\n{}\n< position >\n({}:{})\n< function >\n{}\n[ __MESSAGE__ ]\n{}\n",
                           location.file_name(),
                           location.line(),
                           location.column(),
                           location.function_name(),
                           msg);
    }
#define AGTB_EXCEPTION_MESSAGE_SPLIT_SEP "[ __MESSAGE__ ]"
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
    __AGTB_MACROS msg_throw<__error_type>(std::format("[ {} ]\n{}", #__error_type, __msg), std::source_location::current())

#define AGTB_STATIC_THROW(__msg) static_assert(false, __msg)

#define AGTB_NOT_IMPLEMENT() \
    AGTB_THROW(NotImplementError, "AGTB::[ Not implement this ]")

#define AGTB_TEMPLATE_NOT_SPECIALIZED() \
    AGTB_STATIC_THROW("This template must be specified to implement")

#define AGTB_FILE_NOT_IMPLEMENT() \
    AGTB_STATIC_THROW("This header is not implemented, you should not include this")

#define AGTB_WARNING(__msg) [[deprecated(__msg)]]