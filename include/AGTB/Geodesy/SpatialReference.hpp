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
    struct ProjUtils
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    };

    template <GaussZoneInterval __zone_interval, Units __unit>
    struct ProjUtils<ProjectCoordinateSystem::GaussKruger, __zone_interval, __unit>
    {
        using CoeffSolver = Proj::GaussKruger::GaussProjCoeffSolver<__zone_interval, __unit>;

        struct Using : private CoeffSolver
        {
            using CoeffSolver::CenterLongitude;
            using CoeffSolver::Zone;
        };
    };
}

using GeoCS = SpatialRef::GeographicCoordinateSystem;
using ProjCS = SpatialRef::ProjectCoordinateSystem;

template <ProjCS __proj, auto... __args>
using ProjUtils = SpatialRef::ProjUtils<__proj, __args...>::Using;

AGTB_GEODESY_END

#endif