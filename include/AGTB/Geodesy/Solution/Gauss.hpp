#ifndef __AGTB_GEODESY_SOLUTION_GAUSS_HPP__
#define __AGTB_GEODESY_SOLUTION_GAUSS_HPP__

#pragma once

#include "../../details/Macros.hpp"
#include "../../Utils/Angles.hpp"
#include "../Base.hpp"
#include "../PrincipleCurvatureRadii.hpp"

#include <gcem.hpp>

#include <tuple>
#include <numbers>
#include <concepts>

AGTB_GEODESY_BEGIN

namespace Solution::Gauss
{
    struct InverseParams
    {
        GeodeticLatitude B1, B2;
        GeodeticLongitude L1, L2;
    };

    struct InverseResult
    {
        double S, A12_rad, A21_rad;
    };

    template <EllipsoidConcept ellipsoid>
    auto Inverse(InverseParams params)
    {
        using Utils::Angles::ToSeconds;
        double
            e2_2 = ellipsoid::e2_2,
            c = ellipsoid::c,
            B1 = params.B1,
            B2 = params.B2,
            L1 = params.L1,
            L2 = params.L2,
            p2 = GeodeticConstants::rho_second,
            Bm = (B1 + B2) / 2,
            dL2 = ToSeconds(L2 - L1),
            dB2 = ToSeconds(B2 - B1);
        auto glc = GeodeticLatitudeConstants<ellipsoid>(Bm);
        double
            Vm = glc.V,
            tm = glc.t,
            num_2 = glc.nu_2;
        auto [_, N] = PrincipleCurvatureRadii<ellipsoid>(Bm);
    }
}

AGTB_GEODESY_END

#endif