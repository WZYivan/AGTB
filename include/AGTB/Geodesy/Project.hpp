#ifndef __AGTB_GEODESY_PROJECT_HPP__
#define __AGTB_GEODESY_PROJECT_HPP__

#include "Projection/GaussKruger.hpp"

AGTB_GEODESY_BEGIN

template <
    EllipsoidType __ellipsoid_type,
    GaussZoneInterval __zone_interval,
    EllipsoidAlgoOption __algo_opt = EllipsoidAlgoOption::General>
GaussProjCoordinate<__zone_interval> Project(const GeodeticCoordinate<__ellipsoid_type> &geo_coord)
{
    using projector = Projection::GaussKruger::Projector<__ellipsoid_type, __zone_interval, __algo_opt>;
    return projector::Forward(geo_coord);
}

template <
    EllipsoidType __ellipsoid_type,
    GaussZoneInterval __zone_interval,
    EllipsoidAlgoOption __algo_opt = EllipsoidAlgoOption::General>
GeodeticCoordinate<__ellipsoid_type> Project(const GaussProjCoordinate<__zone_interval> &gauss_coord)
{
    using projector = Projection::GaussKruger::Projector<__ellipsoid_type, __zone_interval, __algo_opt>;
    return projector::Inverse(gauss_coord);
}

template <
    EllipsoidType __ellipsoid_type,
    GaussZoneInterval __zone_interval,
    EllipsoidAlgoOption __algo_opt = EllipsoidAlgoOption::General>
struct GaussProjectTParam
{
    constexpr static EllipsoidType ellipsoid_type = __ellipsoid_type;
    constexpr static GaussZoneInterval zone_interval = __zone_interval;
    constexpr static EllipsoidAlgoOption algo_opt = __algo_opt;
    using GeoCoord = GeodeticCoordinate<ellipsoid_type>;
    using GaussCoord = GaussProjCoordinate<zone_interval>;
};

template <typename T>
concept GaussProjectTParamConcept = requires {
    { T::ellipsoid_type } -> std::convertible_to<EllipsoidType>;
    { T::zone_interval } -> std::convertible_to<GaussZoneInterval>;
    { T::algo_opt } -> std::convertible_to<EllipsoidAlgoOption>;
    typename T::GeoCoord;
    typename T::GaussCoord;
};

template <GaussProjectTParamConcept Tp>
Tp::GaussCoord Project(const typename Tp::GeoCoord &geo_coord)
{
    return Project<Tp::ellipsoid_type, Tp::zone_interval, Tp::algo_opt>(geo_coord);
}

template <GaussProjectTParamConcept Tp>
Tp::GeoCoord Project(const typename Tp::GaussCoord &gauss_coord)
{
    return Project<Tp::ellipsoid_type, Tp::zone_interval, Tp::algo_opt>(gauss_coord);
}

AGTB_GEODESY_END

#endif