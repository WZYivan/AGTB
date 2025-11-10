#ifndef __AGTB_CONTINUOUS_RELATIVE_ORIENTATE_HPP__
#define __AGTB_CONTINUOUS_RELATIVE_ORIENTATE_HPP__

#include "Base.hpp"
#include "../Linalg/CorrectionOlsSolve.hpp"
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Adjustment/ErrorMeasure.hpp"
#include "../IO/Eigen.hpp"

#include <gcem.hpp>
#include <tuple>
#include <format>
#include <sstream>

AGTB_PHOTOGRAMMETRY_BEGIN

namespace ContinuousRelativeOrientate
{
    struct ContinuousRelativeOrientationElements
    {
        double Mu, Nu, Phi, Omega, Kappa;

        std::string ToString() const noexcept
        {
            return std::format("{:=^50}\n"
                               "Mu = {}\n"
                               "Nu = {}\n"
                               "Phi = {}\n"
                               "Omega = {}"
                               "Kappa = {}\n",
                               " Continuous Relative Orientation Elements ", Mu, Nu, Phi, Omega, Kappa);
        }
    };

    struct ContinuousRelativeOrienteParam
    {
        Matrix xy1, xy2;
        InteriorOrientationElements in1, in2;
    };

    struct ContinuousRelativeOrienteResult
    {
        ContinuousRelativeOrientationElements cro;
        double m0;
        Matrix sigma;
        IterativeSolutionInfo info;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);
            std::format_to(sbb, "{:=^100}\n{}\nRMSE = {}\nSigma =\n",
                           " Continuous Relative Oriente Result",
                           cro.ToString(), m0);
            std::ostringstream oss{};
            oss << sigma.format(IO::EigenFmt::python_style) << "\n";
            sb.append(oss.str());
            return sb;
        }
    };

    Matrix CalculateImageSpaceCoordinates(const Matrix &xy1, const Matrix &xy2, const InteriorOrientationElements &in1, const InteriorOrientationElements &in2, const ContinuousRelativeOrientationElements &cro)
    {
        Matrix ipc(6, xy1.cols());
        ipc.leftCols(2) = xy1;
        ipc.col(2).array() = -in1.f;
        ipc.col(3) = xy2.col(0);
        ipc.col(4) = xy2.col(1);
        ipc.col(5).array() = -in2.f;

        Matrix rotate = Linalg::CsRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>(cro.Phi, cro.Omega, cro.Kappa);
        // rotate(matrix) : ImgSp_1 -> ImgSp_2
        ipc.rightCols(3) = Linalg::CsRotateInverse(ipc.rightCols(3), rotate); // ImgSp2 -> ImgSp1 : Inverse

        return ipc;
    }

    auto ContinuousRelativeOrientateCoefficientAndConstants(const Matrix &xy1, const Matrix &xy2, const Matrix &ipc, const ContinuousRelativeOrientationElements &ore)
    {
        Matrix coeff(ipc.rows(), 5);
        Matrix consts(ipc.rows(), 1);

        for (auto i = 0; i != ipc.rows(); ++i)
        {
            auto xy1_i = xy1.row(i),
                 xy2_i = xy2.row(i),
                 ipc_i = ipc.row(i);
            double x1 = xy1_i(0), x2 = xy2_i(0),
                   X1 = ipc_i(0), Y1 = ipc_i(1), Z1 = ipc(2),
                   X2 = ipc_i(3), Y2 = ipc_i(4), Z2 = ipc(5);
            double Bx = x1 - x2, By = Bx * ore.Mu, Bz = Bx * ore.Nu;
            double N1 = (Bx * Z2 - Bz * X2) / (X1 * Z2 - Z1 * X2),
                   N2 = (Bx * Z1 - Bz * X1) / (X1 * Z2 - Z1 * X2);
            double const_i = N1 * Y1 - (N2 * Y2 + By);
            double
                c_mu = Bx,
                c_nu = -Y2 / Z2 * Bx,
                c_phi = -X2 * Y2 / Z2 * N2,
                c_omega = -(Z2 * gcem::pow(Y2, 2) / Z2) * N2,
                c_kappa = X2 * N2;
            consts.row(i) << const_i;
            coeff.row(i) << c_mu, c_nu, c_phi, c_omega, c_kappa;
        }

        return std::make_tuple(coeff, consts);
    }

    void UpdateContinuousRelativeOrientationElements(ContinuousRelativeOrientationElements &cro, const Matrix &correction)
    {
        cro.Mu += correction(0);
        cro.Nu += correction(1);
        cro.Phi += correction(2);
        cro.Omega += correction(3);
        cro.Kappa += correction(4);
    }

    bool IsContinuousRelativeOrientationElementsConverged(const Matrix &correction, double threshold)
    {
        for (auto i = 0; i != correction.size(); ++i)
        {
            if (gcem::abs(correction(i)) > threshold)
            {
                return false;
            }
        }
        return true;
    }

    template <Linalg::LinalgOption opt = Linalg::LinalgOption::Cholesky>
    ContinuousRelativeOrienteResult Solve(const ContinuousRelativeOrienteParam &param, int max_loop = 50, double threshold = 3e-5)
    {
        auto &xy1 = param.xy1, xy2 = param.xy2;
        auto &in1 = param.in1, in2 = param.in2;

        k if (!(xy1.rows() != 0 && xy2.rows() != 0 && xy1.cols() == xy2.cols() && xy1.cols() > 5))
        {
            AGTB_THROW(std::invalid_argument, "Input coordinate must have same count and larger than 5.");
        }

        ContinuousRelativeOrienteResult cro_res{
            .cro = {
                .Mu = 0,
                .Nu = 0,
                .Phi = 0,
                .Omega = 0,
                .Kappa = 0},
            .info = IterativeSolutionInfo::NotConverged};
        ContinuousRelativeOrientationElements &cro = cro_res.cro;

        while (max_loop-- > 0)
        {
            Matrix left_ImageSpaceCoordinate = CalculateImageSpaceCoordinates(xy1, xy2, in1, in2, cro);
            auto [A, L] =
                ContinuousRelativeOrientateCoefficientAndConstants(xy1, xy2, left_ImageSpaceCoordinate, cro);

            Matrix correction = Linalg::CorrectionOlsSolve(A, L);

            UpdateContinuousRelativeOrientationElements(cro, correction);

            if (IsContinuousRelativeOrientationElementsConverged(correction, threshold))
            {
                Matrix N = Linalg::NormalEquationMatrixInverse<opt>(A);
                Matrix V = A * correction - L;
                cro_res.m0 = Adjustment::MeanRootSquareError(V, xy1.rows(), 5);
                cro_res.sigma = Adjustment::ErrorMatrix(cro_res.m0, N);
                cro_res.info = IterativeSolutionInfo::Success;
                break;
            }
        }

        return cro_res;
    }
}

using ContinuousRelativeOrientate::ContinuousRelativeOrientationElements;
using ContinuousRelativeOrientate::ContinuousRelativeOrienteParam;
using ContinuousRelativeOrientate::ContinuousRelativeOrienteResult;
using ContinuousRelativeOrientate::Solve;

AGTB_PHOTOGRAMMETRY_END

#endif