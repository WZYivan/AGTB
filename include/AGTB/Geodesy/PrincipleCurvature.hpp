#ifndef AGTB_GEODESY_PRINCIPLE_CURVATURE_HPP
#define AGTB_GEODESY_PRINCIPLE_CURVATURE_HPP

#pragma once

#include "../details/Macros.hpp"
#include "Base.hpp"

#include <concepts>
#include <tuple>
#include <gcem.hpp>

AGTB_GEODESY_BEGIN

template <typename T>
concept PrincipleCurvatureSpecifiedCoeffConcept = requires {
    { T::m0 } -> std::convertible_to<double>;
    { T::m2 } -> std::convertible_to<double>;
    { T::m4 } -> std::convertible_to<double>;
    { T::m6 } -> std::convertible_to<double>;
    { T::m8 } -> std::convertible_to<double>;

    { T::n0 } -> std::convertible_to<double>;
    { T::n2 } -> std::convertible_to<double>;
    { T::n4 } -> std::convertible_to<double>;
    { T::n6 } -> std::convertible_to<double>;
    { T::n8 } -> std::convertible_to<double>;
};

template <EllipsoidConcept ellipsoid>
struct PrincipleCurvatureSpecifiedCoefficient
{
    constexpr static double
        a = ellipsoid::a,
        e2 = ellipsoid::e1_2,

        m0 = a * (1 - e2),
        m2 = 3.0 / 2 * e2 * m0,
        m4 = 5.0 / 4 * e2 * m2,
        m6 = 7.0 / 6 * e2 * m4,
        m8 = 9.0 / 8 * e2 * m6,

        n0 = a,
        n2 = 1.0 / 2 * e2 * n0,
        n4 = 3.0 / 4 * e2 * n2,
        n6 = 5.0 / 6 * e2 * n4,
        n8 = 7.0 / 8 * e2 * n6;
};

#define AGTB_DEFINE_PRINCIPLE_CURVATURE_COEFF(_ellipsoid, _m0, _m2, _m4, _m6, _m8, _n0, _n2, _n4, _n6, _n8) \
    template <>                                                                                             \
    struct PrincipleCurvatureSpecifiedCoefficient<_ellipsoid>                                               \
    {                                                                                                       \
        constexpr static double                                                                             \
            m0 = _m0,                                                                                       \
            m2 = _m2,                                                                                       \
            m4 = _m4,                                                                                       \
            m6 = _m6,                                                                                       \
            m8 = _m8,                                                                                       \
            n0 = _n0,                                                                                       \
            n2 = _n2,                                                                                       \
            n4 = _n4,                                                                                       \
            n6 = _n6,                                                                                       \
            n8 = _n8;                                                                                       \
    }

AGTB_DEFINE_PRINCIPLE_CURVATURE_COEFF(Ellipsoid::Krasovski,
                                      6'335'552.717'00,
                                      63'609.788'33,
                                      532.208'92,
                                      4.156'02,
                                      0.031'30,

                                      6'378'245.000'00,
                                      21'346.141'49,
                                      107.159'04,
                                      0.597'72,
                                      0.003'50);

AGTB_DEFINE_PRINCIPLE_CURVATURE_COEFF(Ellipsoid::IE1975,
                                      6'335'442.275'00,
                                      63'617.835'00,
                                      532.353,
                                      4.158'00,
                                      0.031'00,

                                      6'378'140.000'00,
                                      21'348.862'00,
                                      107.188'00,
                                      0.598'00,
                                      0.003'00);

struct PrincipleCurvatureRadiiResult
{
    double M, N;
};

template <EllipsoidConcept ellipsoid>
PrincipleCurvatureRadiiResult PrincipleCurvatureRadii(GeodeticLatitude B)
{
    using coeff = PrincipleCurvatureSpecifiedCoefficient<ellipsoid>;
    double
        k = gcem::sin(static_cast<double>(B)),
        k2 = gcem::pow(k, 2),
        k4 = gcem::pow(k, 4),
        k6 = gcem::pow(k, 6),
        k8 = gcem::pow(k, 8),

        m0 = coeff::m0,
        m2 = coeff::m2,
        m4 = coeff::m4,
        m6 = coeff::m6,
        m8 = coeff::m8,

        n0 = coeff::n0,
        n2 = coeff::n2,
        n4 = coeff::n4,
        n6 = coeff::n6,
        n8 = coeff::n8,

        M = m0 + m2 * k2 + m4 * k4 + m6 * k6 + m8 * k8,
        N = n0 + n2 * k2 + n4 * k4 + n6 * k6 + n8 * k8;
    return {.M = M, .N = N};
};

AGTB_GEODESY_END

#endif