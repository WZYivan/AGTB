#ifndef __AGTB_GEODESY_SPATIAL_REFERENCE_HPP__
#define __AGTB_GEODESY_SPATIAL_REFERENCE_HPP__

#include "SpatialReference/Geodetic.hpp"
#include "SpatialReference/GaussKruger.hpp"

AGTB_GEODESY_BEGIN

namespace SpatialRef
{
    enum class GeographicCoordinateSystem
    {
        Geodetic
    };

    enum class ProjetCoordinateSystem
    {
        GaussKruger
    };
}

using GeoCS = SpatialRef::GeographicCoordinateSystem;
using ProjCS = SpatialRef::ProjetCoordinateSystem;

AGTB_GEODESY_END

#endif