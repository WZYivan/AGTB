#ifndef __AGTB_PHOTOGRAPHIC_BASE_HPP__
#define __AGTB_PHOTOGRAPHIC_BASE_HPP__

#include "../details/Macros.hpp"

#include "../Linalg/Base.hpp"

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
};

AGTB_PHOTOGRAMMETRY_END

#endif