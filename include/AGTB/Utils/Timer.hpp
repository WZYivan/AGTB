#ifndef __AGTB_UTILS_TIMER_HPP__
#define __AGTB_UTILS_TIMER_HPP__

#include "../details/Macros.hpp"

#include <chrono>
#include <iostream>
#include <format>

AGTB_UTILS_BEGIN

template <typename __clock = std::chrono::high_resolution_clock>
class Timer
{
public:
    using clock = __clock;
    using time_point = clock::time_point;
    using duration = clock::duration;

private:
    std::vector<time_point> tiks;

public:
    Timer() = default;
    ~Timer()
    {
        tiks.reserve(5);
    }

    void Tik(std::ostream &os = std::cout) noexcept
    {
        time_point tik{clock::now()};
        tiks.push_back(tik);
    }

    void Tok(std::ostream &os = std::cout) noexcept
    {
        time_point tok = clock::now();

        time_point tik = tiks.back();
        tiks.pop_back();
        duration costing = tok - tik;

        os << std::format("\n[ AGTB_TIMER ]\n> [{}] => [{}]~[{}]\n", costing, tik, tok);
    }

    void Now(std::ostream &os = std::cout) noexcept
    {
        time_point tok = clock::now();

        os << std::format("\n[ AGTB_TIMER ]\n> [{}]\n", tok);
    }
};

AGTB_UTILS_END

AGTB_BEGIN

Utils::Timer timer{};

AGTB_END

#endif