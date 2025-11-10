#ifndef __AGTB_UTILS_STD_CHAR_CONV_HPP__
#define __AGTB_UTILS_STD_CHAR_CONV_HPP__

#include "../../details/Macros.hpp"
#include <concepts>

AGTB_UTILS_BEGIN

/**
 * @brief String wrap of std::from_chars
 *
 * @tparam value_type
 * @param str
 * @param value
 * @return if error occurs -> false
 */
template <typename value_type>
    requires std::floating_point<value_type> || std::integral<value_type>
bool FromString(const std::string &str, value_type &value)
{
    const char *ptr = str.data();
    const size_t len = str.size();
    auto [__ptr, __ec] = std::from_chars(ptr, ptr + len, value);
    return __ec == std::errc{};
}

AGTB_UTILS_END

#endif