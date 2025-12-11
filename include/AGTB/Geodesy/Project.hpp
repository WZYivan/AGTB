#ifndef __AGTB_GEODESY_PROJECT_HPP__
#define __AGTB_GEODESY_PROJECT_HPP__

#include "Projection/GaussKruger.hpp"

#include <concepts>

AGTB_GEODESY_BEGIN

template <GeoCS __geo, ProjCS __proj>
struct Projector
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
};

namespace ProjectConfigConcept
{
    template <typename T>
    concept GeodeticGaussKruger = requires {
        { T::ellipsoid } -> std::convertible_to<Ellipsoids>;
        { T::zone_interval } -> std::convertible_to<GaussZoneInterval>;
        { T::unit } -> std::convertible_to<Units>;

        typename T::geo_coord;
        typename T::proj_coord;
    };
}

template <>
struct Projector<GeoCS::Geodetic, ProjCS::GaussKruger>
{
    constexpr static GeoCS geo_cs = GeoCS::Geodetic;
    constexpr static ProjCS proj_cs = ProjCS::GaussKruger;

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    struct Config
    {
        constexpr static Ellipsoids ellipsoid = __ellipsoid;
        constexpr static GaussZoneInterval zone_interval = __zone_interval;
        constexpr static Units unit = __unit;
        using geo_coord = GeodeticCoordinate<ellipsoid, unit>;
        using proj_coord = GaussProjCoordinate<zone_interval>;
    };

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    static inline GeodeticCoordinate<__ellipsoid, __unit> Project(const GaussProjCoordinate<__zone_interval> &gpc)
    {
        return Projection::GaussKruger::GaussProjToGeodetic<__ellipsoid, __zone_interval, __unit>(gpc);
    }

    template <Ellipsoids __ellipsoid, GaussZoneInterval __zone_interval, Units __unit>
    static inline GaussProjCoordinate<__zone_interval> Project(const GeodeticCoordinate<__ellipsoid, __unit> &gc)
    {
        return Projection::GaussKruger::GeodeticToGaussProj<__ellipsoid, __zone_interval, __unit>(gc);
    }

    template <ProjectConfigConcept::GeodeticGaussKruger __config>
    static inline __config::geo_coord Project(const __config::proj_coord &pc)
    {
        return Project<__config::ellipsoid, __config::zone_interval, __config::unit>(pc);
    }

    template <ProjectConfigConcept::GeodeticGaussKruger __config>
    static inline __config::proj_coord Project(const __config::geo_coord &gc)
    {
        return Project<__config::ellipsoid, __config::zone_interval, __config::unit>(gc);
    }
};

AGTB_GEODESY_END

#endif