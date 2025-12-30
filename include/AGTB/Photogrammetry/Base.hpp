#ifndef __AGTB_PHOTOGRAPHIC_BASE_HPP__
#define __AGTB_PHOTOGRAPHIC_BASE_HPP__

#include "../details/Macros.hpp"

#include "../Linalg/RotationMatrix.hpp"
#include "../Linalg/CoordinateSystemTranform.hpp"

#include <Eigen/Dense>

AGTB_PHOTOGRAMMETRY_BEGIN

using Linalg::Matrix;

/**
 * @brief Represent status of iterative solution in photogrammetry
 *
 */
enum class IterativeSolutionInfo : size_t
{
    Success,
    NotConverged,
    Failed,
    Unknown
};

struct InteriorOrientationElements
{
    double x0{0.0}, y0{0.0}, f{0.0}, m{0};

    std::string ToString() const noexcept
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb,
                       "f : {}\nm : {}\nx0 : {}\ny0 : {}\n",
                       f, m, x0, y0);
        return sb;
    }
};

struct ExteriorOrientationElements
{
    double Xs, Ys, Zs, Phi, Omega, Kappa;

    std::string ToString() const noexcept
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb,
                       "Xs : {}\nYs : {}\nZs : {}\nPhi : {}\nOmega : {}\nKappa : {}\n",
                       Xs, Ys, Zs, Phi, Omega, Kappa);
        return sb;
    }

    static ExteriorOrientationElements FromInteriorAndObjectCoordinate(const InteriorOrientationElements &internal, const Matrix &object)
    {
        const double &x0 = internal.x0,
                     &y0 = internal.y0,
                     &f = internal.f,
                     &m = internal.m;
        auto cols = object.cols();
        return ExteriorOrientationElements{
            .Xs = object.col(0).mean(),
            .Ys = object.col(1).mean(),
            .Zs = object.col(2).mean() + m * f,
            .Phi = 0,
            .Omega = 0,
            .Kappa = 0};
    }

    template <Linalg::Axis ax1, Linalg::Axis ax2, Linalg::Axis ax3>
    Matrix ToRotationMatrix() const noexcept
    {
        return Linalg::CsRotationMatrix<ax1, ax2, ax3>(Phi, Omega, Kappa);
    }
};

AGTB_PHOTOGRAMMETRY_END

#endif