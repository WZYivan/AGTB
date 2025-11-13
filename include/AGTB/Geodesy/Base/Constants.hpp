#ifndef __AGTB_GEODESY_BASE_CONSTANTS_HPP__
#define __AGTB_GEODESY_BASE_CONSTANTS_HPP__

#include "../../details/Macros.hpp"

AGTB_GEODESY_BEGIN

struct Constants
{
    constexpr static double
        rho_degree = 57.295'779'513'082'321'0,
        rho_minute = 3'437.746'770'784'939'17,
        rho_second = 206'264.806'247'096'355;
};

AGTB_GEODESY_END

#endif