#ifndef __AGTB_ADJUSTMENT_BASE_HPP__
#define __AGTB_ADJUSTMENT_BASE_HPP__

#include "../details/Macros.hpp"
#include "../Linalg/Base.hpp"
#include "../Utils/Math.hpp"

#include "../Geodesy/Datum.hpp"
#include "../Geodesy/Solution/OrdinaryPlane.hpp"

#include <gcem.hpp>
#include <numeric>

AGTB_ADJUSTMENT_BEGIN

using Linalg::Matrix;

enum class RouteType
{
    /**
     * @brief Known edge lies within route loop
     *
     */
    ClosedLoop,

    /**
     * @brief Known edge lies out range of route loop
     *
     */
    ClosedConnecting,

    /**
     * @brief Begin from known edge and end in another known edge
     *
     */
    Connecting,

    Net
};

struct PlaneCoordinate
{
    double x;
    double y;
};

decltype(auto) PlaneInverse(PlaneCoordinate c1, PlaneCoordinate c2)
{
    return Geodesy::Solution::OrdinaryPlane::InverseSolve(c1.x, c1.y, c2.x, c2.y);
}

constexpr double rho2 = Geodesy::rho<Geodesy::Units::Second>;

AGTB_ADJUSTMENT_END

#endif