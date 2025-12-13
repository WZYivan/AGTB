#ifndef __AGTB_GEODESY_SPATIAL_REFERENCE_HPP__
#define __AGTB_GEODESY_SPATIAL_REFERENCE_HPP__

#include "SpatialReference/Geo/Geodetic.hpp"
#include "SpatialReference/Proj/GaussKruger.hpp"

AGTB_GEODESY_BEGIN

namespace SpatialRef
{
    enum class GeographicCoordinateSystem
    {
        Geodetic
    };

    enum class ProjectCoordinateSystem
    {
        GaussKruger
    };

    template <ProjectCoordinateSystem __proj, auto... __args>
    struct ProjCsUtils
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    };

    // template <>
    template <GaussZoneInterval __zone_interval, Units __unit>
    struct ProjCsUtils<ProjectCoordinateSystem::GaussKruger, __zone_interval, __unit>
    {
        using Impl = Proj::GaussKruger::GaussProjCoeffSolver<__zone_interval, __unit>;
    };
}

using GeoCS = SpatialRef::GeographicCoordinateSystem;
using ProjCS = SpatialRef::ProjectCoordinateSystem;

template <ProjCS __proj, auto... __args>
using ProjUtils = SpatialRef::ProjCsUtils<__proj, __args...>::Impl;

AGTB_GEODESY_END

#endif