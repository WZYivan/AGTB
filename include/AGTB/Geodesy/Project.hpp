#ifndef __AGTB_GEODESY_PROJECT_HPP__
#define __AGTB_GEODESY_PROJECT_HPP__

#include "Projection/GaussKruger.hpp"
#include "SpatialReference.hpp"

#include <concepts>

AGTB_GEODESY_BEGIN

template <GeoCS __geo, ProjCS __proj>
struct Projector
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

namespace Projection::ConfigConcept
{
    template <typename T>
    concept GeodeticGaussKruger = requires {
        { T::ellipsoid } -> std::convertible_to<Ellipsoids>;
        { T::zone_interval } -> std::convertible_to<GaussZoneInterval>;
        { T::unit } -> std::convertible_to<Units>;

        typename T::GeoCoord;
        typename T::ProjCoord;
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
        using GeoCoord = GeodeticCoordinate<ellipsoid, unit>;
        using ProjCoord = GaussProjCoordinate<zone_interval>;
    };

    template <Projection::ConfigConcept::GeodeticGaussKruger __config>
    static inline __config::GeoCoord Project(const __config::ProjCoord &pc)
    {
        return Projection::GaussKruger::GaussProjToGeodetic<__config::ellipsoid, __config::zone_interval, __config::unit>(pc);
    }

    template <Projection::ConfigConcept::GeodeticGaussKruger __config>
    static inline __config::ProjCoord Project(const __config::GeoCoord &gc, int custom_zone = 0)
    {
        return Projection::GaussKruger::GeodeticToGaussProj<__config::ellipsoid, __config::zone_interval, __config::unit>(gc, custom_zone);
    }

    template <Projection::ConfigConcept::GeodeticGaussKruger __config>
    static inline __config::ProjCoord ReProject(const __config::ProjCoord &pc, int tar_zone)
    {
        return Projection::GaussKruger::TransformZone<__config::ellipsoid, __config::zone_interval, __config::unit>(pc, tar_zone);
    }

    template <Projection::ConfigConcept::GeodeticGaussKruger __config, typename __coordinate>
        requires std::convertible_to<__coordinate, typename __config::GeoCoord> ||
                 std::convertible_to<__coordinate, typename __config::ProjCoord>
    static inline Angle MeridianConvergence(const __coordinate &coord)
    {
        return Projection::GaussKruger::MeridianConvergence<__config::ellipsoid, __config::zone_interval, __config::unit>(coord);
    }

    using DirctionCorrectionResult = typename Projection::GaussKruger::DirctionCorrectionResult;

    template <Projection::ConfigConcept::GeodeticGaussKruger __config>
    static inline DirctionCorrectionResult DirectionCorrection(const __config::ProjCoord &beg, const __config::ProjCoord &end)
    {
        return Projection::GaussKruger::DirectionCorrection<__config::ellipsoid, __config::zone_interval, __config::unit>(beg, end);
    }

    template <Projection::ConfigConcept::GeodeticGaussKruger __config, typename __coordinate>
        requires std::convertible_to<__coordinate, typename __config::GeoCoord> ||
                 std::convertible_to<__coordinate, typename __config::ProjCoord>
    static inline double Strech(const __coordinate &coord)
    {
        return Projection::GaussKruger::Strech<__config::ellipsoid, __config::zone_interval, __config::unit>(coord);
    }

    template <Projection::ConfigConcept::GeodeticGaussKruger __config>
    static inline double DistanceCorrection(double S, const __config::GeoCoord &beg, const __config::GeoCoord &end)
    {
        return Projection::GaussKruger::DistanceCorrection<__config::ellipsoid, __config::zone_interval, __config::unit>(S, beg, end);
    }
};

AGTB_GEODESY_END

#endif