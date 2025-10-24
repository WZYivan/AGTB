#ifndef AGTB_SPACERESECTION_HPP
#define AGTB_SPACERESECTION_HPP

#pragma once

#include <Eigen/Dense>
#include <vector>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <filesystem>

#include "details/Macros.hpp"

AGTB_BEGIN

/**
 * @brief Space Resection Namespace
 *
 * Provides complete implementation of space resection algorithms for solving
 * exterior orientation parameters in photogrammetry.
 * Includes multiple solving methods, data I/O, and result visualization capabilities.
 */
namespace SpaceResection
{
    using Matrix = Eigen::MatrixXd;

    struct InternalElements;
    struct ExternalElements;
    struct SpaceResectionSolveResult;

    namespace details
    {
        bool IsInputValid(const Matrix &photo, const Matrix &object);
        ExternalElements InitExternalElements(const InternalElements &internal, const Matrix &object);
        Matrix RotationMatrix(const ExternalElements &external);
        Matrix TransformedObjectiveCoordinates(const Matrix &rotate, const Matrix &object, const ExternalElements &external);
        Matrix TransformedPhotoCoordinates(const InternalElements &internal, const Matrix &transformed_obj);
        Matrix ResidualMatrix(const Matrix &photo, const Matrix &transformed_photo);

        namespace CoefficientCalculator
        {
            enum class Tag : size_t;
            struct CoefficientCalculatorParams;

            template <Tag ccTag>
            void Invoke(Matrix &coefficient, CoefficientCalculatorParams params);

            namespace details
            {
                Matrix FullAngles(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExternalElements &external, const InternalElements &internal);
                Matrix KappaOnly(const Matrix &transformed_photo, const ExternalElements &external, const InternalElements &internal);
                Matrix NoAngles(const Matrix &transformed_photo, const InternalElements &internal);
            }

        }

        Matrix CorrectionMatrix(const Matrix &N, const Matrix &residual);
        void UpdateExternalElements(ExternalElements &external, const Matrix &correction);
        bool IsExternalElementsConverged(const Matrix &correction, const double threshold);
        void CompleteResult(SpaceResectionSolveResult &result, const Matrix &coefficient, const Matrix &correction, const Matrix &residual, Matrix &rotate, const Matrix &photo, const Matrix &N);

        namespace NormalizationMethod
        {
            enum class Tag : size_t;

            namespace details
            {
                Matrix Cholesky(const Matrix &A);
                Matrix SVD(const Matrix &A);
            }

            template <Tag nmTag>
            void Invoke(Matrix &N, const Matrix &A);
        }

        enum class Info : size_t;
    }

    using CoefficientTag = details::CoefficientCalculator::Tag;
    using NormalizationTag = details::NormalizationMethod::Tag;
    using details::Info;

    /**
     * @brief Printing and Output Module
     *
     * Provides formatted output functions for debugging and result display.
     */
    namespace Print
    {
        namespace fmt
        {
            static const Eigen::IOFormat bordered(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]");
        }

        void Matrix(const SpaceResection::Matrix &m, const std::string msg, const Eigen::IOFormat &fmt = Print::fmt::bordered, std::ostream &os = std::cout);
        void ExternalElements(const SpaceResection::ExternalElements &external, std::ostream &os = std::cout);
        void InternalElements(const SpaceResection::InternalElements &internal, std::ostream &os = std::cout);
        void Result(const SpaceResectionSolveResult &, std::ostream &os = std::cout);
    }

    /**
     * @brief Input/Output Module
     *
     * Provides data reading and parsing functionality with support for multiple data formats and layouts.
     */
    namespace IO
    {
        namespace details
        {
            namespace in
            {
                template <typename valT, size_t col>
                using row_dynamic_matrix = std::vector<std::array<valT, col>>;

                template <typename valT, size_t col>
                std::array<valT, col> SplitLineThenCast(std::string &line, const std::string sep = ",");
            }
        }

        enum class ReadTag : size_t;

        template <ReadTag rT>
        Matrix Read(std::istream &is, const std::string sep = ",");
    }

    /**
     * @brief Solving Module
     *
     * Provides main solving algorithms and utility functions for space resection.
     */
    namespace Solve
    {
        namespace Core = SpaceResection::details;
        namespace Print = SpaceResection::Print;
        using SpaceResection::CoefficientTag;
        using SpaceResection::ExternalElements;
        using SpaceResection::Info;
        using SpaceResection::InternalElements;
        using SpaceResection::Matrix;
        using SpaceResection::NormalizationTag;
        using SpaceResection::SpaceResectionSolveResult;

        namespace Utils
        {
            using SpaceResection::IO::Read;
            using SpaceResection::IO::ReadTag;

            enum class MatrixLayout : size_t;

            template <MatrixLayout layout>
            auto ReadMatrix(const std::string path, std::string sep = ",");

            template <MatrixLayout layout>
            auto ReadMatrix(std::istream &is, std::string sep = ",");
        }

