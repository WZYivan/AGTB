#ifndef __AGTB_ADJUSTMENT_BASE_HPP__
#define __AGTB_ADJUSTMENT_BASE_HPP__

#include "../details/Macros.hpp"
#include "../Linalg/Base.hpp"
#include "../Utils/Math.hpp"

#include <gcem.hpp>
#include <numeric>

AGTB_ADJUSTMENT_BEGIN

using Linalg::Matrix;

enum class RouteType
{
    Closed,
    Connecting
};

AGTB_ADJUSTMENT_END

#endif