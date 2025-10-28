#ifndef AGTB_UTILS_ANGLES_HPP
#define AGTB_UTILS_ANGLES_HPP

#include "../details/Macros.hpp"

#include <gcem.hpp>
#include <numbers>
#include <tuple>

AGTB_UTILS_BEGIN

namespace Angles
{
    constexpr double FromDegrees(double degrees = 0, double minutes = 0, double seconds = 0)
    {
        return (degrees * 3600 + minutes * 60 + seconds) / (3600 * 180) * std::numbers::pi;
    }

    constexpr auto ToDegrees(double rad = 0)
    {
        double all_seconds = rad * 180 / std::numbers::pi;
        double
            degrees = gcem::floor(all_seconds / 3600),
            minutes = gcem::floor((all_seconds - degrees * 3600) / 60),
            seconds = all_seconds - degrees * 3600 - minutes * 60;
        return std::make_tuple(degrees, minutes, seconds);
    }
}

AGTB_UTILS_END

#endif