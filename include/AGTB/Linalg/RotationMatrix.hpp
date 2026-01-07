#ifndef __AGTB_LINALG_ROTATION_MATRIX_HPP__
#define __AGTB_LINALG_ROTATION_MATRIX_HPP__

#include "Base.hpp"

#include <cmath>

AGTB_LINALG_BEGIN

/**
 * @brief Rotate in axis X
 *
 * @param omega
 * @return Matrix
 */
Matrix RotateX(double omega)
{
    const double
        sinw = sin(omega),
        cosw = cos(omega);
    Matrix Rw(3, 3);
    Rw << 1, 0, 0,
        0, cosw, -sinw,
        0, sinw, cosw;
    return std::move(Rw);
}

/**
 * @brief Rotate in axis Y
 *
 * @param phi
 * @return Matrix
 */
Matrix RotateY(double phi)
{
    const double
        sinp = sin(phi),
        cosp = cos(phi);
    Matrix Rp(3, 3);
    Rp << cosp, 0, -sinp,
        0, 1, 0,
        sinp, 0, cosp;
    return std::move(Rp);
}

/**
 * @brief Rotate in axis Z
 *
 * @param kappa
 * @return Matrix
 */
Matrix RotateZ(double kappa)
{
    const double
        sink = sin(kappa),
        cosk = cos(kappa);
    Matrix Rk(3, 3);
    Rk << cosk, -sink, 0,
        sink, cosk, 0,
        0, 0, 1;
    return std::move(Rk);
}

/**
 * @brief Uniform interface of Rotate<X | Y | Z>
 *
 * @tparam ax
 * @param angle
 * @return Matrix
 */
template <Axis ax>
Matrix RotateByAxis(double angle)
{
    if constexpr (ax == Axis::X)
    {
        return RotateX(angle);
    }
    else if constexpr (ax == Axis::Y)
    {
        return RotateY(angle);
    }
    else if constexpr (ax == Axis::Z)
    {
        return RotateZ(angle);
    }
    else
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    }
}

// Matrix RotationMatrix(double phi, double omega, double kappa)
// {
//     double
//         cosp = std::cos(phi),
//         sinp = std::sin(phi),
//         cosw = std::cos(omega),
//         sinw = std::sin(omega),
//         cosk = std::cos(kappa),
//         sink = std::sin(kappa);
//     double
//         a1 = cosp * cosk - cosp * sinw * sink,
//         a2 = -cosp * sink - sinp * sinw * sink,
//         a3 = -sinp * cosw,
//         b1 = cosw * sink,
//         b2 = cosw * cosk,
//         b3 = -sinw,
//         c1 = sinp * cosk + cosp * sinw * sink,
//         c2 = -sinw * cosk + cosp * sinw * sink,
//         c3 = cosp * cosw;
//     Matrix rm(3, 3);
//     rm << a1, a2, a3,
//         b1, b2, b3,
//         c1, c2, c3;
//     return rm;
// }

AGTB_LINALG_END

#endif