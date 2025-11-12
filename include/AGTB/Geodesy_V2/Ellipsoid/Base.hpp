#ifndef __AGTB_GEODESY_ELLIPSOID_BASE_HPP__
#define __AGTB_GEODESY_ELLIPSOID_BASE_HPP__

#include "../../details/Macros.hpp"
#include "../Base/GeoLatLon.hpp"

#include <concepts>

AGTB_GEODESY_BEGIN

enum class EllipsoidType : size_t
{
    Krasovski,
    IE1975,
    WGS84,
    CGCS2000
};

enum class EllipsoidAlgoOption : size_t
{
    General,
    Specified
};

template <typename T>
concept LatitudeConstantsConcept = requires(T t) {
    { t.B } -> std::convertible_to<double>;
    { t.t } -> std::convertible_to<double>;
    { t.nu_2 } -> std::convertible_to<double>;
    { t.W } -> std::convertible_to<double>;
    { t.V } -> std::convertible_to<double>;
};

AGTB_GEODESY_END

#endif