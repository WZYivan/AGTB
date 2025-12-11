#ifndef __AGTB_GEODESY_SPATIAL_REF_GAUSS_KRUGER_HPP__
#define __AGTB_GEODESY_SPATIAL_REF_GAUSS_KRUGER_HPP__

#include "../Datum/Ellipsoid.hpp"
#include "../Datum/GeoLatLon.hpp"

AGTB_GEODESY_BEGIN

/**
 * @brief Interval of gauss-kruger projection zone
 *
 */
enum class GaussZoneInterval : size_t
{
    D3,
    D6,
};

/**
 * @brief Convert `GaussZoneInterval` to double
 *
 * @tparam interval
 * @return constexpr double
 */
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

/**
 * @brief Convert zone number to longitude of its centerline
 *
 * @tparam interval
 * @param zone
 * @return Longitude
 */
template <GaussZoneInterval interval>
Longitude<Units::Radian> GaussProjCenterLongitude(int zone)
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
}

template <>
Longitude<Units::Radian> GaussProjCenterLongitude<GaussZoneInterval::D6>(int zone)
{
    return FromDMS(6 * zone - 3);
}

template <>
Longitude<Units::Radian> GaussProjCenterLongitude<GaussZoneInterval::D3>(int zone)
{
    return FromDMS(3 * zone);
}

/**
 * @brief Convert longitude to its projection zone
 *
 * @tparam interval
 * @param L
 * @return int
 */
template <GaussZoneInterval interval>
constexpr int GaussProjZone(Longitude<Units::Radian> L)
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
    return 0;
}

template <>
constexpr int GaussProjZone<GaussZoneInterval::D6>(Longitude<Units::Radian> L)
{
    return int(L.Rad() / FromDMS(6.0)) + 1;
}

template <>
constexpr int GaussProjZone<GaussZoneInterval::D3>(Longitude<Units::Radian> L)
{
    double rad_d3 = FromDMS(3.0);
    return int(L.Rad() / rad_d3) + (gcem::fmod(L.Rad(), rad_d3) > FromDMS(1.5) ? 1 : 0);
}

/**
 * @brief Convert longitude to longitude of its centerline of projection zone
 *
 * @tparam Z
 * @param l
 * @return Longitude
 */
template <GaussZoneInterval Z>
Longitude<Units::Radian> GaussProjCenterLongitude(Longitude<Units::Radian> l)
{
    return GaussProjCenterLongitude<Z>(GaussProjZone<Z>(l));
}

/**
 * @brief Coordinates of gauss-kruger projection
 *
 * @tparam __gzi
 */
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

AGTB_GEODESY_END

#endif