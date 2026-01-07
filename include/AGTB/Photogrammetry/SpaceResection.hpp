#ifndef __AGTB_PHOTOGRAMMETRY_SPACE_RESECTION_HPP__
#define __AGTB_PHOTOGRAMMETRY_SPACE_RESECTION_HPP__

#pragma once

#include <Eigen/Dense>
#include <vector>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "../details/Macros.hpp"
#include "../IO/Eigen.hpp"
#include "Base.hpp"
#include "SpaceMath/CollinearityEquation.hpp"
#include "SpaceMath/Transform.hpp"
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Linalg/RotationMatrix.hpp"
#include "../Linalg/CorrectionOlsSolve.hpp"
#include "../Adjustment/ErrorMeasure.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace detail::SpaceResection
{
    constexpr CollinearityEquationStyle equation_style = CollinearityEquationStyle::Linearization;
    using Equation = ::AGTB::Photogrammetry::CollinearityEquation<equation_style>;
    using Simplify = Equation::Simplify;

    struct Param
    {
        InteriorOrientationElements interior;
        Matrix image, object;
    };

    struct Result
    {
        ExteriorOrientationElements exterior;
        Matrix rotate;
        Matrix sigma;
        Matrix image;
        double m0;
        IterativeSolutionInfo info;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);

            std::format_to(sbb,
                           "{:=^100}\n"
                           "ExteriorOrientationElements:\n{}\n"
                           "Median Error:\n{}\n",
                           " SpaceResectionResult ",
                           exterior.ToString(), m0);

            std::ostringstream oss;
            oss << "New Photo:\n"
                << image.format(IO::EigenFmt::python_style) << "\n"
                << "Final Rotation:\n"
                << rotate.format(IO::EigenFmt::python_style) << "\n"
                << "Error Matrix:\n"
                << sigma.format(IO::EigenFmt::python_style) << "\n";
            sb.append(oss.str());

            return sb;
        }
    };

    bool IsInputValid(const Matrix &image, const Matrix &object)
    {
        if (image.rows() == object.rows() && image.cols() == 2 && object.cols() == 3 && object.rows() >= 4)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    Matrix ResidualMatrix(const Matrix &img, const Matrix &img_calc)
    {
        const Matrix dxy = img - img_calc;
        const auto l = img.rows();
        Matrix residual(2 * l, 1);
        for (auto li = 0uz; li != l; ++li)
        {
            residual(2 * li) = dxy(li, 0);
            residual(2 * li + 1) = dxy(li, 1);
        }
        return residual;
    }

    template <Simplify __simplify>
    Matrix SpaceResectionCoefficient(const Matrix &rotate, const Matrix &img_sp, const Matrix &img_calc, const ExteriorOrientationElements &ex, const InteriorOrientationElements &in)
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    }

    template <>
    Matrix SpaceResectionCoefficient<Simplify::None>(const Matrix &rotate, const Matrix &img_sp, const Matrix &img_calc, const ExteriorOrientationElements &ex, const InteriorOrientationElements &in)
    {
        auto pc = img_calc.rows();
        Matrix coefficient(pc * 2, 6);

        const double &f = in.f,
                     &m = in.m,
                     &k = ex.Kappa,
                     &w = ex.Omega;
        const auto &a = rotate.row(0),
                   &b = rotate.row(1),
                   &c = rotate.row(2);
        double H = f * m;

        Equation::Param param{
            .f = f,
            .H = H,
            .kappa = k,
            .omega = w,
            .rotate = rotate};

        for (auto pi = 0uz; pi != pc; ++pi)
        {
            const double
                &x = img_calc(pi, 0),
                &y = img_calc(pi, 1),
                &z = img_sp(pi, 2);
            param.x = x;
            param.y = y;
            param.z = z;

            auto c =
                Equation::Solve<Simplify::None>(param);

            const auto &&l = 2 * pi;
            coefficient.row(l) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(l + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        return coefficient;
    }

    template <>
    Matrix SpaceResectionCoefficient<Simplify::Kappa>(const Matrix &rotate, const Matrix &img_sp, const Matrix &img_calc, const ExteriorOrientationElements &ex, const InteriorOrientationElements &in)
    {
        auto pc = img_calc.rows();
        Matrix coefficient(pc * 2, 6);

        const double &f = in.f,
                     &m = in.m,
                     &k = ex.Kappa;

        double H = f * m;
        Equation::Param param{
            .f = f,
            .H = H,
            .kappa = k};
        for (auto pi = 0uz; pi != pc; ++pi)
        {
            const double
                &x = img_calc(pi, 0),
                &y = img_calc(pi, 1);
            param.x = x;
            param.y = y;

            auto c =
                Equation::Solve<Simplify::Kappa>(param);

            const auto &&l = 2 * pi;
            coefficient.row(l) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(l + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        return coefficient;
    }
    template <>
    Matrix SpaceResectionCoefficient<Simplify::All>(
        const Matrix &rotate,
        const Matrix &img_sp,
        const Matrix &img_calc,
        const ExteriorOrientationElements &ex,
        const InteriorOrientationElements &in)
    {
        auto pc = img_calc.rows();
        Matrix coefficient(pc * 2, 6);

        const double &f = in.f;
        double H = f * in.m;
        Equation::Param param{
            .f = f,
            .H = H};
        for (auto pi = 0uz; pi != pc; ++pi)
        {
            const double
                &x = img_calc(pi, 0),
                &y = img_calc(pi, 1);
            param.x = x;
            param.y = y;

            auto c =
                Equation::Solve<Simplify::All>(param);

            const auto &&l = 2 * pi;
            coefficient.row(l) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(l + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        return coefficient;
    }

    void UpdateExternalElements(ExteriorOrientationElements &exterior, const Matrix &correction)
    {
        exterior.Xs += correction(0);
        exterior.Ys += correction(1);
        exterior.Zs += correction(2);
        exterior.Phi += correction(3);
        exterior.Omega += correction(4);
        exterior.Kappa += correction(5);
    }

    /**
     * @brief Check if angle corrections are converged
     *
     * @param correction
     * @param threshold
     * @return true
     * @return false
     */
    bool IsExternalElementsConverged(const Matrix &correction, const double threshold)
    {
        const std::array<std::reference_wrapper<const double>, 3> angles{
            std::ref(correction(3)),
            std::ref(correction(4)),
            std::ref(correction(5))};
        return std::ranges::all_of(angles, [&threshold](const auto &a)
                                   { return abs(a) < threshold; });
    }

    void CompleteResult(Result &result, const Matrix &coefficient, const Matrix &correction, const Matrix &residual, Matrix &rotate, const Matrix &img, const Matrix &N)
    {
        const Matrix &A = coefficient, &x = correction, &L = residual;
        Matrix V = A * x - L;
        result.m0 = Adjustment::MeanRootSquareError(V, coefficient.rows(), 6);
        result.sigma = Adjustment::ErrorMatrix(result.m0, N);
        result.rotate = std::move(rotate);
        result.image = Matrix(img);
        Matrix &p = result.image;
        for (size_t pc = coefficient.rows() / 2, pi = 0uz; pi != pc; ++pi)
        {
            p(pi, 0) = (p(pi, 0) + V(2 * pi)) * 1000;
            p(pi, 1) = (p(pi, 1) + V(2 * pi + 1)) * 1000;
        }
    }
}

/**
 * @brief All in one space resection solver. Everything needed for algorithm is in scope.
 *
 */
struct SpaceResection
{
    constexpr static CollinearityEquationStyle equation_style = detail::SpaceResection::equation_style;
    using Equation = detail::SpaceResection::Equation;

    using Param = detail::SpaceResection::Param;
    using Result = detail::SpaceResection::Result;

    using InverseMethod = LinalgOption;
    using Simplify = typename Equation::Simplify;
    template <InverseMethod __inverse_method, Simplify __simplify>
    struct Config
    {
        constexpr static InverseMethod inverse_method = __inverse_method;
        constexpr static Simplify simplify = __simplify;
    };

    template <InverseMethod __inverse_method, Simplify __simplify>
    static Result Solve(const Param &param, size_t max_loop, const double threshold)
    {
        using namespace detail::SpaceResection;

        auto &interior = param.interior;
        auto &image = param.image;
        auto &object = param.object;

        if (!IsInputValid(image, object))
        {
            throw std::invalid_argument("Arguments are invalid");
        }

        Result result{
            .exterior = ExteriorOrientationElements::FromInteriorAndObjectCoordinate(interior, object),
            .info = IterativeSolutionInfo::NotConverged};
        ExteriorOrientationElements &exterior = result.exterior;

        while (max_loop-- > 0)
        {
            Matrix rotate = Transform::Ex2YXZ(exterior);
            Matrix isp = Transform::Aux2Isp(Transform::Obj2Aux(object, exterior), rotate);
            Matrix img_calc = Transform::Isp2Img(isp, interior);
            Matrix residual = ResidualMatrix(image, img_calc);
            Matrix coefficient = SpaceResectionCoefficient<__simplify>(rotate, isp, img_calc, exterior, interior);
            Matrix correction = Linalg::CorrectionOlsSolve(coefficient, residual);

#if (AGTB_DEBUG)
            IO::PrintEigen(isp, "isp");
            IO::PrintEigen(img_calc, "img calc");
            IO::PrintEigen(residual, "residual");
            IO::PrintEigen(coefficient, "coefficient");
            IO::PrintEigen(correction, "correction");
#endif
            if (IsExternalElementsConverged(correction, threshold))
            {
                Matrix N = Linalg::NormalEquationMatrixInverse<__inverse_method>(coefficient);
                CompleteResult(result, coefficient, correction, residual, rotate, image, N);
                result.info = IterativeSolutionInfo::Success;
                break;
            }

            UpdateExternalElements(exterior, correction);
        }

        return result;
    }

    template <typename __config = Config<InverseMethod::Cholesky, Simplify::None>>
    static Result Solve(const Param &param, size_t max_loop = 50, const double threshold = 3e-5)
    {
        return Solve<__config::inverse_method, __config::simplify>(param, max_loop, threshold);
    }
};

AGTB_PHOTOGRAMMETRY_END

#endif