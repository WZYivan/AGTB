#ifndef __AGTB_GEODESY_DATUM_BASIC_ELLIPSOID_MATH_HPP__
#define __AGTB_GEODESY_DATUM_BASIC_ELLIPSOID_MATH_HPP__

#include "Ellipsoid.hpp"
#include "../ExMath/PreCorrection.hpp"

AGTB_GEODESY_BEGIN

namespace EllipsoidMath
{
    /**
     * @brief Curvature radius of meridian(M) and prime vertical(N), also calculate average curvature radius(R) from M, N
     *
     */
    struct CurvatureRadiusCollection
    {
        double M, N;
        double R()
        {
            return gcem::sqrt(M * N);
        }
    };

    /**
     * @brief Coefficients to calculate principe curvature
     *
     * @tparam __ellipsoid
     */
    template <Ellipsoids __ellipsoid>
    struct PrincipleCurvatureRadiiCoeff
    {
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        constexpr static Ellipsoids ellipsoid = __ellipsoid;

        constexpr static double
            a = ellipsoid_geometry::a,
            e2 = ellipsoid_geometry::e1_2,

            m0 = a * (1 - e2),
            m2 = 3.0 / 2.0 * e2 * m0,
            m4 = 5.0 / 4.0 * e2 * m2,
            m6 = 7.0 / 6.0 * e2 * m4,
            m8 = 9.0 / 8.0 * e2 * m6,

            n0 = a,
            n2 = 1.0 / 2.0 * e2 * n0,
            n4 = 3.0 / 4.0 * e2 * n2,
            n6 = 5.0 / 6.0 * e2 * n4,
            n8 = 7.0 / 8.0 * e2 * n6;
    };

    template <>
    struct PrincipleCurvatureRadiiCoeff<Ellipsoids::Krasovski>
    {
        constexpr static double
            m0 = 6'335'552.717'00,
            m2 = 63'609.788'33,
            m4 = 532.208'92,
            m6 = 4.156'02,
            m8 = 0.031'30,
            n0 = 6'378'245.000'00,
            n2 = 21'346.141'49,
            n4 = 107.159'04,
            n6 = 0.597'72,
            n8 = 0.003'50;
        constexpr static Ellipsoids ellipsoid = Ellipsoids::Krasovski;
    };

    template <>
    struct PrincipleCurvatureRadiiCoeff<Ellipsoids::IE1975>
    {
        constexpr static double
            m0 = 6'335'442.275'00,
            m2 = 63'617.835'00,
            m4 = 532.353,
            m6 = 4.158'00,
            m8 = 0.031'00,
            n0 = 6'378'140.000'00,
            n2 = 21'348.862'00,
            n4 = 107.188'00,
            n6 = 0.598'00,
            n8 = 0.003'00;
        constexpr static Ellipsoids ellipsoid = Ellipsoids::IE1975;
    };

    template <typename T>
    concept PrincipleCurvatureRadiiCoeffConcept = requires {
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
        { T::ellipsoid } -> std::convertible_to<Ellipsoids>;
    };

    template <EllipsoidGeometryConcept __ellipsoid_geometry>
    CurvatureRadiusCollection PrincipleCurvatureRadiiImpl(Latitude<Units::Radian> B)
    {
        using ellipsoid_geometry = __ellipsoid_geometry;
        double sinBp2 = gcem::pow(B.Sin(), 2),
               a = ellipsoid_geometry::a,
               e1_2 = ellipsoid_geometry::e1_2,
               k = 1.0 - e1_2 * sinBp2;
        double M = a * (1.0 - e1_2) * gcem::pow(k, -1.5),
               N = a * gcem::pow(k, -0.5);
        return {
            .M = M,
            .N = N};
    }

    template <Ellipsoids __ellipsoid>
    CurvatureRadiusCollection PrincipleCurvatureRadii(Latitude<Units::Radian> B)
    {
        return PrincipleCurvatureRadiiImpl<EllipsoidGeometry<__ellipsoid>>(B);
    }

    template <PrincipleCurvatureRadiiCoeffConcept __coeff>
    struct QuarterArcCoeff
    {
    private:
        using Coeff = __coeff;
        constexpr static double
            m0 = Coeff::m0,
            m2 = Coeff::m2,
            m4 = Coeff::m4,
            m6 = Coeff::m6,
            m8 = Coeff::m8;

