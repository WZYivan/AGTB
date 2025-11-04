#ifndef __AGTB_ADJUSTMENT_BASE_HPP__
#define __AGTB_ADJUSTMENT_BASE_HPP__

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

enum class RouteType
{
    Closed,
    Connecting
};

AGTB_ADJUSTMENT_END

#endif