#ifndef __AGTB_GEODESY_DATUM_UNITS_HPP__
#define __AGTB_GEODESY_DATUM_UNITS_HPP__

#include "../../details/Macros.hpp"

AGTB_GEODESY_BEGIN

enum class Units
{
    Radian,
    Degree,
    Minute,
    Second
};

struct Constants
{
    constexpr static double
        rho_degree = 57.295'779'513'082'321'0,
        rho_minute = 3'437.746'770'784'939'17,
        rho_second = 206'264.806'247'096'355;
};

template <Units __unit>
constexpr double Rho()
{
    if constexpr (__unit == Units::Degree)
    {
        return Constants::rho_degree;
    }
    else if constexpr (__unit == Units::Minute)
    {
        return Constants::rho_minute;
    }
    else if constexpr (__unit == Units::Second)
    {
        return Constants::rho_second;
    }
    else
    {
        AGTB_STATIC_THROW("Unknown unit");
    }
}

template <Units __unit>
constexpr double rho = Rho<__unit>();

AGTB_GEODESY_END

#endif