#ifndef AGTB_GEODESY_BASE_HPP
#define AGTB_GEODESY_BASE_HPP

#pragma once

#include "../details/Macros.hpp"
#include "../Utils/Angles.hpp"

#include <concepts>

#include <gcem.hpp>

AGTB_GEODESY_BEGIN

template <typename T>
concept EllipsoidConcept = requires {
    { T::a } -> std::convertible_to<double>;
    { T::b } -> std::convertible_to<double>;
    { T::c } -> std::convertible_to<double>;
    { T::alpha } -> std::convertible_to<double>;
    { T::e1_2 } -> std::convertible_to<double>;
    { T::e2_2 } -> std::convertible_to<double>;
};

template <typename T>
concept GeodeticLatitudeConstantsConcept = requires {
    { T::B } -> std::convertible_to<double>;
    { T::t } -> std::convertible_to<double>;
    { T::nu_2 } -> std::convertible_to<double>;
    { T::W } -> std::convertible_to<double>;
    { T::V } -> std::convertible_to<double>;
};

#define AGTB_DEFINE_PRECISE_ELLIPSOID(_name, _a, _b, _c, _alpha, _e1_2, _e2_2) \
    struct _name                                                               \
    {                                                                          \
        constexpr static double                                                \
            a = _a,                                                            \
            b = _b,                                                            \
            c = _c,                                                            \
            alpha = _alpha,                                                    \
            e1_2 = _e1_2,                                                      \
            e2_2 = _e2_2;                                                      \
    }

#define AGTB_DEFINE_QUICK_ELLIPSOID(_name, _a, _b)                  \
    AGTB_DEFINE_PRECISE_ELLIPSOID(                                  \
        _name,                                                      \
        _a,                                                         \
        _b,                                                         \
        gcem::pow((_a), 2) / (_b),                                  \
        ((_a) - (_b)) / (_a),                                       \
        (gcem::pow(_a, 2) - gcem::pow(_b, 2)) / gcem::pow((_a), 2), \
        (gcem::pow(_a, 2) - gcem::pow(_b, 2)) / gcem::pow((_b), 2))

namespace Ellipsoid
{
    AGTB_DEFINE_PRECISE_ELLIPSOID(
        Krasovski,
        6'378'245,
        6'356'863.018'773'047'3,
        6'399'698.901'782'771'0,
        1.0 / 298.3,
        0.006'693'421'622'966,
        0.006'738'525'414'683);

    AGTB_DEFINE_PRECISE_ELLIPSOID(
        IE1975,
        6'378'140,
        6'356'755.288'157'528'7,
        6'399'596.651'988'010'5,
        1.0 / 298.257,
        0.066'694'384'999'588,
        0.006'739'501'819'473);

    AGTB_DEFINE_PRECISE_ELLIPSOID(
        WGS84,
        6'378'137,
        6'356'752.314'2,
        6'399'593.625'8,
        1 / 298.257'223'563,
        0.006'694'379'990'13,
        0.006'739'496'742'77);

    AGTB_DEFINE_PRECISE_ELLIPSOID(
        CGCS2000,
        6'378'137,
        6'356'752.314'1,
        6'399'593.625'9,
        1 / 298.257'222'101,
        0.006'694'380'022'90,
        0.006'739'496'775'48);
}

struct GeodeticLatitude
{
    double B;
    constexpr GeodeticLatitude(double _B_rad) : B(_B_rad)
    {
        if (!Valid())
        {
            AGTB_THROW(std::invalid_argument, std::format("Invalid Latitude: \'{}\'", B));
        }
    }
    constexpr explicit operator double() const noexcept
    {
        return B;
    }
    constexpr bool Valid()
    {
        return gcem::abs(B) <= Utils::Angles::FromDMS(90);
    }
    template <std::floating_point T = double>
    constexpr T To() const noexcept
    {
        return static_cast<T>(*this);
    }
};

struct GeodeticLongitude
{
    double L;
    constexpr GeodeticLongitude(double _L_rad) : L(_L_rad)
    {
        if (!Valid())
        {
            AGTB_THROW(std::invalid_argument, std::format("Invalid Longitude: \'{}\'", L));
        }
    }
    constexpr explicit operator double() const noexcept
    {
        return L;
    }
    constexpr bool Valid()
    {
        return gcem::abs(L) <= Utils::Angles::FromDMS(180);
    }
    template <std::floating_point T = double>
    constexpr T To() const noexcept
    {
        return static_cast<T>(*this);
    }
};

template <EllipsoidConcept ellipsoid>
struct GeodeticLatitudeConstants
{
    double
        B,
        t,
        nu_2,
        W,
        V;
    constexpr GeodeticLatitudeConstants(GeodeticLatitude _B) : B(static_cast<double>(_B))
    {
        if (!_B.Valid())
        {
            AGTB_THROW(std::invalid_argument, std::format("invalid latitude: \'{}\'", B));
        }
        t = gcem::tan(B);
        nu_2 = ellipsoid::e2_2 * gcem::pow(gcem::cos(B), 2);
        W = gcem::sqrt(1 - ellipsoid::e1_2 * gcem::pow(gcem::sin(B), 2));
        V = gcem::sqrt(1 + nu_2);
    }
};

struct GeodeticConstants
{
    constexpr static double
        rho_degree = 57.295'779'513'082'321'0,
        rho_minute = 3'437.746'770'784'939'17,
        rho_second = 206'264.806'247'096'355;
};

enum class EllipsoidBasedOption
{
    General,
    Specified
#if defined(AGTB_ENABLE_USER_EXTENSION) && defined(AGTB_EllipsoidBasedOption_EXTENSION)
    ,
    AGTB_EllipsoidBasedAlgoOption_EXTENSION
#endif
};

AGTB_GEODESY_END

#endif