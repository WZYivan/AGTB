#ifndef __AGTB_PHOTOGRAMMETRY_SPACE_MATH_COLLINEARITY_EQUATION_HPP__
#define __AGTB_PHOTOGRAMMETRY_SPACE_MATH_COLLINEARITY_EQUATION_HPP__

#include "../Base.hpp"
#include "gcem.hpp"
#include "../../IO/Eigen.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

enum class CollinearityEquationStyle
{
    Linearization,
    DLT
};

namespace detail::CollinearityEquation
{
    template <CollinearityEquationStyle __style>
    struct Param
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    };

    template <>
    struct Param<CollinearityEquationStyle::Linearization>
    {
        double x, y, f, H, kappa, omega, z;
        Matrix rotate;
    };

    template <CollinearityEquationStyle __style>
    struct Coefficient
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    };

    template <>
    struct Coefficient<CollinearityEquationStyle::Linearization>
    {
        double a11, a12, a13, a14, a15, a16,
            a21, a22, a23, a24, a25, a26;

        Matrix ToMatrix26() const noexcept
        {
            Matrix mat(2, 6);
            mat << a11, a12, a13, a14, a15, a16,
                a21, a22, a23, a24, a25, a26;
            return mat;
        }

        Matrix ToMatrix29() const noexcept
        {
            Matrix mat(2, 9);
            mat << a11, a12, a13, a14, a15, a16, -a11, -a12, -a13,
                a21, a22, a23, a24, a25, a26, -a21, -a22, -a23;
            return mat;
        }
    };
}

template <CollinearityEquationStyle __style>
struct CollinearityEquation
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

template <>
struct CollinearityEquation<CollinearityEquationStyle::Linearization>
{
    constexpr static CollinearityEquationStyle style = CollinearityEquationStyle::Linearization;
    enum class Simplify
    {
        None,
        Kappa,
        All
    };

    using Coefficient = detail::CollinearityEquation::Coefficient<style>;
    using Param = detail::CollinearityEquation::Param<style>;

    template <Simplify __simplify>
    static Coefficient Solve(const Param &param)
    {
        if constexpr (__simplify == Simplify::None)
        {
            return Solve_None(param);
        }
        else if constexpr (__simplify == Simplify::Kappa)
        {
            return Solve_Kappa(param);
        }
        else if constexpr (__simplify == Simplify::All)
        {
            return Solve_All(param);
        }
        else
        {
            AGTB_TEMPLATE_NOT_SPECIALIZED();
        }
    }

private:
    static Coefficient Solve_None(const Param &param)
    {
        double
            x = param.x,
            y = param.y, z = param.z,
            f = param.f, H = param.H,
            k = param.kappa, w = param.omega;
        const Matrix &rotate = param.rotate;

        double
            cosk = gcem::cos(k),
            sink = gcem::sin(k),
            cosw = gcem::cos(w),
            sinw = gcem::sin(w);
        const auto &a = rotate.row(0),
                   &b = rotate.row(1),
                   &c = rotate.row(2);

        Coefficient coeff{
            .a11 = 1 / z * (a(0) * f + a(2) * x),
            .a12 = 1 / z * (b(0) * f + b(2) * x),
            .a13 = 1 / z * (c(0) * f + c(2) * x),
            .a14 = y * sinw - (x / f * (x * cosk - y * sink) + f * cosk) * cosw,
            .a15 = -f * sink - x / f * (x * sink + y * cosk),
            .a16 = y,
            .a21 = 1 / z * (a(1) * f + a(2) * y),
            .a22 = 1 / z * (b(1) * f + b(2) * y),
            .a23 = 1 / z * (c(1) * f + c(2) * y),
            .a24 = -x * sinw - (y / f * (x * cosk - y * sink) - f * sink) * cosw,
            .a25 = -f * cosk - y / f * (x * sink + y * cosk),
            .a26 = -x};
        return coeff;
    }

    static Coefficient Solve_Kappa(const Param &param)
    {
        double
            x = param.x,
            y = param.y,
            f = param.f, H = param.H,
            k = param.kappa;

        double
            xx = f + pow(x, 2) / f,
            yy = f + pow(y, 2) / f,
            xy = x * y / f,
            cosk = gcem::cos(k),
            sink = gcem::sin(k);

        Coefficient coeff{
            .a11 = -f / H * cosk,
            .a12 = -f / H * sink,
            .a13 = -x / H,
            .a14 = -xx * cosk + xy * sink,
            .a15 = -xy * cosk - xx * sink,
            .a16 = y,
            .a21 = f / H * sink,
            .a22 = -f / H * cosk,
            .a23 = -y / H,
            .a24 = -xy * cosk + yy * sink,
            .a25 = -yy * cosk - xy * sink,
            .a26 = -x};
        return coeff;
    }

    static Coefficient Solve_All(const Param &param)
    {
        double
            x = param.x,
            y = param.y,
            f = param.f, H = param.H;

        double
            xx = f + pow(x, 2) / f,
            yy = f + pow(y, 2) / f,
            xy = x * y / f;

        Coefficient coeff{
            .a11 = -f / H,
            .a12 = 0,
            .a13 = -x / H,
            .a14 = -xx,
            .a15 = -xy,
            .a16 = y,
            .a21 = 0,
            .a22 = -f / H,
            .a23 = -y / H,
            .a24 = -xy,
            .a25 = -yy,
            .a26 = -x};
        return coeff;
    }
};

AGTB_PHOTOGRAMMETRY_END

#endif