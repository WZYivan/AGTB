#ifndef __AGTB_GEODESY_SOLUTION_BESSEL_HPP__
#define __AGTB_GEODESY_SOLUTION_BESSEL_HPP__

#include "Base.hpp"

AGTB_GEODESY_BEGIN

namespace Solution::Bessel
{
    template <Ellipsoids __ellipsoid>
    class CoeffSolver
    {
    public:
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        constexpr static double e2_2 = ellipsoid_geometry::e2_2, e2 = ellipsoid_geometry::e1_2, b = ellipsoid_geometry::b;

        ~CoeffSolver() = default;
        CoeffSolver(double cosA0p2)
        {
            double
                k2 = e2_2 * cosA0p2,
                k4 = gcem::pow(k2, 2),
                k6 = gcem::pow(k2, 3),
                e4 = gcem::pow(e2, 2),
                e6 = gcem::pow(e2, 3),
                cosA0p4 = gcem::pow(cosA0p2, 2);

            A = b * (1 +
                     k2 / 4.0 -
                     3 * k4 / 64.0 +
                     5 * k6 / 256.0);
            B = b * (k2 / 8.0 -
                     k4 / 32.0 +
                     15 * k6 / 1024.0);
            C = b * (k4 / 128.0 - 3 * k6 / 512.0);
            alpha = (e2 / 2.0 + e4 / 8.0 + e6 / 16.0) -
                    (e4 / 16.0 + e6 / 16.0) * cosA0p2 +
                    (3 * e6 / 128.0) * cosA0p4;
            beta = (e4 / 32.0 + e6 / 32.0) * cosA0p2 -
                   (e6 / 64.0) * cosA0p4;
            beta_prime = 2 * beta / cosA0p2;
        }

        double A, B, C, alpha, beta, beta_prime;
    };

