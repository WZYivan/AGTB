#ifndef __AGTB_GEODESY_SOLUTION_GAUSS_HPP__
#define __AGTB_GEODESY_SOLUTION_GAUSS_HPP__

#include "../Datum.hpp"

AGTB_GEODESY_BEGIN

namespace Solution::Gauss
{
    struct InverseResult
    {
        Angle a_forwards, a_backwards;
        double s;
    };

    template <Ellipsoids __ellipsoid>
    class InverseCoeffSolver
    {
    public:
        constexpr static Ellipsoids ellipsoid = __ellipsoid;

        ~InverseCoeffSolver() = default;
        InverseCoeffSolver(Latitude<Units::Radian> Bm)
        {
            using ellipsoid_geometry = EllipsoidGeometry<ellipsoid>;
            LatitudeConstants<ellipsoid_geometry> lat_const(Bm);
            double
                V = lat_const.V,
                t = lat_const.t,
                n2 = lat_const.nu_2,
                cosB = Bm.Cos(),
                ps = rho<Units::Second>;
            CurvatureRadiusCollection crc = PrincipleCurvatureRadii<ellipsoid>(Bm);
            double N = crc.N;

            double
                p2 = gcem::pow(ps, 2),
                p3 = gcem::pow(ps, 3),
                V2 = gcem::pow(V, 2),
                V4 = gcem::pow(V, 4),
                V6 = gcem::pow(V, 6),
                n4 = gcem::pow(n2, 2),
                t2 = gcem::pow(t, 2),
                cosB2 = gcem::pow(cosB, 2),
                cosB3 = gcem::pow(cosB, 3);

            r01 = N / ps * cosB;
            r21 = (N * cosB) / (24 * p3 * V4) * (1 + n2 - 9 * n2 * t2 + n4);
            r03 = -N / (24 * p3) * (cosB3 * t2);
            S10 = N / (ps * V2);
            S12 = N / (24 * p3 * V2) * cosB2 * (2 + 3 * t2 + 2 * n2);
            S30 = N / (8 * p3 * V6) * (n2 - t2 * n2);
            t01 = t * cosB;
            t21 = 1.0 / (24 * p2 * V4) * cosB * t * (2 + 7 * n2 + 9 * t2 * n2 + 5 * n4);
            t03 = 1.0 / (24 * p2) * cosB3 * t * (2 + t2 + 2 * n2);
        }

        double r01, r21, r03, S10, S12, S30, t01, t21, t03;

        inline double U(double dLs, double dBs) const noexcept
        {
            return r01 * dLs + r21 * gcem::pow(dBs, 2) * dLs + r03 * gcem::pow(dLs, 3);
        }

        inline double V(double dLs, double dBs) const noexcept
        {
            return S10 * dBs + S12 * dBs * gcem::pow(dLs, 2) + S30 * gcem::pow(dBs, 3);
        }

        inline double DeltaAs(double dLs, double dBs) const noexcept
        {
            return t01 * dLs + t21 * gcem::pow(dBs, 2) * dLs + t03 * gcem::pow(dLs, 3);
        }
    };

    double ComputeAmRadFromT(double T, double dBs, double dLs)
    {
        if (dBs > 0 && dLs >= 0)
        {
            return T;
        }
        else if (dBs < 0 && dLs >= 0)
        {
            return std::numbers::pi - T;
        }
        else if (dBs <= 0 && dLs < 0)
        {
            return std::numbers::pi + T;
        }
        else if (dBs > 0 && dLs < 0)
        {
            return 2.0 * std::numbers::pi - T;
        }
        else if (dBs == 0 && dLs >= 0)
        {
            return std::numbers::pi / 2.0;
        }
        else
        {
            return std::numeric_limits<double>::signaling_NaN();
        }
    }

