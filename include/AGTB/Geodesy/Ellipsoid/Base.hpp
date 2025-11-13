#ifndef __AGTB_GEODESY_ELLIPSOID_BASE_HPP__
#define __AGTB_GEODESY_ELLIPSOID_BASE_HPP__

#include "../../details/Macros.hpp"
#include "../Base/GeoLatLon.hpp"

#include <concepts>

AGTB_GEODESY_BEGIN

enum class EllipsoidType : size_t
{
    Krasovski,
    IE1975,
    WGS84,
    CGCS2000
};

enum class EllipsoidAlgoOption : size_t
{
    General,
    Specified
};

AGTB_GEODESY_END

#endif