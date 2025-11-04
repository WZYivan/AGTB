#ifndef __AGTB_PHOTOGRAPHIC_BASE_HPP__
#define __AGTB_PHOTOGRAPHIC_BASE_HPP__

#include "../details/Macros.hpp"

#include "../Linalg/RotationMatrix.hpp"

#include <Eigen/Dense>

AGTB_PHOTOGRAMMETRY_BEGIN

using Linalg::Matrix;

enum class IterativeSolutionInfo : size_t
{
    Success,
    NotConverged,
    Failed,
    Unknown
};

struct InteriorOrientationElements
{
    double x0, y0, f, m;

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

    Matrix RotationMatrix_YXZ_CN() const noexcept
    {
        return Linalg::RotateY(Phi) * Linalg::RotateX(Omega) * Linalg::RotateZ(Kappa);
    }

    Matrix RotationMatrix_XYZ_EU() const noexcept
    {
        return Linalg::RotateX(Omega) * Linalg::RotateY(Phi) * Linalg::RotateZ(Kappa);
    }

    Matrix RotationMatrix_ZYX_USSR() const noexcept
    {
        return Linalg::RotateZ(Kappa) * Linalg::RotateY(Phi) * Linalg::RotateX(Omega);
    }

    enum RotationOption : size_t
    {
        YXZ,
        CN,
        XYZ,
        EU,
        ZYX,
        USSR
    };

    template <RotationOption opt>
    Matrix RotationMatrix() const noexcept
    {
        if constexpr (opt == RotationOption::CN || opt == RotationOption::YXZ)
        {
            return RotationMatrix_YXZ_CN();
        }
        else if constexpr (opt == RotationOption::EU || opt == RotationOption::XYZ)
        {
            return RotationMatrix_XYZ_EU();
        }
        else if constexpr (opt == RotationOption::USSR || opt == RotationOption::ZYX)
        {
            return RotationMatrix_ZYX_USSR();
        }
        else
        {
            AGTB_NOT_IMPLEMENT();
        }
    }
};

AGTB_PHOTOGRAMMETRY_END

#endif