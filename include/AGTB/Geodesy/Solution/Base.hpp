#ifndef __AGTB_GEODESY_SOLUTION_BASE_HPP__
#define __AGTB_GEODESY_SOLUTION_BASE_HPP__

#include "../Datum.hpp"

AGTB_GEODESY_BEGIN

namespace Solution
{
    struct InverseResult
    {
        Angle a_forwards, a_backwards;
        double s;
    };

    template <Units __unit>
    struct ForwardResult
    {
        Longitude<__unit> L;
        Latitude<__unit> B;
        Angle a_backward;
    };

    struct OrdinaryInverseResult
    {
        Angle a_forward;
        double s;
    };

    struct OrdinaryForwardResult
    {
        double x, y;
    };
}

AGTB_GEODESY_END

#endif