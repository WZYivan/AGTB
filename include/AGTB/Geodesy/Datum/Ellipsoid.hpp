#ifndef __AGTB_GEODESY_DATUM_ELLIPSOID_HPP__
#define __AGTB_GEODESY_DATUM_ELLIPSOID_HPP__

#include "GeoLatLon.hpp"

#include <concepts>

#include <gcem.hpp>

AGTB_GEODESY_BEGIN

enum class Ellipsoids
{
    Krasovski,
    IE1975,
    WGS84,
    CGCS2000
};

namespace Concept
{
    template <typename ellipsoid_geometry>
    concept EllipsoidGeometry = requires {
        { ellipsoid_geometry::a } -> std::convertible_to<double>;
        { ellipsoid_geometry::b } -> std::convertible_to<double>;
        { ellipsoid_geometry::c } -> std::convertible_to<double>;
        { ellipsoid_geometry::alpha } -> std::convertible_to<double>;
        { ellipsoid_geometry::e1_2 } -> std::convertible_to<double>;
        { ellipsoid_geometry::e2_2 } -> std::convertible_to<double>;
        { ellipsoid_geometry::ellipsoid } -> std::convertible_to<Ellipsoids>;
    };
}

template <Ellipsoids __ellipsoid>
struct EllipsoidGeometry
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

template <>
struct EllipsoidGeometry<Ellipsoids::Krasovski>
{
    constexpr static double
        a = 6'378'245,
        b = 6'356'863.018'773'047'3,
        c = 6'399'698.901'782'771'0,
        alpha = 1.0 / 298.3,
        e1_2 = 0.006'693'421'622'966,
        e2_2 = 0.006'738'525'414'683;
    constexpr static Ellipsoids ellipsoid = Ellipsoids::Krasovski;
};

template <>
struct EllipsoidGeometry<Ellipsoids::IE1975>
{
    constexpr static double
        a = 6'378'140,
        b = 6'356'755.288'157'528'7,
        c = 6'399'596.651'988'010'5,
        alpha = 1.0 / 298.257,
        e1_2 = 0.006'694'384'999'588,
        e2_2 = 0.006'739'501'819'473;
    constexpr static Ellipsoids ellipsoid = Ellipsoids::IE1975;
};

template <>
struct EllipsoidGeometry<Ellipsoids::WGS84>
{
    constexpr static double
        a = 6'378'137,
        b = 6'356'752.314'2,
        c = 6'399'593.625'8,
        alpha = 1 / 298.257'223'563,
        e1_2 = 0.006'694'379'990'13,
        e2_2 = 0.006'739'496'742'77;
    constexpr static Ellipsoids ellipsoid = Ellipsoids::WGS84;
};

template <>
struct EllipsoidGeometry<Ellipsoids::CGCS2000>
{
    constexpr static double
        a = 6'378'137,
        b = 6'356'752.314'1,
        c = 6'399'593.625'9,
        alpha = 1 / 298.257'222'101,
        e1_2 = 0.006'694'380'022'90,
        e2_2 = 0.006'739'496'775'48;
    constexpr static Ellipsoids ellipsoid = Ellipsoids::CGCS2000;
};

/**
 * @brief More constants associated to latitude
 *
 * @tparam __ellipsoid_geometry
 */
template <Concept::EllipsoidGeometry __ellipsoid_geometry>
struct LatitudeConstants
{
    double
        B,
        t,
        nu_2,
        W,
        V;
    using ellipsoid_geometry = __ellipsoid_geometry;
    constexpr LatitudeConstants(Latitude<Units::Radian> _B) : B(_B.Rad())
    {
        t = gcem::tan(B);
        nu_2 = ellipsoid_geometry::e2_2 * gcem::pow(gcem::cos(B), 2);
        W = gcem::sqrt(1 - ellipsoid_geometry::e1_2 * gcem::pow(gcem::sin(B), 2));
        V = gcem::sqrt(1 + nu_2);
    }
};

namespace Concept
{
    template <typename T>
    concept LatitudeConstants = requires(T t) {
        { t.B } -> std::convertible_to<double>;
        { t.t } -> std::convertible_to<double>;
        { t.nu_2 } -> std::convertible_to<double>;
        { t.W } -> std::convertible_to<double>;
        { t.V } -> std::convertible_to<double>;
    };
}

AGTB_GEODESY_END

#endif