#ifndef __AGTB_GEODESY_SPATIAL_REF_GEODETIC_HPP__
#define __AGTB_GEODESY_SPATIAL_REF_GEODETIC_HPP__

#include "../../Datum/Ellipsoid.hpp"
#include "../../Datum/GeoLatLon.hpp"

AGTB_GEODESY_BEGIN

namespace SpatialRef::Geo
{
    template <Ellipsoids __ellipsoid, Units __unit>
    struct GeodeticCoordinate
    {
        Longitude<__unit> L;
        Latitude<__unit> B;
        constexpr static Ellipsoids ellipsoid = __ellipsoid;
        constexpr static Units unit = __unit;
    };
}

using SpatialRef::Geo::GeodeticCoordinate;

AGTB_GEODESY_END

#endif