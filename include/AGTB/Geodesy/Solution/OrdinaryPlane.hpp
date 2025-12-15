#ifndef __AGTB_GEODESY_SOLUTION_ORDINARY_PLANE_HPP__
#define __AGTB_GEODESY_SOLUTION_ORDINARY_PLANE_HPP__

#include "Base.hpp"

AGTB_GEODESY_BEGIN

namespace Solution::OrdinaryPlane
{
    OrdinaryInverseResult InverseSolve(double x1, double y1, double x2, double y2)
    {
        double
            dx = x2 - x1,
            dy = y2 - y1,
            alpha = gcem::atan(dy / dx),
            d180r = 180 * deg2rad;
        alpha = (dy > 0 && dx > 0) ? alpha : ((dy > 0 && dx < 0) ? d180r - alpha : ((dy < 0 && dx < 0) ? d180r + alpha : ((dy < 0 && dx > 0) ? d180r * 2 - alpha : 0.0)));
        double s = gcem::sqrt(dx * dx + dy * dy);
        return {
            .a_forward = Angle::FromRad(alpha),
            .s = s};
    }

    OrdinaryForwardResult ForwardSolve(double x, double y, double s, Angle a_forward)
    {
        double
            dx = s * a_forward.Cos(),
            dy = s * a_forward.Sin();
        return {
            .x = x + dx,
            .y = y + dy};
    }
}

AGTB_GEODESY_END

#endif