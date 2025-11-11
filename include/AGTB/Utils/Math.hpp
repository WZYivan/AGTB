#ifndef __AGTB_UTILS_MATH_HPP__
#define __AGTB_UTILS_MATH_HPP__

#include "../details/Macros.hpp"
#include <cmath>
#include <limits>
#include "gcem.hpp"

AGTB_BEGIN

/**
 * @brief Abs difference less than epsilon
 *
 * @param x
 * @param y
 * @return true
 * @return false
 */
constexpr bool ApproxEq(double x, double y)
{
    return gcem::abs(x - y) < std::numeric_limits<double>::epsilon();
}

/**
 * @brief ( > 0.5) -> 1; ( < 0.5) -> 0; ( == 0.5) -> 0(even) | 1(odd)
 *
 * @param x
 * @return constexpr double
 */
constexpr double Round(double x)
{
    double int_part;
    double frac_part = std::modf(x, &int_part);

    if (ApproxEq(frac_part, 0.5))
    {
        if (ApproxEq(gcem::fmod(int_part, 2), 0))
        {
            return int_part;
        }
        else
        {
            return int_part + (x >= 0 ? 1 : -1);
        }
    }
    else if (gcem::abs(frac_part) < 0.5)
    {
        return int_part;
    }
    else if (gcem::abs(frac_part) > 0.5)
    {
        return int_part + (x >= 0 ? 1 : -1);
    }
}

/**
 * @brief Take specifed precision
 *
 * @param x
 * @param p precision place
 * @return constexpr double
 */
constexpr double TakePlace(double x, int p)
{
    if (p == 0)
    {
        return Round(x);
    }
    double scale = gcem::pow(10, p);
    return Round(x * scale) / scale;
}

/**
 * @brief Minimum unit under specified precision
 *
 * @param place
 * @return constexpr double
 */
constexpr double MinUnit(int place)
{
    if (place == 0)
    {
        return 1;
    }

    return gcem::pow(0.1, place);
}

AGTB_END

#endif