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

template <typename __value_type, size_t __N>
struct CubicSpline
{
    using value_type = __value_type;
    std::array<value_type, __N + 1>
        x_knots, y_knots;
    std::array<value_type, __N>
        a, b, c, d;
    value_type threshold;
    value_type x_max = 1, x_min = 0;
    size_t n_intervals = __N;

    constexpr value_type operator()(value_type x) const noexcept
    {
        if (x <= x_min)
        {
            double dx = x - x_knots[0];
            return DirectInvoke(dx, 0);
        }

        if (x >= x_max)
        {
            int last_idx = n_intervals - 1;
            double dx = x - x_knots[last_idx];
            return DirectInvoke(dx, last_idx);
        }

        size_t idx = 0;
        for (int i = 0; i != n_intervals; ++i)
        {
            if (x < x_knots[i + 1])
            {
                idx = i;
                break;
            }
        }

        double dx = x - x_knots[idx];
        return DirectInvoke(dx, idx);
    }

    constexpr value_type DirectInvoke(value_type dx, size_t idx) const noexcept
    {
        value_type v{a[idx] + b[idx] * dx +
                     c[idx] * gcem::pow(dx, 2) + d[idx] * gcem::pow(dx, 3)};
        return gcem::abs(v) < threshold ? v : threshold;
    }
};

AGTB_END

#endif