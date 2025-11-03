#ifndef AGTB_ADJUSTMENT_BASE_HPP
#define AGTB_ADJUSTMENT_BASE_HPP

#include "../details/Macros.hpp"

#include <gcem.hpp>

AGTB_ADJUSTMENT_BEGIN

constexpr double TakePrecision(double x, int v)
{
    if (v == 0)
    {
        return gcem::round(x);
    }
    double scale = gcem::pow(10, v);
    return gcem::round(x * scale) / scale;
}

AGTB_ADJUSTMENT_END

#endif