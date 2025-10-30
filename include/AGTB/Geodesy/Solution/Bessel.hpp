#ifndef AGTB_GEODESY_SOLUTION_BESSEL_HPP
#define AGTB_GEODESY_SOLUTION_BESSEL_HPP

#pragma once

#include "../../details/Macros.hpp"
#include "../Base.hpp"

#include <gcem.hpp>

#include <tuple>
#include <numbers>
#include <concepts>

AGTB_GEODESY_BEGIN

namespace Solution::Bessel
{
    namespace Coefficients
    {
        enum class Tag
        {
            General,
            Specified
        };

        template <typename T>
        concept BasicTagConcept = requires {
            T::General;
            T::Specified;
        };

        template <typename T>
        concept ImplConcept = requires {
            { T::A_B_C(double(0.0)) } -> std::convertible_to<std::tuple<double, double, double>>;
            { T::alpha_beta(double(0.0)) } -> std::convertible_to<std::tuple<double, double>>;
        };

        template <EllipsoidConcept ellipsoid, BasicTagConcept tag, tag t>
        struct Impl
        {
            static auto A_B_C(double pow_cosA0_2)
            {
                AGTB_NOT_IMPLEMENT();
            }

            auto alpha_beta(double pow_cosA0_2)
            {
                AGTB_NOT_IMPLEMENT();
            }
        };
#pragma region Custom_New_Tag_Macros
#define __AGTB_BESSEL_COEFF_NEW_DEF_BEGIN \
    AGTB_GEODESY_BEGIN                    \
    namespace Solution::Bessel            \
    {                                     \
                                          \
        namespace Coefficients            \
        {
#define __AGTB_BESSEL_COEFF_NEW_DEF_END \
    }                                   \
    }                                   \
    AGTB_GEODESY_END

#define AGTB_DEFAULT_IMPL_CUSTOM_TAG_GENERAL(__Tag)                                           \
    __AGTB_BESSEL_COEFF_NEW_DEF_BEGIN                                                         \
    template <EllipsoidConcept ellipsoid>                                                     \
    struct Impl<ellipsoid, __Tag, __Tag::General>                                             \
    {                                                                                         \
        static auto A_B_C(double pow_cosA0_2)                                                 \
        {                                                                                     \
            return Coefficients::Impl<ellipsoid, Tag, Tag::General>::A_B_C(pow_cosA0_2);      \
        }                                                                                     \
                                                                                              \
        static auto alpha_beta(double pow_cosA0_2)                                            \
        {                                                                                     \
            return Coefficients::Impl<ellipsoid, Tag, Tag::General>::alpha_beta(pow_cosA0_2); \
        }                                                                                     \
    };                                                                                        \
    __AGTB_BESSEL_COEFF_NEW_DEF_END

#define AGTB_DEFAULT_IMPL_CUSTOM_TAG_ELLIPSOID_SPECIFIED(__Tag, __Ellipsoid)                      \
    __AGTB_BESSEL_COEFF_NEW_DEF_BEGIN                                                             \
    template <>                                                                                   \
    struct Impl<__Ellipsoid, __Tag, __Tag::Specified>                                             \
    {                                                                                             \
        static auto A_B_C(double pow_cosA0_2)                                                     \
        {                                                                                         \
            return Coefficients::Impl<__Ellipsoid, Tag, Tag::Specified>::A_B_C(pow_cosA0_2);      \
        }                                                                                         \
                                                                                                  \
        static auto alpha_beta(double pow_cosA0_2)                                                \
        {                                                                                         \
            return Coefficients::Impl<__Ellipsoid, Tag, Tag::Specified>::alpha_beta(pow_cosA0_2); \
        }                                                                                         \
    };                                                                                            \
    __AGTB_BESSEL_COEFF_NEW_DEF_END
#pragma endregion

        template <EllipsoidConcept ellipsoid>
        struct Impl<ellipsoid, Tag, Tag::General>
        {
            AGTB_WARNING("Not a good implement, use Tag::Specified or custom tag")
            static auto A_B_C(double pow_cosA0_2)
            {
                double
                    k2 = ellipsoid::e2_2 * pow_cosA0_2,
                    k4 = gcem::pow(k2, 2),
                    k6 = gcem::pow(k2, 3),
                    b = ellipsoid::b,
                    A = b * (1 +
                             k2 / 4 -
                             k4 * 3 / 64 +
                             k6 * 5 / 256),
                    B = b * (k2 / 8 -
                             k4 / 32 +
                             k6 * 15 / 1024),
                    C = b * (k4 / 128 -
                             k6 * 3 / 512);
                return std::make_tuple(A, B, C);
            }

            AGTB_WARNING("Not a good implement, use Tag::Specified or custom tag")
            static auto alpha_beta(double pow_cosA0_2)
            {
                double
                    e2 = ellipsoid::e1_2,
                    e4 = gcem::pow(e2, 2),
                    e6 = gcem::pow(e2, 3),
                    k2 = pow_cosA0_2,
                    k4 = gcem::pow(k2, 2),
                    alpha = (e2 / 2 + e4 / 8 + e6 / 16) -
                            (e4 / 16 + e6 / 16) * k2 +
                            (e6 * 3 / 128) * k4,
                    beta = (e4 / 32 + e6 / 32) * k2 -
                           (e6 / 64) * k4;
                return std::make_tuple(alpha, beta);
            }
        };

        template <>
        struct Impl<Ellipsoid::Krasovski, Tag, Tag::Specified>
        {
            static auto A_B_C(double pow_cosA0_2)
            {
                double
                    A = 6'356'863.020 + (10'708.949 - 13.474 * pow_cosA0_2) * pow_cosA0_2,
                    B = (5'354.469 - 8.978 * pow_cosA0_2) * pow_cosA0_2,
                    C = (2.238 * pow_cosA0_2) * pow_cosA0_2 + 0.006;
                return std::make_tuple(A, B, C);
            }

            static auto alpha_beta(double pow_cosA0_2)
            {
                double
                    alpha = (33'523'299.0 - (28'189.0 - 70.0 * pow_cosA0_2) * pow_cosA0_2) * gcem::pow(10.0, -10),
                    beta = (0.2907 - 0.001'0 * pow_cosA0_2) * pow_cosA0_2;
                return std::make_tuple(alpha, beta);
            }
        };

        template <>
        struct Impl<Ellipsoid::IE1975, Tag, Tag::Specified>
        {
            static auto A_B_C(double pow_cosA0_2)
            {
                double
                    A = 6'356'755.288 + (10'710.341 - 13.534 * pow_cosA0_2) * pow_cosA0_2,
                    B = (5'355.171 - 9.023 * pow_cosA0_2) * pow_cosA0_2,
                    C = (2.256 * pow_cosA0_2) * pow_cosA0_2 + 0.006;
                return std::make_tuple(A, B, C);
            }

            static auto alpha_beta(double pow_cosA0_2)
            {
                double
                    alpha = (33'528'130.0 - (28'190.0 - 70.0 * pow_cosA0_2) * pow_cosA0_2) * gcem::pow(10.0, -10),
                    beta = (14'095.0 - 46.7 * pow_cosA0_2) * pow_cosA0_2 * gcem::pow(10.0, -10);
                return std::make_tuple(alpha, beta);
            }
        };
    }

    template <EllipsoidConcept _e, Coefficients::BasicTagConcept _tag, _tag _t>
    struct BesselParams
    {
        static constexpr _tag Tag = _t;
        using Ellipsoid = _e;
        using TagType = _tag;
    };
}

AGTB_GEODESY_END

#endif