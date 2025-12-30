#ifndef __AGTB_PHOTOGRAMMETRY_SPACE_MATH_COLLINEARITY_EQUATION_HPP__
#define __AGTB_PHOTOGRAMMETRY_SPACE_MATH_COLLINEARITY_EQUATION_HPP__

#include "../Base.hpp"
#include "gcem.hpp"
#include "../../IO/Eigen.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace detail::CollinearityEquation
{
    /**
     * @brief Represent simplification method
     *
     */
    enum class CollinearityEquationCoeffOption : size_t
    {
        NoAngles,
        KappaOnly,
        FullAngles
    };

    /**
     * @brief 12 coefficients of collinearity equation
     *
     */
    struct CollinearityEquationCoefficient
    {
        double a11, a12, a13, a14, a15, a16,
            a21, a22, a23, a24, a25, a26;
    };

    /**
     * @brief Parameters needed to calculate its coefficients
     *
     */
    struct CollinearityEquationCoeffParam
    {
        double x, y, f, H, kappa, omega, z;
        Matrix rotate;
    };

    /**
     * @brief Calculate coefficients of collinearity equation
     *
     * @tparam __equation_opt CollinearityEquationCoeffOption
     * @return CollinearityEquationCoefficient
     */
    template <CollinearityEquationCoeffOption __equation_opt>
    CollinearityEquationCoefficient CalculateCoeff(const CollinearityEquationCoeffParam &)
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    }

    template <>
    CollinearityEquationCoefficient CalculateCoeff<CollinearityEquationCoeffOption::FullAngles>(const CollinearityEquationCoeffParam &param)
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

        CollinearityEquationCoefficient coeff{
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

    template <>
    CollinearityEquationCoefficient CalculateCoeff<CollinearityEquationCoeffOption::KappaOnly>(const CollinearityEquationCoeffParam &param)
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

        CollinearityEquationCoefficient coeff{
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

    template <>
    CollinearityEquationCoefficient CalculateCoeff<CollinearityEquationCoeffOption::NoAngles>(const CollinearityEquationCoeffParam &param)
    {
        double
            x = param.x,
            y = param.y,
            f = param.f, H = param.H;

        double
            xx = f + pow(x, 2) / f,
            yy = f + pow(y, 2) / f,
            xy = x * y / f;

        CollinearityEquationCoefficient coeff{
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
}

using detail::CollinearityEquation::CalculateCoeff;
using detail::CollinearityEquation::CollinearityEquationCoefficient;
using detail::CollinearityEquation::CollinearityEquationCoeffOption;
using detail::CollinearityEquation::CollinearityEquationCoeffParam;

AGTB_PHOTOGRAMMETRY_END

#endif