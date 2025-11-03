#ifndef AGTB_PHOTOGRAPHIC_BASE_HPP
#define AGTB_PHOTOGRAPHIC_BASE_HPP

#include "../details/Macros.hpp"

#include <Eigen/Dense>

AGTB_PHOTOGRAPHIC_BEGIN

using Matrix = Eigen::MatrixXd;

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

AGTB_PHOTOGRAPHIC_END

#endif