        using Utils::MatrixLayout;
        using Utils::ReadMatrix;
    }
}

namespace SpaceResection
{
    /**
     * @brief Internal Orientation Elements Structure
     *
     * Contains interior orientation parameters describing camera internal geometry:
     * - Principal point coordinates (x0, y0)
     * - Focal length f
     * - Photo scale denominator m
     */
    struct InternalElements
    {
        double x0, y0, f, m;
    };

    /**
     * @brief External Orientation Elements Structure
     *
     * Contains exterior orientation parameters describing camera position and orientation:
     * - Projection center coordinates (Xs, Ys, Zs)
     * - Three rotation angles (Phi, Omega, Kappa) around X, Y, Z axes respectively
     */
    struct ExternalElements
    {
        double Xs, Ys, Zs, Phi, Omega, Kappa;
    };

    /**
     * @brief Space Resection Solution Result Structure
     *
     * Contains complete output of space resection computation:
     * - Exterior orientation elements
     * - Rotation matrix
     * - Accuracy assessment parameters
     * - Adjusted photo coordinates
     * - Solution status information
     */
    struct SpaceResectionSolveResult
    {
        ExternalElements external;
        Matrix rotate;
        Matrix sigma;
        Matrix photo;
        double m0;
        Info info;
    };

    namespace details
    {
        /**
         * @brief Validates input data for space resection
         *
         * Checks if input matrices meet the requirements for space resection computation:
         * - Same number of points in photo and object coordinates
         * - Photo coordinates must be 2D (x,y)
         * - Object coordinates must be 3D (X,Y,Z)
         * - Minimum 4 control points required
         * - Even number of control points for equation stability
         *
         * @param photo Photo coordinate matrix of size n×2
         * @param object Object coordinate matrix of size n×3
         * @return true Input data is valid
         * @return false Input data is invalid
         */
        bool IsInputValid(const Matrix &photo, const Matrix &object)
        {
            if (photo.rows() == object.rows() && photo.cols() == 2 && object.cols() == 3 && object.rows() >= 4 && object.rows() % 2 == 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /**
         * @brief Initializes exterior orientation elements
         *
         * Provides initial values for exterior orientation parameters based on:
         * - Projection center coordinates as mean of object coordinates
         * - Zs initialized as mean elevation plus flying height (m×f)
         * - Rotation angles initialized to zero (assuming near-vertical photography)
         *
         * @param internal Internal orientation elements containing f and m
         * @param object Object coordinate matrix for initial position calculation
         * @return ExternalElements Initialized exterior orientation elements
         */
        ExternalElements InitExternalElements(const InternalElements &internal, const Matrix &object)
        {
            const double &x0 = internal.x0,
                         &y0 = internal.y0,
                         &f = internal.f,
                         &m = internal.m;
            auto cols = object.cols();
            return ExternalElements{
                .Xs = object.col(0).mean(),
                .Ys = object.col(1).mean(),
                .Zs = object.col(2).mean() + m * f,
                .Phi = 0,
                .Omega = 0,
                .Kappa = 0};
        }

        /**
         * @brief Computes rotation matrix from Euler angles
         *
         * Constructs 3×3 rotation matrix using three exterior orientation angles
         * in Z-Y-X rotation order (Kappa-Omega-Phi):
         * R = R_phi × R_omega × R_kappa
         *
         * @param external Exterior orientation elements containing rotation angles
         * @return Matrix 3×3 rotation matrix for coordinate system transformation
         */
        Matrix RotationMatrix(const ExternalElements &external)
        {
            const double
                &phi = external.Phi,
                &omega = external.Omega,
                &kappa = external.Kappa;
            const double
                sinp = sin(phi),
                cosp = cos(phi),
                sinw = sin(omega),
                cosw = cos(omega),
                sink = sin(kappa),
                cosk = cos(kappa);
            Matrix Rp(3, 3), Rw(3, 3), Rk(3, 3);
            Rp << cosp, 0, -sinp,
                0, 1, 0,
                sinp, 0, cosp;
            Rw << 1, 0, 0,
                0, cosw, -sinw,
                0, sinw, cosw;
            Rk << cosk, -sink, 0,
                sink, cosk, 0,
                0, 0, 1;
            return Rp * Rw * Rk;
        }

        /**
         * @brief Transforms object coordinates to image space auxiliary coordinates
         *
         * Converts object coordinates to camera-centered coordinate system:
         * [X̄, Ȳ, Z̄]ᵀ = Rᵀ × ([X, Y, Z]ᵀ - [Xs, Ys, Zs]ᵀ)
         *
         * This transformation prepares coordinates for collinearity equation computation.
         *
         * @param rotate Rotation matrix describing camera orientation
         * @param object Original object coordinate matrix
         * @param external Exterior orientation elements containing projection center
         * @return Matrix Transformed object coordinates in image space auxiliary system
         */
        Matrix TransformedObjectiveCoordinates(const Matrix &rotate, const Matrix &object, const ExternalElements &external)
        {
            auto pc = object.rows();
            const double &Xs = external.Xs,
                         &Ys = external.Ys,
                         &Zs = external.Zs;
            Matrix trans_o(pc, 3);

            for (auto pi = 0uz; pi != pc; ++pi)
            {
                Matrix obj_i = object.row(pi).transpose();
                obj_i(0) -= Xs;
                obj_i(1) -= Ys;
                obj_i(2) -= Zs;
                auto oib = rotate.transpose() * obj_i;
                trans_o.row(pi) << oib(0), oib(1), oib(2);
            }
            return trans_o;
        }

        /**
         * @brief Computes theoretical image coordinates using collinearity equations
         *
         * Calculates theoretical image coordinates based on collinearity principle:
         * x = -f × (X̄ / Z̄)
         * y = -f × (Ȳ / Z̄)
         *
         * Negative signs indicate opposite directions between image and object coordinates.
         *
         * @param internal Internal orientation elements containing focal length
         * @param transformed_obj Transformed object coordinates in auxiliary system
         * @return Matrix Theoretical image coordinates matrix
         */
        Matrix TransformedPhotoCoordinates(const InternalElements &internal, const Matrix &transformed_obj)
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

        /**
         * @brief Computes residual matrix between observed and theoretical coordinates
         *
         * Calculates differences between observed image coordinates and computed theoretical coordinates:
         * Δx = x_observed - x_calculated
         * Δy = y_observed - y_calculated
         *
         * Residual matrix is arranged as: [Δx₁, Δy₁, Δx₂, Δy₂, ..., Δxₙ, Δyₙ]ᵀ
         * Used for least squares adjustment computation.
         *
         * @param photo Observed image coordinate matrix
         * @param transformed_photo Theoretical image coordinate matrix
         * @return Matrix Residual matrix of size 2n×1
         */
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

        namespace CoefficientCalculator
        {
            /**
             * @brief Coefficient matrix calculation method enumeration
             *
             * Defines different approaches for coefficient matrix computation:
             * - FullAngles: Complete method considering all three rotation angles
             * - KappaOnly: Simplified method considering only Kappa angle
             * - NoAngles: Most simplified method ignoring all rotation angles
             */
            enum class Tag : size_t
            {
                FullAngles,
                KappaOnly,
                NoAngles
            };

            /**
             * @brief Coefficient matrix calculation parameters structure
             *
             * Contains all parameters required for coefficient matrix computation.
             */
            struct CoefficientCalculatorParams
            {
                const Matrix &rotate;
                const Matrix &transformed_obj;
                const Matrix &transformed_photo;
                const ExternalElements &external;
                const InternalElements &internal;
            };

            /**
             * @brief Invokes coefficient matrix calculation method
             *
             * Selects and executes appropriate coefficient matrix computation method
             * based on template parameter. Coefficient matrix A describes relationship
             * between exterior orientation corrections and image coordinate residuals.
             *
             * @tparam ccTag Coefficient calculation method tag
             * @param coefficient Output coefficient matrix of size 2n×6
             * @param params Input parameters containing computation data
             */
            template <Tag ccTag>
            void Invoke(Matrix &coefficient, CoefficientCalculatorParams params)
            {

                if constexpr (ccTag == Tag::FullAngles)
                {
                    coefficient = details::FullAngles(params.rotate, params.transformed_obj, params.transformed_photo, params.external, params.internal);
                }
                else if constexpr (ccTag == Tag::KappaOnly)
                {
                    coefficient = details::KappaOnly(params.transformed_photo, params.external, params.internal);
                }
                else if constexpr (ccTag == Tag::NoAngles)
                {
                    coefficient = details::NoAngles(params.transformed_photo, params.internal);
                }
                else
                {
                    static_assert(false, "invoke with invalid tag");
                }
            }
            namespace details
            {
                /**
                 * @brief Full angles coefficient matrix calculation
                 *
                 * Computes coefficient matrix considering all three rotation angles.
                 * Based on complete linearization of collinearity equations.
                 * Suitable for arbitrary camera orientations.
                 *
                 * For each control point, generates two rows of coefficients:
                 * - Row 2i: ∂x/∂Xs, ∂x/∂Ys, ∂x/∂Zs, ∂x/∂Phi, ∂x/∂Omega, ∂x/∂Kappa
                 * - Row 2i+1: ∂y/∂Xs, ∂y/∂Ys, ∂y/∂Zs, ∂y/∂Phi, ∂y/∂Omega, ∂y/∂Kappa
                 *
                 * @param rotate Rotation matrix
                 * @param transformed_obj Transformed object coordinates
                 * @param transformed_photo Transformed image coordinates
                 * @param external Exterior orientation elements
                 * @param internal Internal orientation elements
                 * @return Matrix Coefficient matrix computed using full angles method
                 */
                Matrix FullAngles(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExternalElements &external, const InternalElements &internal)
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

                /**
                 * @brief Kappa-only coefficient matrix calculation
                 *
                 * Simplified method considering only Kappa rotation angle.
                 * Assumes Phi≈0 and Omega≈0 (near-vertical photography).
                 * Provides better computational efficiency while maintaining reasonable accuracy.
                 *
                 * @param transformed_photo Transformed image coordinates
                 * @param external Exterior orientation elements
                 * @param internal Internal orientation elements
                 * @return Matrix Coefficient matrix computed using kappa-only method
                 */
                Matrix KappaOnly(const Matrix &transformed_photo, const ExternalElements &external, const InternalElements &internal)
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

                /**
                 * @brief No-angles coefficient matrix calculation
                 *
                 * Most simplified method ignoring all rotation angles.
                 * Assumes perfectly vertical photography (Phi=0, Omega=0, Kappa=0).
                 * Maximum computational efficiency but limited to ideal conditions.
                 *
                 * @param transformed_photo Transformed image coordinates
                 * @param internal Internal orientation elements
                 * @return Matrix Coefficient matrix computed using no-angles method
                 */
                Matrix NoAngles(const Matrix &transformed_photo, const InternalElements &internal)
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

        }

        /**
         * @brief Computes correction matrix using least squares
         *
         * Solves for exterior orientation element corrections using least squares:
         * Δ = (AᵀA)⁻¹AᵀL
         *
         * Uses column-pivoting QR decomposition for numerical stability.
         * Correction vector contains 6 elements: [ΔXs, ΔYs, ΔZs, ΔPhi, ΔOmega, ΔKappa]ᵀ
         *
         * @param coefficient Coefficient matrix A of size 2n×6
         * @param residual Residual matrix L of size 2n×1
         * @return Matrix Correction matrix of size 6×1
         */
        Matrix CorrectionMatrix(const Matrix &coefficient, const Matrix &residual)
        {
            const Matrix &A = coefficient, &L = residual;
            return A.colPivHouseholderQr().solve(L);
        }

        /**
         * @brief Updates exterior orientation elements with corrections
         *
         * Applies computed corrections to current exterior orientation elements:
         * Xs_new = Xs_old + ΔXs
         * Ys_new = Ys_old + ΔYs
         * Zs_new = Zs_old + ΔZs
         * Phi_new = Phi_old + ΔPhi
         * Omega_new = Omega_old + ΔOmega
         * Kappa_new = Kappa_old + ΔKappa
         *
         * @param external Exterior orientation elements to be updated
         * @param correction Correction matrix containing 6 correction values
         */
        void UpdateExternalElements(ExternalElements &external, const Matrix &correction)
        {
            external.Xs += correction(0);
            external.Ys += correction(1);
            external.Zs += correction(2);
            external.Phi += correction(3);
            external.Omega += correction(4);
            external.Kappa += correction(5);
        }

        /**
         * @brief Checks convergence of exterior orientation elements
         *
         * Determines if iteration has converged based on angular corrections:
         * - Primarily checks three rotation angle corrections against threshold
         * - Linear element corrections not used as they are scale-dependent
         *
         * Convergence criterion: |ΔPhi|, |ΔOmega|, |ΔKappa| < threshold
         * Default threshold 3.0e-5 radians ≈ 6.2 arc-seconds
         *
         * @param correction Correction matrix
         * @param threshold Convergence threshold in radians
         * @return true All angular corrections below threshold, converged
         * @return false At least one angular correction above threshold, continue iteration
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

        /**
         * @brief Completes result computation and accuracy assessment
         *
         * After iteration convergence, computes final results and accuracy metrics:
         * - Unit weight mean square error m0
         * - Exterior orientation element accuracy assessment
         * - Adjusted image coordinates
         *
         * Unit weight error formula: m0 = √(VᵀV / (n - 6))
         * where n is number of observations (2×control points), 6 is number of unknowns
         *
         * @param result Output parameter storing complete space resection result
         * @param coefficient Coefficient matrix A
         * @param correction Correction matrix Δ
         * @param residual Residual matrix L
         * @param rotate Rotation matrix
         * @param photo Original image coordinates
         * @param N Normal equation inverse matrix (AᵀA)⁻¹
         */
        void CompleteResult(SpaceResectionSolveResult &result, const Matrix &coefficient, const Matrix &correction, const Matrix &residual, Matrix &rotate, const Matrix &photo, const Matrix &N)
        {
            const Matrix &A = coefficient, &x = correction, &L = residual;
            Matrix V = A * x - L;
            result.m0 = ((V.transpose() * V) / (coefficient.rows() - 6)).cwiseSqrt()(0);
            result.sigma = result.m0 * N.cwiseSqrt();
            result.rotate = std::move(rotate);
            result.photo = Matrix(photo);
            Matrix &p = result.photo;
            for (size_t pc = coefficient.rows() / 2, pi = 0uz; pi != pc; ++pi)
            {
                p(pi, 0) = (p(pi, 0) + V(2 * pi)) * 1000;
                p(pi, 1) = (p(pi, 1) + V(2 * pi + 1)) * 1000;
            }
        }

        namespace NormalizationMethod
        {
            /**
             * @brief Normal equation solution method enumeration
             *
             * Defines different approaches for solving normal equations:
             * - Cholesky: Cholesky decomposition for symmetric positive definite matrices
             * - SVD: Singular Value Decomposition for ill-conditioned matrices, better numerical stability
             */
            enum class Tag : size_t
            {
                Cholesky,
                SVD
            };

            namespace details
            {
                /**
                 * @brief Cholesky decomposition method for normal equations
                 *
                 * Uses Cholesky decomposition to solve normal equations (AᵀA)⁻¹:
                 * - First attempts LLT decomposition (for symmetric positive definite matrices)
                 * - Falls back to LDLT decomposition (for symmetric positive semi-definite matrices)
                 *
                 * Cholesky decomposition is computationally efficient but requires positive definite matrices.
                 * In space resection, normal equations are usually positive definite with well-distributed control points.
                 *
                 * @param A Coefficient matrix
                 * @return Matrix Normal equation inverse matrix (AᵀA)⁻¹ for accuracy assessment
                 */
                Matrix Cholesky(const Matrix &A)
                {
                    Matrix AtA = A.transpose() * A;
                    Eigen::LLT<Matrix> llt(AtA);
                    if (llt.info() == Eigen::Success)
                    {
                        Matrix invAtA = llt.solve(Matrix::Identity(AtA.rows(), AtA.cols()));
                        return invAtA;
                    }
                    else
                    {
                        Eigen::LDLT<Matrix> ldlt(AtA);
                        Matrix invAtA = ldlt.solve(Matrix::Identity(AtA.rows(), AtA.cols()));
                        return invAtA;
                    }
                }

                /**
                 * @brief SVD decomposition method for normal equations
                 *
                 * Uses Singular Value Decomposition (SVD) to solve normal equations:
                 * AᵀA = UΣVᵀ
                 * (AᵀA)⁻¹ = VΣ⁻¹Uᵀ
                 *
                 * SVD method provides better numerical stability and handles ill-conditioned matrices.
                 * Singular value thresholding avoids numerical instability from small singular values.
                 *
                 * @param A Coefficient matrix
                 * @return Matrix Normal equation inverse matrix (AᵀA)⁻¹
                 */
                Matrix SVD(const Matrix &A)
                {
                    Matrix AtA = A.transpose() * A;
                    Eigen::JacobiSVD<Matrix> svd(AtA, Eigen::ComputeFullU | Eigen::ComputeFullV);
                    const Eigen::VectorXd &singular_values = svd.singularValues();

                    double tolerance = singular_values(0) * std::max(AtA.rows(), AtA.cols()) * 1e-12;

                    Eigen::VectorXd inv_singular_values(singular_values.size());
                    for (int i = 0; i < singular_values.size(); ++i)
                    {
                        inv_singular_values(i) = (singular_values(i) > tolerance) ? 1.0 / singular_values(i) : 0.0;
                    }

                    Matrix invAtA = svd.matrixV() *
                                    inv_singular_values.asDiagonal() *
                                    svd.matrixU().transpose();
                    return invAtA;
                }

            }

            /**
             * @brief Invokes normal equation solution method
             *
             * Selects and executes appropriate normal equation solution method
             * based on template parameter. Normal equation inverse matrix N = (AᵀA)⁻¹
             * is used for exterior orientation element accuracy computation.
             *
             * @tparam nmTag Normal equation solution method tag
             * @param N Output normal equation inverse matrix of size 6×6
             * @param A Input coefficient matrix
             */
            template <Tag nmTag>
            void Invoke(Matrix &N, const Matrix &A)
            {
                if constexpr (nmTag == Tag::Cholesky)
                {
                    N = details::Cholesky(A);
                }
                else if constexpr (nmTag == Tag::SVD)
                {
                    N = details::SVD(A);
                }
                else
                {
                    static_assert(false, "invoke with invalid tag");
                }
            }
        }

        /**
         * @brief Solution information enumeration
         *
         * Provides status information about space resection solution:
         * - Success: Solution converged successfully
         * - NotConverged: Maximum iterations reached without convergence
         * - Failed: Solution failed due to numerical or input issues
         * - Unknown: Unknown solution status
         */
        enum class Info : size_t
        {
            Success,
            NotConverged,
            Failed,
            Unknown
        };
    }
}

namespace SpaceResection::Solve
{
    /**
     * @brief General space resection solving function
     *
     * Implements iterative least squares solution for space resection problem.
     * Algorithm workflow:
     * 1. Input validation and parameter initialization
     * 2. Iterative computation:
     *    a. Rotation matrix computation
     *    b. Coordinate transformation
     *    c. Residual calculation
     *    d. Coefficient matrix construction
     *    e. Correction solving
     *    f. Exterior orientation element update
     *    g. Convergence checking
     * 3. Accuracy assessment and result output
     *
     * Based on collinearity equation linearization with iterative approximation.
     * Suitable for various photogrammetric applications including aerial and close-range photogrammetry.
     *
     * @tparam ccTag Coefficient matrix calculation method
     * @tparam nmTag Normal equation solution method
     * @param internal Internal orientation elements describing camera internal geometry
     * @param photo Image coordinate matrix containing observed image plane coordinates
     * @param object Object coordinate matrix containing control point object space coordinates
     * @param max_loop Maximum iteration count to prevent infinite loops
     * @param threshold Convergence threshold for angular corrections in radians
     * @return SpaceResectionSolveResult Complete result with exterior orientation elements and accuracy assessment
     * @throws std::invalid_argument When input parameters don't meet requirements
     *
     * @note Control points should be evenly distributed around image plane, avoid clustering in center
     * @note Initial exterior orientation elements affect convergence, better with near-vertical photography
     * @note Default angular threshold 3.0e-5 radians ≈ 6.2 arc-seconds
     */
    template <
        CoefficientTag ccTag,
        NormalizationTag nmTag>
    SpaceResectionSolveResult GeneralSolve [[nodiscard]] (
        const InternalElements &internal,
        const Matrix &photo,
        const Matrix &object,
        size_t max_loop = 50,
        const double threshold = 3e-5)
    {
        if (!details::IsInputValid(photo, object))
        {
            throw std::invalid_argument("Arguments of SymSolve are invalid");
        }

        SpaceResectionSolveResult result{
            .external = details::InitExternalElements(internal, object),
            .info = Info::NotConverged};
        ExternalElements &external = result.external;

        while (max_loop-- > 0)
        {

            Matrix rotate = details::RotationMatrix(external);
            Matrix transformed_obj =
                details::TransformedObjectiveCoordinates(rotate, object, external);
            Matrix transformed_photo =
                details::TransformedPhotoCoordinates(internal, transformed_obj);
            Matrix residual =
                details::ResidualMatrix(photo, transformed_photo);

            Matrix coefficient(transformed_photo.rows() * 2, 6);
            details::CoefficientCalculator::Invoke<ccTag>(
                coefficient,
                {.rotate = rotate,
                 .transformed_obj = transformed_obj,
                 .transformed_photo = transformed_photo,
                 .external = external,
                 .internal = internal});

            Matrix correction = details::CorrectionMatrix(coefficient, residual);

            details::UpdateExternalElements(external, correction);

            if (details::IsExternalElementsConverged(correction, threshold))
            {
                Matrix N(6, 6);
                details::NormalizationMethod::Invoke<nmTag>(N, coefficient);
                details::CompleteResult(
                    result,
                    coefficient,
                    correction,
                    residual,
                    rotate,
                    photo,
                    N);
                result.info = Info::Success;
            }
        }

        return result;
    }

    /**
     * @brief Quick space resection solving function
     *
     * Simplified space resection using NoAngles coefficient calculation method.
     * Fastest computation but assumes perfectly vertical photography.
     * Suitable for preliminary results or ideal conditions.
     *
     * @tparam nTag Normal equation solution method, defaults to Cholesky
     * @param internal Internal orientation elements
     * @param photo Image coordinate matrix
     * @param object Object coordinate matrix
     * @param max_loop Maximum iteration count
     * @param threshold Convergence threshold
     * @return SpaceResectionSolveResult Space resection solution result
     */
    template <NormalizationTag nTag = NormalizationTag::Cholesky>
    SpaceResectionSolveResult QuickSolve [[nodiscard]] (
        const InternalElements &internal,
        const Matrix &photo,
        const Matrix &object,
        size_t max_loop = 50,
        const double threshold = 3e-5)
    {
        return GeneralSolve<CoefficientTag::NoAngles, nTag>(internal, photo, object, max_loop, threshold);
    }

    /**
     * @brief Simplified space resection solving function
     *
     * Space resection using KappaOnly coefficient calculation method.
     * Balanced approach considering only Kappa rotation angle.
     * Good compromise between accuracy and computational efficiency.
     *
     * @tparam nTag Normal equation solution method, defaults to Cholesky
     * @param internal Internal orientation elements
     * @param photo Image coordinate matrix
     * @param object Object coordinate matrix
     * @param max_loop Maximum iteration count
     * @param threshold Convergence threshold
     * @return SpaceResectionSolveResult Space resection solution result
     */
    template <NormalizationTag nTag = NormalizationTag::Cholesky>
    SpaceResectionSolveResult SimplifiedSolve [[nodiscard]] (
        const InternalElements &internal,
        const Matrix &photo,
        const Matrix &object,
        size_t max_loop = 50,
        const double threshold = 3e-5)
    {
        return GeneralSolve<CoefficientTag::KappaOnly, nTag>(internal, photo, object, max_loop, threshold);
    }

    /**
     * @brief Precise space resection solving function
     *
     * Space resection using FullAngles coefficient calculation method.
     * Most accurate approach considering all three rotation angles.
     * Suitable for arbitrary camera orientations and high-precision applications.
     *
     * @tparam nTag Normal equation solution method, defaults to Cholesky
     * @param internal Internal orientation elements
     * @param photo Image coordinate matrix
     * @param object Object coordinate matrix
     * @param max_loop Maximum iteration count
     * @param threshold Convergence threshold
     * @return SpaceResectionSolveResult Space resection solution result
     */
    template <NormalizationTag nTag = NormalizationTag::Cholesky>
    SpaceResectionSolveResult PreciseSolve [[nodiscard]] (
        const InternalElements &internal,
        const Matrix &photo,
        const Matrix &object,
        size_t max_loop = 50,
        const double threshold = 3e-5)
    {
        return GeneralSolve<CoefficientTag::FullAngles, nTag>(internal, photo, object, max_loop, threshold);
    }
}

namespace SpaceResection::Print
{
    /**
     * @brief Formats and outputs matrix with descriptive message
     *
     * Outputs Eigen matrix in readable format with borders and custom message.
     * Primarily used for debugging and result presentation.
     *
     * @param m Matrix to output
     * @param msg Descriptive message preceding matrix output
     * @param fmt Matrix output format controlling precision, separators, etc.
     * @param os Output stream, defaults to standard output
     */
    void Matrix(const SpaceResection::Matrix &m, const std::string msg, const Eigen::IOFormat &fmt, std::ostream &os)
    {
        os << msg << "\n"
           << m.format(fmt) << std::endl;
    }

    /**
     * @brief Formats and outputs exterior orientation elements
     *
     * Outputs six exterior orientation parameters in tabular format:
     * - Projection center coordinates Xs, Ys, Zs
     * - Three rotation angles Phi, Omega, Kappa
     *
     * @param external Exterior orientation elements to output
     * @param os Output stream, defaults to standard output
     */
    void ExternalElements(const SpaceResection::ExternalElements &external, std::ostream &os)
    {
        os << std::vformat(
            "Xs : {}\nYs : {}\nZs : {}\nPhi : {}\nOmega : {}\nKappa : {}\n",
            std::make_format_args(external.Xs, external.Ys, external.Zs, external.Phi, external.Omega, external.Kappa));
    }

    /**
     * @brief Formats and outputs internal orientation elements
     *
     * Outputs four internal orientation parameters:
     * - Focal length f
     * - Scale denominator m
     * - Principal point coordinates x0, y0
     *
     * @param internal Internal orientation elements to output
     * @param os Output stream, defaults to standard output
     */
    void InternalElements(const SpaceResection::InternalElements &internal, std::ostream &os)
    {
        os << std::vformat(
            "f : {}\nm : {}\nx0 : {}\ny0 : {}\n",
            std::make_format_args(internal.f, internal.m, internal.x0, internal.y0));
    }

    /**
     * @brief Formats and outputs complete space resection result
     *
     * Outputs all significant space resection computation results:
     * - Exterior orientation elements
     * - Adjusted image coordinates
     * - Rotation matrix
     * - Accuracy assessment parameters
     * - Unit weight mean square error
     *
     * Results presented in formatted tables for easy reading and analysis.
     *
     * @param result Space resection solution result to output
     * @param os Output stream, defaults to standard output
     */
    void Result(const SpaceResectionSolveResult &result, std::ostream &os)
    {
        std::println(os, "{:=^50}", " SpaceResectionSolveResult ");
        ExternalElements(result.external, os);
        Matrix(result.photo, "new photo");
        Matrix(result.rotate, "final rotate");
        Matrix(result.sigma, "error matrix");
        os << std::vformat("medial error: {}\n", std::make_format_args(result.m0)) << std::flush;
    }
}

namespace SpaceResection::IO
{
    namespace details::in
    {
        /**
         * @brief Splits line and casts tokens to specified type
         *
         * Parses input line using specified separator and converts tokens to target type.
         * Handles whitespace trimming and character conversion errors.
         *
         * @tparam valT Target value type for conversion
         * @tparam col Number of expected columns
         * @param line Input line to parse
         * @param sep Separator character, defaults to comma
         * @return std::array<valT, col> Array of converted values
         * @throws std::runtime_error When token conversion fails
         */
        template <typename valT, size_t col>
        std::array<valT, col> SplitLineThenCast(std::string &line, const std::string sep)
        {
            std::array<valT, col> arr{};

            auto split_view = line | std::views::split(sep);

            size_t count = 0;
            for (auto token_view : split_view)
            {
                if (count == col)
                    break;

                std::string token(token_view.begin(), token_view.end());

                size_t start = token.find_first_not_of(" \t\r\n");
                size_t end = token.find_last_not_of(" \t\r\n");
                size_t len = end - start + 1;
                if (start != std::string::npos)
                {
                    token = token.substr(start, len);
                }
                else
                {
                    token = "";
                }

                auto cstr = token.c_str();
                auto res = std::from_chars(cstr, cstr + len, arr.at(count));
                if (res.ec != std::errc{})
                {
                    throw std::runtime_error("bad token");
                }

                ++count;
            }

            return arr;
        }
    }

    /**
     * @brief Data reading tag enumeration
     *
     * Defines expected column counts for different data types:
     * - Photo: 2 columns (x,y image coordinates)
     * - Object: 3 columns (X,Y,Z object coordinates)
     * - Adjacent: 5 columns (combined photo and object coordinates)
     */
    enum class ReadTag : size_t
    {
        Photo = 2,
        Object = 3,
        Adjacent = 5
    };

    /**
     * @brief Reads matrix data from input stream
     *
     * Parses input stream data into matrix format based on specified reading tag.
     * Supports different data formats through template specialization.
     *
     * @tparam rT Reading tag specifying expected column format
     * @param is Input stream to read from
     * @param sep Separator character for parsing, defaults to comma
     * @return Matrix Parsed matrix data
     */
    template <ReadTag rT>
    Matrix Read(std::istream &is, const std::string sep)
    {
        constexpr size_t col = static_cast<size_t>(rT);
        details::in::row_dynamic_matrix<double, col> rdm{};
        {
            for (std::string line; std::getline(is, line);)
            {
                if (line.empty())
                    continue;
                rdm.push_back(details::in::SplitLineThenCast<double, col>(line, sep));
            }
        }
        auto r = rdm.size(), c = col;
        Matrix m(r, c);

        for (auto ri = 0; ri != r; ++ri)
        {
            for (auto ci = 0; ci != c; ++ci)
            {
                m(ri, ci) = rdm.at(ri).at(ci);
            }
        }

        return m;
    }
}

namespace SpaceResection::Solve::Utils
{
    /**
     * @brief Matrix layout enumeration for data reading
     *
     * Defines different data organization layouts:
     * - PhotoOnly: Only photo coordinates (2 columns)
     * - ObjectOnly: Only object coordinates (3 columns)
     * - PhotoLeft: Combined data with photo coordinates in left 2 columns
     * - PhotoRight: Combined data with photo coordinates in right 2 columns
     */
    enum class MatrixLayout : size_t
    {
        PhotoOnly,
        ObjectOnly,
        PhotoLeft,
        PhotoRight
    };

    /**
     * @brief Reads matrix data from file with specified layout
     *
     * Reads and parses matrix data from file path with automatic layout handling.
     * Validates file existence and delegates to stream-based reading.
     *
     * @tparam layout Matrix layout specification
     * @param path File path to read from
     * @param sep Separator character for parsing
     * @return auto Tuple containing parsed matrices based on layout
     * @throws std::invalid_argument When file path doesn't exist
     */
    template <MatrixLayout layout>
    auto ReadMatrix(const std::string path, std::string sep)
    {
        namespace fs = std::filesystem;
        if (!fs::exists(path))
        {
            throw std::invalid_argument("input filepath not found");
        }

        std::ifstream ifs{path};

        return ReadMatrix<layout>(ifs, sep);
    }

    /**
     * @brief Reads matrix data from stream with specified layout
     *
     * Reads and parses matrix data from input stream with layout-based processing.
     * Automatically splits combined data into photo and object coordinate matrices.
     *
     * @tparam layout Matrix layout specification
     * @param is Input stream to read from
     * @param sep Separator character for parsing
     * @return auto Tuple containing parsed matrices based on layout
     */
    template <MatrixLayout layout>
    auto ReadMatrix(std::istream &is, std::string sep)
    {
        if constexpr (layout == MatrixLayout::PhotoOnly)
        {
            return std::make_tuple(Read<ReadTag::Photo>(is, sep));
        }
        else if constexpr (layout == MatrixLayout::ObjectOnly)
        {
            return std::make_tuple(Read<ReadTag::Object>(is, sep));
        }
        else if constexpr (layout == MatrixLayout::PhotoLeft)
        {
            auto adj = Read<ReadTag::Adjacent>(is, sep);
            auto photo = adj.leftCols(2).eval(),
                 obj = adj.rightCols(3).eval();
            return std::make_tuple(photo, obj);
        }
        else if constexpr (layout == MatrixLayout::PhotoRight)
        {
            auto adj = Read<ReadTag::Adjacent>(is, sep);
            auto photo = adj.rightCols(2).eval(),
                 obj = adj.leftCols(3).eval();
            return std::make_tuple(obj, photo);
        }
        else
        {
            static_assert(false, "invalid tag");
        }
    }
}

AGTB_END

#endif