    public:
        constexpr static double
            a0 = (m0 +
                  m2 / 2.0 +
                  m4 * 3.0 / 8.0 +
                  m6 * 5.0 / 16.0 +
                  m8 * 35.0 / 128.0) *
                 Utils::Angles::deg2rad,
            a2 = (m2 / 2.0 +
                  m4 / 2.0 +
                  m6 * 15.0 / 32.0 +
                  m8 * 7.0 / 16.0) /
                 2.0,
            a4 = (m4 / 8.0 +
                  m6 * 3.0 / 16.0 +
                  m8 * 7.0 / 32.0) /
                 4.0,
            a6 = (m6 / 32.0 +
                  m8 / 16.0) /
                 6.0;
    };

    template <typename T>
    concept QuarterArcCoeffConcept = requires {
        { T::a0 } -> std::convertible_to<double>;
        { T::a2 } -> std::convertible_to<double>;
        { T::a4 } -> std::convertible_to<double>;
        { T::a6 } -> std::convertible_to<double>;
    };

    template <QuarterArcCoeffConcept __coeff>
    double MeridianArcLengthImpl(Latitude<Units::Radian> _B)
    {
        using coeff = __coeff;

        double a0 = coeff::a0,
               a2 = coeff::a2,
               a4 = coeff::a4,
               a6 = coeff::a6;
        double B = _B.Rad(),
               sin2B = gcem::sin(2 * B),
               sin4B = gcem::sin(4 * B),
               sin6B = gcem::sin(6 * B);
        double X = a0 * Utils::Angles::ToDegrees(B) -
                   a2 * sin2B +
                   a4 * sin4B -
                   a6 * sin6B;
        return X;
    }

    template <Ellipsoids __ellipsoid>
    double MeridianArcLength(Latitude<Units::Radian> B)
    {
        using principle_curvature_radii_coeff = PrincipleCurvatureRadiiCoeff<__ellipsoid>;
        using quarter_arc_coeff = QuarterArcCoeff<principle_curvature_radii_coeff>;
        return MeridianArcLengthImpl<quarter_arc_coeff>(B);
    }

    template <Ellipsoids __ellipsoid, QuarterArcCoeffConcept __coeff>
    Latitude<Units::Radian> MeridianArcBottomIterImpl(double len, double iter_threshold, bool enable_precorrectoin)
    {
        using coeff = __coeff;
        constexpr Ellipsoids ellipsoid = __ellipsoid;

        double Bf_cur = FromDMS(len / coeff::a0), Bf_next = Bf_cur, FB = 0; // rad
        double dB = 0.0;
        if (enable_precorrectoin)
        {
            dB = PreCorrection::MeridianArcBottom<ellipsoid>(len);
        }

        if (ApproxEq(Bf_cur, 0.0))
        {
            return Bf_cur;
        }

        do
        {
            Bf_cur = Bf_next;
            FB =
                -coeff::a2 * gcem::sin(2 * Bf_cur) +
                coeff::a4 * gcem::sin(4 * Bf_cur) -
                coeff::a6 * gcem::sin(6 * Bf_cur);
            Bf_next = FromDMS((len - FB) / coeff::a0); // deg -> rad
        } while (Bf_next - Bf_cur >= iter_threshold);
        return Bf_cur + dB; // return rad
    }

    template <Ellipsoids __ellipsoid>
    Latitude<Units::Radian> MeridianArcBottom(double len, double iter_threshold, bool enable_precorrectoin = true)
    {
        constexpr Ellipsoids ellipsoid = __ellipsoid;
        using principle_curvature_radii_coeff = PrincipleCurvatureRadiiCoeff<ellipsoid>;
        using quarter_arc_coeff = QuarterArcCoeff<principle_curvature_radii_coeff>;
        return MeridianArcBottomIterImpl<ellipsoid, quarter_arc_coeff>(len, iter_threshold, enable_precorrectoin);
    }
};

using EllipsoidMath::CurvatureRadiusCollection;
using EllipsoidMath::MeridianArcBottom;
using EllipsoidMath::MeridianArcLength;
using EllipsoidMath::PrincipleCurvatureRadii;

AGTB_GEODESY_END

#endif