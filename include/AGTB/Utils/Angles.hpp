#ifndef AGTB_UTILS_ANGLES_HPP
#define AGTB_UTILS_ANGLES_HPP

#include "../details/Macros.hpp"
#include "../Utils/Error.hpp"

#include <gcem.hpp>
#include <numbers>
#include <tuple>

AGTB_UTILS_BEGIN

namespace Angles
{
    constexpr double Deg2Rad = std::numbers::pi / 180;
    constexpr double Rad2Deg = 180 / std::numbers::pi;
    constexpr double Pi = std::numbers::pi;

    constexpr inline bool IsValidMS(double minutes, double seconds)
    {
        return !(minutes < 0 || minutes > 59 || seconds < 0 || seconds >= 60);
    }

    constexpr inline double FromDMS(double degrees = 0, double minutes = 0, double seconds = 0)
    {
        if (minutes < 0 || minutes > 59 || seconds < 0 || seconds >= 60)
        {
            AGTB_THROW(std::domain_error, std::format("[{}:{}:{}] is not a valid DMS", degrees, minutes, seconds));
        }

        bool neg = gcem::signbit(degrees);
        degrees = gcem::abs(degrees);
        double all_degrees = (degrees * 3600 + minutes * 60 + seconds) / 3600.0;
        double rad = all_degrees * Deg2Rad;
        return (!neg) ? rad : -rad;
    }

    inline constexpr double
        rad_45d = FromDMS(45),
        rad_90d = FromDMS(90),
        rad_180d = FromDMS(180);

    constexpr inline double ToDegrees(double rad)
    {
        return rad * Rad2Deg;
    }
    constexpr inline double ToSeconds(double rad)
    {
        return ToDegrees(rad) * 3600.0;
    }

    constexpr inline auto ToDMS(double rad)
    {
        bool neg = gcem::signbit(rad);
        rad = gcem::abs(rad);
        double all_seconds = ToSeconds(rad);
        double
            degrees = gcem::floor(all_seconds / 3600),
            minutes = gcem::floor((all_seconds - degrees * 3600) / 60),
            seconds = all_seconds - degrees * 3600 - minutes * 60;
        return std::make_tuple((!neg) ? degrees : -degrees, minutes, seconds);
    }

    constexpr inline double NormalizedSym(double rad)
    {
        double result = gcem::fmod(rad + Pi, 2.0 * Pi);
        if (result < 0)
        {
            result += 2.0 * Pi;
        }
        return result - Pi;
    }

    constexpr inline double NormalizedStd(double rad)
    {
        double result = gcem::fmod(rad, 2.0 * Pi);
        if (result < 0)
        {
            result += 2.0 * Pi;
        }
        return result;
    }

    namespace DMS_Support
    {
        class AngleDMS
        {
        private:
            bool neg;
            int d, m;
            double s;

        public:
            constexpr AngleDMS(int _d, int _m, double _s)
                : d(_d), m(_m), s(_s)
            {
                AGTB_NOT_IMPLEMENT();

                if (!IsValidMS(m, s))
                {
                    AGTB_THROW(Utils::constructor_error, "Invalid DMS");
                }

                neg = d < 0;
                d = gcem::abs(d);
            }
            ~AngleDMS() = default;
        };
    }
}

AGTB_UTILS_END

#endif