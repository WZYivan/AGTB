#ifndef __AGTB_GEODESY_SPATIAL_REF_GAUSS_KRUGER_HPP__
#define __AGTB_GEODESY_SPATIAL_REF_GAUSS_KRUGER_HPP__

#include "../../Datum/Ellipsoid.hpp"
#include "../../Datum/GeoLatLon.hpp"

AGTB_GEODESY_BEGIN

namespace SpatialRef::Proj::GaussKruger
{
    enum class GaussZoneInterval : size_t
    {
        D3,
        D6,
    };

    template <GaussZoneInterval interval>
    constexpr double ToDouble()
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();

        return 0.0;
    }

    template <>
    constexpr double ToDouble<GaussZoneInterval::D3>()
    {
        return 3.0;
    }

    template <>
    constexpr double ToDouble<GaussZoneInterval::D6>()
    {
        return 6.0;
    }

    template <GaussZoneInterval __zone_interval>
    constexpr double gauss_zone_interval = ToDouble<__zone_interval>();

    template <GaussZoneInterval __zone_interval, Units _unit>
    struct GaussProjCoeffSolver
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    };

    template <Units __unit>
    struct GaussProjCoeffSolver<GaussZoneInterval::D3, __unit>
    {
        static inline Longitude<__unit> CenterLongitude(int zone)
        {
            return (3 * zone) * deg2rad;
        }

        static inline int Zone(Longitude<__unit> L)
        {
            double
                rad_d3 = 3.0 * deg2rad,
                rad_d1_5 = rad_d3 / 2.0;
            return int(L.Rad() / rad_d3) + (gcem::fmod(L.Rad(), rad_d3) > rad_d1_5 ? 1 : 0);
        }

        static inline Longitude<__unit> CenterLongitude(Longitude<__unit> L)
        {
            return CenterLongitude(Zone(L));
        }
    };

    template <Units __unit>
    struct GaussProjCoeffSolver<GaussZoneInterval::D6, __unit>
    {
        static inline Longitude<__unit> CenterLongitude(int zone)
        {
            return (6 * zone - 3) * deg2rad;
        }

        static inline int Zone(Longitude<__unit> L)
        {
            return int(L.Rad() / FromDMS(6.0)) + 1;
        }

        static inline Longitude<__unit> CenterLongitude(Longitude<__unit> L)
        {
            return CenterLongitude(Zone(L));
        }
    };

    template <GaussZoneInterval __gzi>
    struct GaussProjCoordinate
    {
        constexpr static double interval = ToDouble<__gzi>();

        double x, y;
        int zone;

        double ZoneY()
        {
            return y + 500'000.0 + zone * 100'000'0;
        }
    };
}

using SpatialRef::Proj::GaussKruger::GaussProjCoordinate;
using SpatialRef::Proj::GaussKruger::GaussZoneInterval;

AGTB_GEODESY_END

#endif