    template <Ellipsoids __ellipsoid>
    InverseResult InverseSolve(Longitude<Units::Radian> L1, Latitude<Units::Radian> B1, Longitude<Units::Radian> L2, Latitude<Units::Radian> B2)
    {
        double
            dLs = ToSeconds(L2.Rad() - L1.Rad()),
            dBs = ToSeconds(B2.Rad() - B1.Rad());
        Latitude<Units::Radian> Bm((B1.Rad() + B2.Rad()) / 2.0);

        InverseCoeffSolver<__ellipsoid> coeff_solver(Bm);

        double
            U = coeff_solver.U(dLs, dBs),
            V = coeff_solver.V(dLs, dBs),
            dAs = coeff_solver.DeltaAs(dLs, dBs),
            tanA = U / V;

        double
            c = gcem::abs(V / U),
            T = (gcem::abs(dBs) >= gcem::abs(dLs)) ? gcem::atan(U / V) : std::numbers::pi / 4.0 + gcem::atan((1.0 - c) / (1.0 + c)),
            Am_rad = ComputeAmRadFromT(T, dBs, dLs),
            Am = Am_rad * rad2sec;

        double
            S = (U / gcem::sin(Am_rad) + V / gcem::cos(Am_rad)) / 2.0,
            A12 = Am - dAs / 2.0,
            d180s = 180 * 3600,
            A21 = Am + dAs / 2.0 + (A12 < d180s ? 1 : -1) * d180s;
        return InverseResult{
            .a_forwards = Angle(A12),
            .a_backwards = Angle(A21),
            .s{S}};
    }

    struct ForwardResult
    {
        Longitude<> L;
        Latitude<> B;
        Angle a_backward;
    };

    template <Ellipsoids __ellipsoid>
    auto InitForwardSolveIteration(Longitude<> L, Latitude<> B, double S, Angle a_backward, double &b, double &l, double &a)
    {
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        double p = rho<Units::Second>;
        CurvatureRadiusCollection crc = PrincipleCurvatureRadii<__ellipsoid>(B);
        double M = crc.M, N = crc.N;

        b = p / M * a_backward.Cos();
        l = p / N * a_backward.Sin() * (1.0 / B.Cos());
        a = l * B.Sin();
    }

    template <Ellipsoids __ellipsoid>
    ForwardResult ForwardSolve(Longitude<> L, Latitude<> B, double S, Angle a_forward, double epsilon = 1e-5)
    {
        double dB0, dL0, dA0;
        InitForwardSolveIteration<__ellipsoid>(L, B, S, a_forward, dB0, dL0, dA0);
        double
            Bm = B.Rad() * rad2sec + dB0 / 2.0,
            Am = a_forward.Rad() * rad2sec + dA0 / 2.0;

        double dB{}, dL{}, dA{}, dBp{}, dLp{}, dAp{};

        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;

        do
        {
            dBp = dB;
            dLp = dL;
            dAp = dA;

            LatitudeConstants<ellipsoid_geometry> lc(Bm * sec2rad);
            CurvatureRadiusCollection crc = PrincipleCurvatureRadii<__ellipsoid>(Bm * sec2rad);
            double
                N = crc.N,
                N2 = gcem::pow(N, 2),
                V = lc.V,
                V2 = gcem::pow(V, 2),
                S2 = gcem::pow(S, 2),
                t = lc.t,
                t2 = gcem::pow(t, 2),
                n2 = lc.nu_2,
                n4 = gcem::pow(n2, 2),
                p = rho<Units::Second>,
                cosA = gcem::cos(Am * sec2rad),
                cosA2 = gcem::pow(cosA, 2),
                sinA = gcem::sin(Am * sec2rad),
                sinA2 = gcem::pow(sinA, 2),
                secB = 1.0 / gcem::cos(Bm * sec2rad);

            dB = V2 / N * p * S * cosA * (1 + S2 / (24 * N2) * (sinA2 * (2 + 3 * t2 + 3 * n2 * t2) + 3 * n2 * cosA2 * (-1 + t2 - n2 - 4 * t2 * n2)));
            dL = p / N * S * secB * sinA * (1 + S2 / (24 * N2) * (sinA2 * t2 - cosA2 * (1 + n2 - 9 * t2 * n2 + n4)));
            dA = p / N * S * sinA * t * (1 + S2 / (24 * N2) * (cosA2 * (2 + 7 * n2 + 9 * t2 * n2 + 5 * n4) + sinA2 * (2 + t2 + 2 * n2)));

            Bm = B.Rad() * rad2sec + dB / 2.0;
            Am = a_forward.Rad() * rad2sec + dA / 2.0;
        } while (
            !((gcem::abs(dB - dBp) < epsilon) &&
              (gcem::abs(dL - dLp) < epsilon) &&
              (gcem::abs(dA - dAp) < epsilon)));

        Longitude<> L_tar(L.Rad() + dL * sec2rad);
        Latitude<> B_tar(B.Rad() + dB * sec2rad);
        double d180s = 180 * 3600, d180r = 180 * deg2rad;
        Angle a_backward(a_forward.Rad() * rad2sec + dA + (a_forward.Rad() < d180r ? 1 : -1) * d180s);
        return {
            L_tar, B_tar, a_backward};
    }
}

AGTB_GEODESY_END

#endif