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
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Linalg/RotationMatrix.hpp"
#include "../Linalg/CorrectionOlsSolve.hpp"
#include "../Adjustment/ErrorMeasure.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace SpaceResection
{
    struct SpaceResectionSolveResult
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
                           " SpaceResectionSolveResult ",
                           external.ToString(), m0);

            std::ostringstream oss;
            oss << "New Photo:\n"
                << photo.format(EigenIO::Fmt::python_style) << "\n"
                << "Final Rotation:\n"
                << rotate.format(EigenIO::Fmt::python_style) << "\n"
                << "Error Matrix:\n"
                << sigma.format(EigenIO::Fmt::python_style) << "\n";
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

    Matrix TransformToImageSpaceCoordinateSystem(const Matrix &rotate, const Matrix &object, const ExteriorOrientationElements &external)
    {
        auto pc = object.rows();
        const double &Xs = external.Xs,
                     &Ys = external.Ys,
                     &Zs = external.Zs;
        return Linalg::CsRotate(Linalg::CsTranslate(object, Xs, Ys, Zs), rotate);
    }

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

    enum class SpaceResectionCoeffOption : size_t
    {
        FullAngles,
        KappaOnly,
        NoAngles
    };

    namespace details
    {
        Matrix FullAngles(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
        {
            auto pc = transformed_photo.rows();
            Matrix coefficient(pc * 2, 6);

            const double &f = internal.f,
                         &k = external.Kappa,
                         &w = external.Omega;
            const auto &a = rotate.row(0),
                       &b = rotate.row(1),
                       &c = rotate.row(2);
            const double cosk = cos(k), sink = sin(k),
                         cosw = cos(w), sinw = sin(w);

            for (auto pi = 0uz; pi != pc; ++pi)
            {
                const double
                    &ZBar = transformed_obj(pi, 2),
                    &dx = transformed_photo(pi, 0),
                    &dy = transformed_photo(pi, 1);

                const double
                    a11 = 1 / ZBar * (a(0) * f + a(2) * dx),
                    a12 = 1 / ZBar * (b(0) * f + b(2) * dx),
                    a13 = 1 / ZBar * (c(0) * f + c(2) * dx),
                    a21 = 1 / ZBar * (a(1) * f + a(2) * dy),
                    a22 = 1 / ZBar * (b(1) * f + b(2) * dy),
                    a23 = 1 / ZBar * (c(1) * f + c(2) * dy),

                    a14 = dy * sinw - (dx / f * (dx * cosk - dy * sink) + f * cosk) * cosw,
                    a15 = -f * sink - dx / f * (dx * sink + dy * cosk),
                    a16 = dy,
                    a24 = -dx * sinw - (dy / f * (dx * cosk - dy * sink) - f * sink) * cosw,
                    a25 = -f * cosk - dy / f * (dx * sink + dy * cosk),
                    a26 = -dx;

                const auto &&l = 2 * pi;
                coefficient.row(l) << a11, a12, a13, a14, a15, a16;
                coefficient.row(l + 1) << a21, a22, a23, a24, a25, a26;
            }

            return coefficient;
        }

        Matrix KappaOnly(const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
        {
            auto pc = transformed_photo.rows();
            Matrix coefficient(pc * 2, 6);

            const double &f = internal.f,
                         &m = internal.m,
                         &k = external.Kappa;

            const double cosk = cos(k), sink = sin(k),
                         H = m * f;
            const double a11 = -f / H * cosk,
                         a12 = -f / H * sink,
                         a21 = f / H * sink,
                         a22 = -f / H * cosk;

            for (auto pi = 0uz; pi != pc; ++pi)
            {
                const double
                    &dx = transformed_photo(pi, 0),
                    &dy = transformed_photo(pi, 1);
                const double
                    dxdx = f + pow(dx, 2) / f,
                    dydy = f + pow(dy, 2) / f,
                    dxdy = dx * dy / f;

                const double
                    a13 = -dx / H,
                    a23 = -dy / H,
                    a14 = -dxdx * cosk + dxdy * sink,
                    a15 = -dxdy * cosk - dxdx * sink,
                    a16 = dy,
                    a24 = -dxdy * cosk + dydy * sink,
                    a25 = -dydy * cosk - dxdy * sink,
                    a26 = -dx;

                const auto &&l = 2 * pi;
                coefficient.row(l) << a11, a12, a13, a14, a15, a16;
                coefficient.row(l + 1) << a21, a22, a23, a24, a25, a26;
            }

            return coefficient;
        }

        Matrix NoAngles(const Matrix &transformed_photo, const InteriorOrientationElements &internal)
        {
            auto pc = transformed_photo.rows();
            Matrix coefficient(pc * 2, 6);

            const double &f = internal.f;
            const double
                H = f * internal.m,
                a11 = -f / H,
                a12 = 0,
                a21 = 0,
                a22 = -f / H;
            for (auto pi = 0uz; pi != pc; ++pi)
            {
                const double
                    &dx = transformed_photo(pi, 0),
                    &dy = transformed_photo(pi, 1);
                const double
                    dxdx = f + pow(dx, 2) / f,
                    dydy = f + pow(dy, 2) / f,
                    dxdy = dx * dy / f;

                const double
                    a13 = -dx / H,
                    a23 = -dy / H,
                    a14 = -dxdx,
                    a15 = -dxdy,
                    &a16 = dy,
                    &a24 = a15,
                    a25 = -dydy,
                    a26 = -dx;

                const auto &&l = 2 * pi;
                coefficient.row(l) << a11, a12, a13, a14, a15, a16;
                coefficient.row(l + 1) << a21, a22, a23, a24, a25, a26;
            }

            return coefficient;
        }
    }

    template <SpaceResectionCoeffOption opt>
    Matrix SpaceResectionCoefficient(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
    {
        if constexpr (opt == SpaceResectionCoeffOption::FullAngles)
        {
            return details::FullAngles(rotate, transformed_obj, transformed_photo, external, internal);
        }
        else if constexpr (opt == SpaceResectionCoeffOption::KappaOnly)
        {
            return details::KappaOnly(transformed_photo, external, internal);
        }
        else if constexpr (opt == SpaceResectionCoeffOption::NoAngles)
        {
            return details::NoAngles(transformed_photo, internal);
        }
        else
        {
            AGTB_UNKNOWN_TEMPLATE_PARAM();
        }
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

    bool IsExternalElementsConverged(const Matrix &correction, const double threshold)
    {
        const std::array<std::reference_wrapper<const double>, 3> angles{
            std::ref(correction(3)),
            std::ref(correction(4)),
            std::ref(correction(5))};
        return std::ranges::all_of(angles, [&threshold](const auto &a)
                                   { return abs(a) < threshold; });
    }

    void CompleteResult(SpaceResectionSolveResult &result, const Matrix &coefficient, const Matrix &correction, const Matrix &residual, Matrix &rotate, const Matrix &photo, const Matrix &N)
    {
        const Matrix &A = coefficient, &x = correction, &L = residual;
        Matrix V = A * x - L;
        result.m0 = Adjustment::MeanRootSquareError(V, coefficient.rows() / 2, 6);
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

    template <SpaceResectionCoeffOption opt = SpaceResectionCoeffOption::NoAngles, Linalg::LinalgOption mtd = Linalg::LinalgOption::Cholesky>
    SpaceResectionSolveResult Solve(const InteriorOrientationElements &internal, const Matrix &photo, const Matrix &object, size_t max_loop = 50, const double threshold = 3e-5) [[nodiscard]]
    {
        if (!IsInputValid(photo, object))
        {
            throw std::invalid_argument("Arguments are invalid");
        }

        SpaceResectionSolveResult result{
            .external = ExteriorOrientationElements::FromInteriorAndObjectCoordinate(internal, object),
            .info = IterativeSolutionInfo::NotConverged};
        ExteriorOrientationElements &external = result.external;

        while (max_loop-- > 0)
        {
            Matrix rotate = external.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>();

            Matrix transformed_obj = TransformToImageSpaceCoordinateSystem(rotate, object, external);
            Matrix transformed_photo = CalculateImagePlaneCoordinates(internal, transformed_obj);
            Matrix residual = ResidualMatrix(photo, transformed_photo);
            Matrix coefficient = SpaceResectionCoefficient<opt>(rotate, transformed_obj, transformed_photo, external, internal);
            Matrix correction = Linalg::CorrectionOlsSolve(coefficient, residual);

            UpdateExternalElements(external, correction);

            if (IsExternalElementsConverged(correction, threshold))
            {
                Matrix N = Linalg::NormalEquationMatrixInverse<mtd>(coefficient);
                CompleteResult(result, coefficient, correction, residual, rotate, photo, N);
                result.info = IterativeSolutionInfo::Success;
            }
        }

        return result;
    }
}

AGTB_PHOTOGRAMMETRY_END

#endif