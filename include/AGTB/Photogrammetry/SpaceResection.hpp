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
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Linalg/RotationMatrix.hpp"
#include "../Linalg/CorrectionOlsSolve.hpp"
#include "../Adjustment/ErrorMeasure.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace SpaceResection
{
    struct SpaceResectionParam
    {
        InteriorOrientationElements interior;
        Matrix photo, object;
    };

    struct SpaceResectionResult
    {
        ExteriorOrientationElements external;
        Matrix rotate;
        Matrix sigma;
        Matrix photo;
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
                           external.ToString(), m0);

            std::ostringstream oss;
            oss << "New Photo:\n"
                << photo.format(IO::EigenFmt::python_style) << "\n"
                << "Final Rotation:\n"
                << rotate.format(IO::EigenFmt::python_style) << "\n"
                << "Error Matrix:\n"
                << sigma.format(IO::EigenFmt::python_style) << "\n";
            sb.append(oss.str());

            return sb;
        }
    };

    bool IsInputValid(const Matrix &photo, const Matrix &object)
    {
        if (photo.rows() == object.rows() && photo.cols() == 2 && object.cols() == 3 && object.rows() >= 4)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief Image space aux system -> Image space system
     *
     * @param rotate
     * @param object
     * @param external
     * @return Matrix
     */
    Matrix TransformToImageSpaceCoordinateSystem(const Matrix &rotate, const Matrix &object, const ExteriorOrientationElements &external)
    {
        auto pc = object.rows();
        const double &Xs = external.Xs,
                     &Ys = external.Ys,
                     &Zs = external.Zs;
        // rotate(matrix) : ImgAux -> ImgSp
        return Linalg::CsRotateForward(Linalg::CsTranslate(object, Xs, Ys, Zs), rotate); // ImgAux -> ImgSp : forward
    }

    /**
     * @brief Image space system -> Image plane system
     *
     * @param internal
     * @param transformed_obj
     * @return Matrix
     */
    Matrix CalculateImagePlaneCoordinates(const InteriorOrientationElements &internal, const Matrix &transformed_obj)
    {
        Matrix trans_p(transformed_obj.rows(), 2);
        const auto &XBar = transformed_obj.col(0),
                   &YBar = transformed_obj.col(1),
                   &ZBar = transformed_obj.col(2);
        const double &f = internal.f;
        trans_p.col(0) = -f * (XBar.array() / ZBar.array()).matrix(); // x
        trans_p.col(1) = -f * (YBar.array() / ZBar.array()).matrix(); // y
        return trans_p;
    }

    Matrix ResidualMatrix(const Matrix &photo, const Matrix &transformed_photo)
    {
        const Matrix dxy = photo - transformed_photo;
        const auto l = photo.rows();
        Matrix residual(2 * l, 1);
        for (auto li = 0uz; li != l; ++li)
        {
            residual(2 * li) = dxy(li, 0);
            residual(2 * li + 1) = dxy(li, 1);
        }
        return residual;
    }

    template <CollinearityEquationCoeffOption __equation_opt>
    Matrix SpaceResectionCoefficient(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
    {
        AGTB_UNKNOWN_TEMPLATE_PARAM();
    }

    template <>
    Matrix SpaceResectionCoefficient<CollinearityEquationCoeffOption::FullAngles>(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
    {
        auto pc = transformed_photo.rows();
        Matrix coefficient(pc * 2, 6);

        const double &f = internal.f,
                     &m = internal.m,
                     &k = external.Kappa,
                     &w = external.Omega;
        const auto &a = rotate.row(0),
                   &b = rotate.row(1),
                   &c = rotate.row(2);
        double H = f * m;

        CollinearityEquationCoeffParam param{
            .f = f,
            .H = H,
            .kappa = k,
            .omega = w,
            .rotate = rotate};

        for (auto pi = 0uz; pi != pc; ++pi)
        {
            const double
                &x = transformed_photo(pi, 0),
                &y = transformed_photo(pi, 1),
                &z = transformed_obj(pi, 2);
            param.x = x;
            param.y = y;
            param.z = z;

            auto c =
                CalculateCoeff<CollinearityEquationCoeffOption::FullAngles>(param);

            const auto &&l = 2 * pi;
            coefficient.row(l) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(l + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        return coefficient;
    }

    template <>
    Matrix SpaceResectionCoefficient<CollinearityEquationCoeffOption::KappaOnly>(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
    {
        auto pc = transformed_photo.rows();
        Matrix coefficient(pc * 2, 6);

        const double &f = internal.f,
                     &m = internal.m,
                     &k = external.Kappa;

        double H = f * m;
        CollinearityEquationCoeffParam param{
            .f = f,
            .H = H,
            .kappa = k};
        for (auto pi = 0uz; pi != pc; ++pi)
        {
            const double
                &x = transformed_photo(pi, 0),
                &y = transformed_photo(pi, 1);
            param.x = x;
            param.y = y;

            auto c =
                CalculateCoeff<CollinearityEquationCoeffOption::KappaOnly>(param);

            const auto &&l = 2 * pi;
            coefficient.row(l) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(l + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        return coefficient;
    }
    template <>
    Matrix SpaceResectionCoefficient<CollinearityEquationCoeffOption::NoAngles>(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
    {
        auto pc = transformed_photo.rows();
        Matrix coefficient(pc * 2, 6);

        const double &f = internal.f;
        double H = f * internal.m;
        CollinearityEquationCoeffParam param{
            .f = f,
            .H = H};
        for (auto pi = 0uz; pi != pc; ++pi)
        {
            const double
                &x = transformed_photo(pi, 0),
                &y = transformed_photo(pi, 1);
            param.x = x;
            param.y = y;

            auto c =
                CalculateCoeff<CollinearityEquationCoeffOption::NoAngles>(param);

            const auto &&l = 2 * pi;
            coefficient.row(l) << c.a11, c.a12, c.a13, c.a14, c.a15, c.a16;
            coefficient.row(l + 1) << c.a21, c.a22, c.a23, c.a24, c.a25, c.a26;
        }

        return coefficient;
    }

    void UpdateExternalElements(ExteriorOrientationElements &external, const Matrix &correction)
    {
        external.Xs += correction(0);
        external.Ys += correction(1);
        external.Zs += correction(2);
        external.Phi += correction(3);
        external.Omega += correction(4);
        external.Kappa += correction(5);
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

    void CompleteResult(SpaceResectionResult &result, const Matrix &coefficient, const Matrix &correction, const Matrix &residual, Matrix &rotate, const Matrix &photo, const Matrix &N)
    {
        const Matrix &A = coefficient, &x = correction, &L = residual;
        Matrix V = A * x - L;
        result.m0 = Adjustment::MeanRootSquareError(V, coefficient.rows(), 6);
        result.sigma = Adjustment::ErrorMatrix(result.m0, N);
        result.rotate = std::move(rotate);
        result.photo = Matrix(photo);
        Matrix &p = result.photo;
        for (size_t pc = coefficient.rows() / 2, pi = 0uz; pi != pc; ++pi)
        {
            p(pi, 0) = (p(pi, 0) + V(2 * pi)) * 1000;
            p(pi, 1) = (p(pi, 1) + V(2 * pi + 1)) * 1000;
        }
    }

    /**
     * @brief Execute space resection algorithm.
     *
     * @tparam __linalg_opt Inverse method
     * @tparam __equation_opt Coefficient method
     * @param param
     * @param max_loop
     * @param threshold
     * @return SpaceResectionResult
     */
    template <Linalg::LinalgOption __linalg_opt = Linalg::LinalgOption::Cholesky, CollinearityEquationCoeffOption __equation_opt = CollinearityEquationCoeffOption::NoAngles>
    SpaceResectionResult Solve(const SpaceResectionParam &param, size_t max_loop = 50, const double threshold = 3e-5) [[nodiscard]]
    {
        auto &internal = param.interior;
        auto &photo = param.photo;
        auto &object = param.object;

        if (!IsInputValid(photo, object))
        {
            throw std::invalid_argument("Arguments are invalid");
        }

        SpaceResectionResult result{
            .external = ExteriorOrientationElements::FromInteriorAndObjectCoordinate(internal, object),
            .info = IterativeSolutionInfo::NotConverged};
        ExteriorOrientationElements &external = result.external;

        while (max_loop-- > 0)
        {
            Matrix rotate = external.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>();

            Matrix transformed_obj = TransformToImageSpaceCoordinateSystem(rotate, object, external);
            Matrix transformed_photo = CalculateImagePlaneCoordinates(internal, transformed_obj);
            Matrix residual = ResidualMatrix(photo, transformed_photo);
            Matrix coefficient = SpaceResectionCoefficient<__equation_opt>(rotate, transformed_obj, transformed_photo, external, internal);
            Matrix correction = Linalg::CorrectionOlsSolve(coefficient, residual);

            UpdateExternalElements(external, correction);

            if (IsExternalElementsConverged(correction, threshold))
            {
                Matrix N = Linalg::NormalEquationMatrixInverse<__linalg_opt>(coefficient);
                CompleteResult(result, coefficient, correction, residual, rotate, photo, N);
                result.info = IterativeSolutionInfo::Success;
                break;
            }
        }

        return result;
    }

    /**
     * @brief Space resection template param pack
     *
     * @tparam __linalg_opt Inverse method
     * @tparam __equation_opt Coefficient method
     */
    template <Linalg::LinalgOption __linalg_opt = Linalg::LinalgOption::Cholesky, CollinearityEquationCoeffOption __equation_opt = CollinearityEquationCoeffOption::NoAngles>
    struct SpaceResectionTParam
    {
        constexpr static Linalg::LinalgOption linalg_option = __linalg_opt;
        constexpr static CollinearityEquationCoeffOption equation_option = __equation_opt;
    };

    template <typename T>
    concept SpaceResectionTParamConcept = requires {
        { T::linalg_option } -> std::convertible_to<Linalg::LinalgOption>;
        { T::equation_option } -> std::convertible_to<CollinearityEquationCoeffOption>;
    };

    /**
     * @brief Execute space resection algorithm.
     *
     * @tparam Tp Space resection template param pack
     * @param param
     * @param max_loop
     * @param threshold
     * @return SpaceResectionResult
     */
    template <SpaceResectionTParamConcept Tp>
    SpaceResectionResult Solve(const SpaceResectionParam &param, size_t max_loop = 50, const double threshold = 3e-5) [[nodiscard]]
    {
        return Solve<Tp::linalg_option, Tp::equation_option>(param, max_loop, threshold);
    }
}

using SpaceResection::Solve;
using SpaceResection::SpaceResectionParam;
using SpaceResection::SpaceResectionResult;
using SpaceResection::SpaceResectionTParam;

AGTB_PHOTOGRAMMETRY_END

#endif