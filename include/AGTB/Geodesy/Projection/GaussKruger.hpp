#ifndef __AGTB_GEODESY_PROJECTION_GAUSS_KRUGER_HPP__
#define __AGTB_GEODESY_PROJECTION_GAUSS_KRUGER_HPP__

#include "../Base.hpp"
#include "../PrincipleCurvatureRadii.hpp"
#include "../MeridianArc.hpp"
#include "../../Utils/Angles.hpp"
#include "../../Utils/Concept.hpp"

AGTB_GEODESY_BEGIN

namespace Projection::GaussKruger
{
    enum class ZoneInterval : size_t
    {
        D3,
        D6
    };

    template <ZoneInterval zi>
    GeodeticLongitude CenterLongitude(int zone)
    {
        if constexpr (zi == ZoneInterval::D6)
        {
            return Utils::Angles::FromDMS(6 * zone - 3);
        }
        else if constexpr (zi == ZoneInterval::D3)
        {
            return Utils::Angles::FromDMS(3 * zone);
        }
        else
        {
            AGTB_NOT_IMPLEMENT();
        }
    }

    template <ZoneInterval zi>
    int Zone(GeodeticLongitude l)
    {
        if constexpr (zi == ZoneInterval::D6)
        {
            return int(l / 6.0) + 1;
        }
        else if constexpr (zi == ZoneInterval::D3)
        {
            return int(l / 3.0) + gcem::fmod(Utils::Angles::ToDegrees(l), 3) > 1.5 ? 1 : 0;
        }
        else
        {
            AGTB_NOT_IMPLEMENT();
        }
    }

    template <ZoneInterval zi>
    GeodeticLongitude CenterLongitude(GeodeticLongitude l)
    {
        return CenterLongitude<zi>(Zone<zi>(l));
    }

    struct InverseResult
    {
        double x, y;
    };

    namespace InverseSolve
    {
        template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
        struct Impl
        {
            template <ZoneInterval zi = ZoneInterval::D6>
            static InverseResult Invoke(GeodeticLongitude L, GeodeticLatitude B)
            {
                AGTB_NOT_IMPLEMENT();
            }
        };

        template <EllipsoidConcept ellipsoid>
        struct Impl<ellipsoid, EllipsoidBasedOption::General>
        {
            template <ZoneInterval zi = ZoneInterval::D6>
            static InverseResult Invoke(GeodeticLongitude L, GeodeticLatitude B)
            {
                using Utils::Angles::ToSeconds;

                GeodeticLatitudeConstants<ellipsoid> glc(B);
                double t = glc.t,
                       t2 = gcem::pow(t, 2),
                       t4 = gcem::pow(t, 4),
                       n2 = glc.nu_2,
                       n4 = gcem::pow(n2, 2);
                double p = GeodeticConstants::rho_second,
                       p2 = gcem::pow(p, 2),
                       p3 = gcem::pow(p, 3),
                       p4 = gcem::pow(p, 4),
                       p5 = gcem::pow(p, 5),
                       p6 = gcem::pow(p, 6);
                double sinB = gcem::sin(B.Value()),
                       cosB = gcem::cos(B.Value()),
                       cosBp3 = gcem::pow(cosB, 3),
                       cosBp5 = gcem::pow(cosB, 5);
                double l_c = CenterLongitude<zi>(L),
                       l_c_s = ToSeconds(l_c),
                       l_s = ToSeconds(L),
                       dl_s = l_s - l_c_s,
                       l = dl_s,
                       l2 = gcem::pow(dl_s, 2),
                       l3 = gcem::pow(dl_s, 3),
                       l4 = gcem::pow(dl_s, 4),
                       l5 = gcem::pow(dl_s, 5),
                       l6 = gcem::pow(dl_s, 6);
                auto [M, N] = PrincipleCurvatureRadiiSolver<ellipsoid, EllipsoidBasedOption::General>::Invoke(B);
                double X = MeridianArcSolver<ellipsoid, EllipsoidBasedOption::Specified>::Forward(B); // TODO: use General

                double x = X +
                           N / (2 * p2) * sinB * cosB * l2 +
                           N / (24 * p4) * sinB * cosBp3 * (5 - t2 + 9 * n2 + 4 * n4) * l4 +
                           N / (720 * p6) * sinB * cosBp5 * (61 - 58 * t2 + t4) * l6;
                double y = N / p * cosB * l +
                           N / (6 * p3) * cosBp3 * (1 - t2 + n2) * l3 +
                           N / (120 * p5) * cosBp5 * (5 - 18 * t2 + t4 + 14 * n2 - 58 * n2 * t2) * l5;
                return {.x = x, .y = y};
            }
        };
    }

    template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
        requires Utils::InvokerConcept<InverseSolve::Impl<ellipsoid, opt>, InverseResult, GeodeticLongitude, GeodeticLatitude>
    using InverseSolver = InverseSolve::Impl<ellipsoid, opt>;
}

AGTB_GEODESY_END

#endif