    double RefineLambda(double lambda, double sinA1, double tan_lambda)
    {
        double abs_lambda = gcem::abs(lambda);

        if (sinA1 > 0 && tan_lambda > 0)
        {
            return abs_lambda;
        }
        else if (sinA1 > 0 && tan_lambda < 0)
        {
            return 180 * deg2rad - abs_lambda;
        }
        else if (sinA1 < 0 && tan_lambda < 0)
        {
            return -abs_lambda;
        }
        else if (sinA1 < 0 && tan_lambda > 0)
        {
            return abs_lambda - 180 * deg2rad;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double RefineA2(double A2, double sinA1, double tanA2)
    {
        double absA2 = gcem::abs(A2);

        if (sinA1 < 0 && tanA2 > 0)
        {
            return absA2;
        }
        else if (sinA1 < 0 && tanA2 < 0)
        {
            return 180 * deg2rad - absA2;
        }
        else if (sinA1 > 0 && tanA2 > 0)
        {
            return 180 * deg2rad + absA2;
        }
        else if (sinA1 > 0 && tanA2 < 0)
        {
            return 360 * deg2rad - absA2;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    template <Ellipsoids __ellipsoid, Units __unit>
        requires Concept::EllipsoidGeometry<EllipsoidGeometry<__ellipsoid>>
    ForwardResult<__unit> ForwardSolve(Longitude<__unit> L1, Latitude<__unit> B1, double S, Angle a_forward)
    {
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        LatitudeConstants<ellipsoid_geometry> lc(B1);
        double
            W1 = lc.W,
            e2 = ellipsoid_geometry::e1_2,
            sinB1 = B1.Sin(),
            cosB1 = B1.Cos(),
            sinu1 = sinB1 * gcem::sqrt(1 - e2) / W1,
            cosu1 = cosB1 / W1,
            sinA1 = a_forward.Sin(),
            cosA1 = a_forward.Cos(),
            sinA0 = cosu1 * sinA1,
            sinA0p2 = gcem::pow(sinA0, 2),
            cosA0p2 = 1 - sinA0p2,
            cot_sigma1 = cosu1 * cosA1 / sinu1,
            cot_sigma1p2 = gcem::pow(cot_sigma1, 2),
            sin_2sigma1 = 2.0 * cot_sigma1 / (cot_sigma1p2 + 1),
            cos_2sigma1 = (cot_sigma1p2 - 1) / (cot_sigma1p2 + 1);

        CoeffSolver<__ellipsoid> coeff_solver(cosA0p2);
        double
            A = coeff_solver.A,
            B = coeff_solver.B,
            C = coeff_solver.C,
            alpha = coeff_solver.alpha,
            beta = coeff_solver.beta;

        double
            sigma0 = (S - (B + C * cos_2sigma1) * sin_2sigma1) / A,
            sin_2sigma0 = gcem::sin(2 * sigma0),
            cos_2sigma0 = gcem::cos(2 * sigma0),
            sin_2_sigma1_a_sigma0 = sin_2sigma1 * cos_2sigma0 + cos_2sigma1 * sin_2sigma0,
            cos_2_sigma1_a_sigma0 = cos_2sigma1 * cos_2sigma0 - sin_2sigma1 * sin_2sigma0,
            sigma = sigma0 + (B + 5 * C * cos_2_sigma1_a_sigma0) * sin_2_sigma1_a_sigma0 / A,
            sin_sigma = gcem::sin(sigma),
            cos_sigma = gcem::cos(sigma),
            delta = (alpha * sigma + beta * (sin_2_sigma1_a_sigma0 - sin_2sigma1)) * sinA0,
            sinu2 = sinu1 * cos_sigma + cosu1 * cosA1 * sin_sigma,
            sinu2p2 = gcem::pow(sinu2, 2),
            B2 = gcem::atan(
                sinu2 / (gcem::sqrt(1 - e2) * gcem::sqrt(1 - sinu2p2))),
            lambda = gcem::atan(
                (sinA1 * sin_sigma) / (cosu1 * cos_sigma - sinu1 * sin_sigma * cosA1)),
            tan_lambda = gcem::tan(lambda);
        lambda = RefineLambda(lambda, sinA1, tan_lambda);
        double
            L2 = L1.Rad() + lambda - delta,
            A2 = gcem::atan(
                cosu1 * sinA1 / (cosu1 * cos_sigma * cosA1 - sinu1 * sin_sigma)),
            tanA2 = gcem::tan(A2);
        A2 = RefineA2(A2, sinA1, tanA2);

        // std::println(
        //     " W1 = {}\n sinu1 = {}\n cosu1 = {}\n sinA1 = {}\n cosA1 = {}\n sinA0 = {}\n cosA0p2 = {}\n cot_sigma1 = {}\n sin_2sigma1 = {}\n cos_2sigma1 = {}\n A = {}\n B = {}\n C = {}\n alpha = {}\n beta = {}\n sigma0 = {}\n sin2 = {}\n cos2 = {}\n sigma = {}\n delta = {}\n lambda = {}\n",
        //     W1, sinu1, cosu1, sinA1, cosA1, sinA0, cosA0p2, cot_sigma1, sin_2sigma1, cos_2sigma1, A, B, C, alpha, beta, sigma0, sin_2_sigma1_a_sigma0, cos_2_sigma1_a_sigma0, sigma, delta, lambda);

        return {
            .L = Longitude<__unit>(L2),
            .B = Latitude<__unit>(B2),
            .a_backward = Angle::FromRad(A2)};
    }

    double RefineA1(double A1, double p, double q)
    {
        double absA1 = gcem::abs(A1);

        if (p > 0 && q > 0)
        {
            return absA1;
        }
        else if (p > 0 && q < 0)
        {
            return 180 * deg2rad - absA1;
        }
        else if (p < 0 && q < 0)
        {
            return 180 * deg2rad + absA1;
        }
        else if (p < 0 && q > 0)
        {
            return 360 * deg2rad - absA1;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    double RefineSigma(double sigma, double cos_sigma)
    {
        double abs_sigma = gcem::abs(sigma);

        if (cos_sigma > 0)
        {
            return abs_sigma;
        }
        else if (cos_sigma < 0)
        {
            return 180 * deg2rad - abs_sigma;
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    template <Ellipsoids __ellipsoid, Units __unit>
        requires Concept::EllipsoidGeometry<EllipsoidGeometry<__ellipsoid>>
    InverseResult InverseSolve(Longitude<__unit> L1, Latitude<__unit> B1, Longitude<__unit> L2, Latitude<__unit> B2, double epsilon = 1e-5)
    {
        using ellipsoid_geometry = EllipsoidGeometry<__ellipsoid>;
        LatitudeConstants<ellipsoid_geometry> lc1(B1), lc2(B2);

        double
            W1 = lc1.W,
            W2 = lc2.W,
            sinB1 = B1.Sin(),
            cosB1 = B1.Cos(),
            sinB2 = B2.Sin(),
            cosB2 = B2.Cos(),
            e2 = ellipsoid_geometry::e1_2,
            sqrt_1_s_e2 = gcem::sqrt(1 - e2),
            sinu1 = sinB1 * sqrt_1_s_e2 / W1,
            sinu2 = sinB2 * sqrt_1_s_e2 / W2,
            cosu1 = cosB1 / W1,
            cosu2 = cosB2 / W2,
            L = L2.Rad() - L1.Rad(),
            a1 = sinu1 * sinu2,
            a2 = cosu1 * cosu2,
            b1 = cosu1 * sinu2,
            b2 = sinu1 * cosu2;

        double
            delta = 0,
            lambda = L + delta,
            A1,
            sigma,
            x,
            sinA0,
            delta_p,
            lambda_p;

        // std::println(
        //     " sinu1 = {}\n cosu1 = {}\n sinu2 = {}\n cosu2 = {}\n a1 = {}\n a2 = {}\n b1 = {}\n b2 = {}\n",
        //     sinu1, cosu1, sinu2, cosu2, a1, a2, b1, b2);
        // int i = 1;

        do
        {
            delta_p = delta;
            lambda_p = lambda;
            double
                cos_lambda = gcem::cos(lambda),
                sin_lambda = gcem::sin(lambda);

            double
                p = cosu2 * sin_lambda,
                q = b1 - b2 * cos_lambda;
            A1 = gcem::atan(p / q);
            A1 = RefineA1(A1, p, q);

            double
                sinA1 = gcem::sin(A1),
                cosA1 = gcem::cos(A1),
                sin_sigma = p * sinA1 + q * cosA1,
                cos_sigma = a1 + a2 * cos_lambda;
            sigma = gcem::atan(sin_sigma / cos_sigma);
            sigma = RefineSigma(sigma, cos_sigma);

            sinA0 = cosu1 * sinA1;
            double
                sinA0p2 = gcem::pow(sinA0, 2),
                cosA0p2 = 1 - sinA0p2;
            x = 2 * a1 - cosA0p2 * cos_sigma;

            CoeffSolver<__ellipsoid> coeff_solver(cosA0p2);
            double
                alpha = coeff_solver.alpha,
                beta_prime = coeff_solver.beta_prime;

            delta = (alpha * sigma - beta_prime * x * sin_sigma) * sinA0;
            lambda = L + delta;

            // std::println(">>> {}\n sinl = {}\n p = {}\n q = {}\n A1 = {}\n sigma = {}\n sinA0 = {}\n x = {}\n alpha = {}\n delta = {}\n beta_prime = {}\n",
            //              i++, sin_lambda, p, q, Angle::FromRad(A1).ToString(), sigma, sinA0, x, alpha, delta * rad2sec, beta_prime);
        } while (
            !(
                gcem::abs(delta - delta_p) < epsilon &&
                gcem::abs(lambda - lambda_p) < epsilon));

        double
            sinA0p2 = gcem::pow(sinA0, 2),
            cosA0p2 = 1 - sinA0p2,
            cosA0p4 = gcem::pow(cosA0p2, 2),
            x2 = gcem::pow(x, 2),
            cos_sigma = gcem::cos(sigma),
            sin_sigma = gcem::sin(sigma),
            sin_lambda = gcem::sin(lambda),
            cos_lambda = gcem::cos(lambda),
            y = (cosA0p4 - 2 * x2) * cos_sigma;

        CoeffSolver<__ellipsoid> coeff_solver(cosA0p2);
        double
            A = coeff_solver.A,
            B_pp = 2 * coeff_solver.B / cosA0p2,
            C_pp = 2 * coeff_solver.C / cosA0p4,
            S = A * sigma + (B_pp * x + C_pp * y) * sin_sigma,
            A2 = gcem::atan(
                cosu1 * sin_lambda / (b1 * cos_lambda - b2));
        double d180r = 180 * deg2rad;
        A2 = (A1 < d180r ? 1 : -1) * d180r + A2;

        // std::println(
        //     " A = {}\n B_pp = {}\n C_pp = {}\n y = {}\n",
        //     A, B_pp, C_pp, y);

        return {
            .a_forwards = Angle::FromRad(A1),
            .a_backwards = Angle::FromRad(A2),
            .s = S};
    }

}

AGTB_GEODESY_END

#endif