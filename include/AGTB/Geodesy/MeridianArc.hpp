#ifndef __AGTB_GEODESY_MERIDIAN_ARC_HPP__
#define __AGTB_GEODESY_MERIDIAN_ARC_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Angles.hpp"
#include "Base.hpp"
#include "PrincipleCurvatureRadii.hpp"

#include "gcem.hpp"

#include <concepts>
#include <limits>
#include <type_traits>

AGTB_GEODESY_BEGIN

template <typename T>
concept MeridianArcLengthCoeffConcept = requires {
    AGTB_T_HAS_TYPED_MEMBER(a0, double);
    AGTB_T_HAS_TYPED_MEMBER(a2, double);
    AGTB_T_HAS_TYPED_MEMBER(a4, double);
    AGTB_T_HAS_TYPED_MEMBER(a6, double);
};

template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
struct MeridianArcLengthCoefficient
{
    using pcc = PrincipleCurvatureCoefficient<ellipsoid, opt>;

private:
    constexpr static double
        m0 = pcc::m0,
        m2 = pcc::m2,
        m4 = pcc::m4,
        m6 = pcc::m6,
        m8 = pcc::m8;

public:
    constexpr static double
        a0 = m0 +
             m2 / 2.0 +
             m4 * 3.0 / 8.0 +
             m6 * 5.0 / 16.0 +
             m8 * 35.0 / 128.0,
        a2 = m2 / 2.0 +
             m4 / 2.0 +
             m6 * 15.0 / 32.0 +
             m8 * 7.0 / 16.0,
        a4 = m4 / 8.0 +
             m6 * 3.0 / 16.0 +
             m8 * 7.0 / 32.0,
        a6 = m6 / 32.0 +
             m8 / 16.0;
};

#define AGTB_DEFINE_SPECIFIED_MeridianArcLengthCoefficient(_ellipsoid, _a0, _a2, _a4, _a6) \
    template <>                                                                            \
    struct MeridianArcLengthCoefficient<_ellipsoid, EllipsoidBasedOption::Specified>       \
    {                                                                                      \
        constexpr static double                                                            \
            a0 = _a0,                                                                      \
            a2 = _a2,                                                                      \
            a4 = _a4,                                                                      \
            a6 = _a6;                                                                      \
    }

AGTB_DEFINE_SPECIFIED_MeridianArcLengthCoefficient(
    Ellipsoid::Krasovski,
    111'134.861,
    16'036.480,
    16.828,
    0.022);

AGTB_DEFINE_SPECIFIED_MeridianArcLengthCoefficient(
    Ellipsoid::IE1975,
    111'133.005,
    16'038.528,
    16.833,
    0.022);

template <typename T>
concept MeridianArcSolveImplConcept = requires {
    { T::Forward(0.0) } -> std::convertible_to<double>;
    { T::Inverse(0.0, 0.0) } -> std::convertible_to<GeodeticLatitude>;
};

namespace MeridianArcSolve
{

    template <EllipsoidConcept _ellipsoid, EllipsoidBasedOption _opt>
    struct ImplBase
    {
        using coeff = MeridianArcLengthCoefficient<_ellipsoid, _opt>;
        using ellipsoid = _ellipsoid;
        constexpr static EllipsoidBasedOption option = _opt;

        static GeodeticLatitude Inverse(double len_m, double iter_threshold)
        {
            using Utils::Angles::FromDMS;
            using Utils::Angles::ToSeconds;
            double Bf_cur = FromDMS(len_m / coeff::a0), Bf_next = Bf_cur, FB = 0;

            if (gcem::abs(Bf_cur - 0.0) < std::numeric_limits<double>::epsilon())
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
                Bf_next = (len_m - FB) / coeff::a0;
            } while (ToSeconds(Bf_next - Bf_cur) > iter_threshold);
            return FromDMS(Bf_cur); // return rad
        }

        static GeodeticLatitude Inverse(double len_m)
        {
            AGTB_NOT_IMPLEMENT();
        }

        static double Forward(GeodeticLatitude _B)
        {
            AGTB_NOT_IMPLEMENT();
        }
    };

    template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
    struct Impl : public ImplBase<ellipsoid, opt>
    {
    };

    template <>
    struct Impl<Ellipsoid::Krasovski, EllipsoidBasedOption::Specified> : public ImplBase<Ellipsoid::Krasovski, EllipsoidBasedOption::Specified>
    {
        static double Forward(GeodeticLatitude _B)
        {
            double B = static_cast<double>(_B),
                   Bd = Utils::Angles::ToDegrees(B);
            double sinB = gcem::sin(B),
                   cosB = gcem::cos(B),
                   sinBp3 = gcem::pow(sinB, 3),
                   sinBp5 = gcem::pow(sinB, 5);
            return coeff::a0 * Bd - 32'005.780 * sinB * cosB - 133.929 * sinBp3 * cosB - 0.697 * sinBp5 * cosB;
        }
    };

    template <>
    struct Impl<Ellipsoid::IE1975, EllipsoidBasedOption::Specified> : public ImplBase<Ellipsoid::IE1975, EllipsoidBasedOption::Specified>
    {
        static double Forward(GeodeticLatitude _B)
        {
            double B = static_cast<double>(_B),
                   Bd = Utils::Angles::ToDegrees(B);
            double sinB = gcem::sin(B),
                   cosB = gcem::cos(B),
                   sinBp3 = gcem::pow(sinB, 3),
                   sinBp5 = gcem::pow(sinB, 5);
            return coeff::a0 * Bd - 32'009.858 * sinB * cosB - 133.960 * sinBp3 * cosB - 0.698 * sinBp5 * cosB;
        }
    };
};

template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
    requires MeridianArcSolveImplConcept<MeridianArcSolve::Impl<ellipsoid, opt>>
using MeridianArcSolver = MeridianArcSolve::Impl<ellipsoid, opt>;

AGTB_GEODESY_END

#endif