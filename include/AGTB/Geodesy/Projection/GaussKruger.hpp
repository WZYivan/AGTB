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
    Longitude CenterLongitude(int zone)
    {
        using Utils::Angles::FromDMS;
        if constexpr (zi == ZoneInterval::D6)
        {
            return FromDMS(6 * zone - 3);
        }
        else if constexpr (zi == ZoneInterval::D3)
        {
            return FromDMS(3 * zone);
        }
        else
        {
            AGTB_NOT_IMPLEMENT();
        }
    }

    template <ZoneInterval zi>
    int Zone(Longitude l)
    {
        using Utils::Angles::FromDMS;
        if constexpr (zi == ZoneInterval::D6)
        {
            return int(l / FromDMS(6.0)) + 1;
        }
        else if constexpr (zi == ZoneInterval::D3)
        {
            return int(l / FromDMS(3.0)) + gcem::fmod(l.Value(), FromDMS(3)) > FromDMS(1.5) ? 1 : 0;
        }
        else
        {
            AGTB_NOT_IMPLEMENT();
        }
    }

    template <ZoneInterval zi>
    Longitude CenterLongitude(Longitude l)
    {
        return CenterLongitude<zi>(Zone<zi>(l));
    }

    struct ForwardResult
    {
        double x, y;
        int zone;

        double ZoneY()
        {
            return y + 500'000.0 + zone * 100'000'0;
        }
    };

    struct InverseResult
    {
        Latitude B;
        Longitude L;
    };

    namespace Project
    {
        template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
        struct Impl
        {
            template <ZoneInterval zi = ZoneInterval::D6>
            static ForwardResult Forward(Longitude L, Latitude B)
            {
                AGTB_NOT_IMPLEMENT();
            }
        };

        template <EllipsoidConcept ellipsoid>
        struct Impl<ellipsoid, EllipsoidBasedOption::General>
        {
            static constexpr EllipsoidBasedOption Option = EllipsoidBasedOption::General;

            template <ZoneInterval zi = ZoneInterval::D6>
            static ForwardResult Forward(Longitude L, Latitude B)
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
                double sinB = B.Sin(),
                       cosB = B.Cos(),
                       cosBp3 = gcem::pow(cosB, 3),
                       cosBp5 = gcem::pow(cosB, 5);
                int zone = Zone<zi>(L);
                double l_c = CenterLongitude<zi>(zone).Value() /*rad*/,
                       l_c_s = ToSeconds(l_c) /*seconds below*/,
                       l_s = ToSeconds(L),
                       dl_s = l_s - l_c_s,
                       l = dl_s,
                       l2 = gcem::pow(dl_s, 2),
                       l3 = gcem::pow(dl_s, 3),
                       l4 = gcem::pow(dl_s, 4),
                       l5 = gcem::pow(dl_s, 5),
                       l6 = gcem::pow(dl_s, 6);
                auto [_, N] = PrincipleCurvatureRadii<ellipsoid, Option>(B);
                double X = MeridianArcSolver<ellipsoid, Option>::Forward(B);

                double x = X +
                           N / (2 * p2) * sinB * cosB * l2 +
                           N / (24 * p4) * sinB * cosBp3 * (5 - t2 + 9 * n2 + 4 * n4) * l4 +
                           N / (720 * p6) * sinB * cosBp5 * (61 - 58 * t2 + t4) * l6;
                double y = N / p * cosB * l +
                           N / (6 * p3) * cosBp3 * (1 - t2 + n2) * l3 +
                           N / (120 * p5) * cosBp5 * (5 - 18 * t2 + t4 + 14 * n2 - 58 * n2 * t2) * l5;
                return {.x = x, .y = y, .zone = zone};
            }

            template <ZoneInterval zi = ZoneInterval::D6>
            static InverseResult Inverse(double x, double y, int zone)
            {
                Latitude Bf = MeridianArcSolver<ellipsoid, Option>::Inverse(x, 1e-15);

                auto [Mf, Nf] = PrincipleCurvatureRadii<ellipsoid, Option>(Bf);
                GeodeticLatitudeConstants<ellipsoid> glc(Bf);
                double tf = glc.t, nf2 = glc.nu_2;
                double tf2 = gcem::pow(tf, 2),
                       tf4 = gcem::pow(tf, 4),
                       nf4 = gcem::pow(nf2, 2);
                double Nf3 = gcem::pow(Nf, 3),
                       Nf5 = gcem::pow(Nf, 5);
                double cosBf = Bf.Cos();
                double y2 = gcem::pow(y, 2),
                       y3 = gcem::pow(y, 3),
                       y4 = gcem::pow(y, 4),
                       y5 = gcem::pow(y, 5),
                       y6 = gcem::pow(y, 6);

                using Utils::Angles::Angle;

                double B = Bf +
                           tf / (2.0 * Mf * Nf) * y2 +
                           tf / (24 * Mf * Nf3) * (5 + 3 * tf2 + nf2 - 9 * nf2 * tf2) * y4 -
                           tf / (720 * Mf * Nf5) * (61 + 90 * tf2 + 45 * tf4) * y6;
                double dl = 1.0 / (Nf * cosBf) * y -
                            1.0 / (6.0 * Nf3 * cosBf) * (1 + 2 * tf2 + nf2) * y3 +
                            1.0 / (120 * Nf5 * cosBf) * (5 + 28 * tf2 + 24 * tf4 + 6 * nf2 + 8 * nf2 * tf2) * y5;
                Longitude Lc = CenterLongitude<zi>(zone);
                return {
                    .B = B,
                    .L = Lc.Value() + dl};
            }
        };
    }

    template <EllipsoidConcept ellipsoid, EllipsoidBasedOption opt>
    using Projector = Project::Impl<ellipsoid, opt>;
}

AGTB_GEODESY_END

#endif