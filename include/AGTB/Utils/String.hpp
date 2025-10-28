#pragma once

#ifndef AGTB_UTILS_STRING_HPP
#define AGTB_UTILS_STRING_HPP

#include <vector>
#include <string>
#include <cctype>
#include <ranges>
#include <algorithm>
#include <charconv>

#include "../details/Macros.hpp"

AGTB_UTILS_BEGIN

std::string LStrip(const std::string &str)
{
    auto beg = str.begin(),
         end = str.end();
    while (beg != end && std::isspace(static_cast<unsigned char>(*beg)))
    {
        ++beg;
    }

    if (beg == end)
    {
        return std::string();
    }

    return std::string(beg, end);
}
std::string RStrip(const std::string &str)
{
    auto end = str.rbegin();
    while (end != str.rend() && std::isspace(static_cast<unsigned char>(*end)))
    {
        ++end;
    }

    if (end == str.rend())
    {
        return std::string();
    }

    auto last_non_ws_pos = end.base() - 1;

    return std::string(str.begin(), last_non_ws_pos + 1);
}
std::string LRStrip(const std::string &str)
{
    return LStrip(RStrip(str));
}
std::string SkipWhiteSpace(const std::string &str)
{
    return str | std::views::filter([](const auto &chr)
                                    { return !isspace(chr); }) |
           std::ranges::to<std::string>();
}
template <typename T>
    requires std::is_arithmetic_v<T>
std::vector<T> SplitThenConv(const std::string &str, const std::string &sep)
{
    std::vector<T> vec{};

    for (auto sub_range : str | std::views::split(sep))
    {
        std::string sub(sub_range.begin(), sub_range.end());
        sub = LRStrip(sub);

        if (sub.empty())
            continue;

        T val{};
        auto [ptr, ec] = std::from_chars(sub.data(), sub.data() + sub.size(), val);

        if (ec != std::errc{})
        {
            AGTB_THROW(std::runtime_error, std::format("bad token: {}", sub));
        }

        vec.push_back(val);
    }

    return vec;
}

AGTB_UTILS_END

#endif