#ifndef __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__
#define __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__

#include "Base.hpp"
#include "../Linalg/RotationMatrix.hpp"
#include "SpaceResection.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace detail::SpaceIntersection
{
    using detail::SpaceResection::Equation;
    using detail::SpaceResection::ResidualMatrix;
    using detail::SpaceResection::Simplify;

    template <Simplify __simplify>
    Matrix SpaceIntersectionCoefficient(const Matrix &rotate, const Matrix &transformed_obj, const Matrix &transformed_photo, const ExteriorOrientationElements &external, const InteriorOrientationElements &internal)
    {
        using detail::SpaceResection::SpaceResectionCoefficient;

        return SpaceResectionCoefficient<__simplify>(rotate, transformed_obj, transformed_photo, external, internal).leftCols(3);
    }
}

/**
 * @brief All in one space intersection solver. Everything needed for algorithm is in scope.
 *
 */
struct SpaceIntersection
{
    using BatchResult = Matrix;
    struct BatchParam
    {
        ExteriorOrientationElements ex;
        InteriorOrientationElements in;
        Matrix image;
    };

    struct IsolatedParam
    {
        ExteriorOrientationElements ex;
        InteriorOrientationElements in;
        double x, y;
    };

    using Param = IsolatedParam;

    using PairedParam = std::pair<Param, Param>;

    struct Result
    {
        double X, Y, Z;
    };

    /**
     * @brief Space intersectin for double img
     *
     * @param left
     * @param right
     * @return Result
     */
    static Result Solve(const Param &left, const Param &right)
    {
        Matrix isp_left(1, 3), isp_right(1, 3);
        isp_left << left.x, left.y, -left.in.f;
        isp_right << right.x, right.y, -right.in.f;
        Matrix
            aux_left = Transform::Isp2Aux(isp_left, left.ex),
            aux_right = Transform::Isp2Aux(isp_right, right.ex);
        double Xs1 = left.ex.Xs, Xs2 = right.ex.Xs,
               Ys1 = left.ex.Ys, Ys2 = right.ex.Ys,
               Zs1 = left.ex.Zs, Zs2 = right.ex.Zs;
        double BX = Xs2 - Xs1,
               BY = Ys2 - Ys1,
               BZ = Zs2 - Zs1;
        double X1 = aux_left(0, 0), Y1 = aux_left(0, 1), Z1 = aux_left(0, 2),
               X2 = aux_right(0, 0), Y2 = aux_right(0, 1), Z2 = aux_right(0, 2);
        double N1 = (BX * Z2 - BZ * X2) / (X1 * Z2 - Z1 * X2),
               N2 = (BX * Z1 - BZ * X1) / (X1 * Z2 - Z1 * X2);
        double X = Xs1 + BX + N2 * X2,
               Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0,
               Z = Zs1 + BZ + N2 * Z2;

#if (AGTB_DEBUG)
        std::println("N1 = {}, N2 = {}", N1, N2);
        IO::PrintEigen(aux_left, "Aux 1");
        IO::PrintEigen(aux_right, "Aux 2");
#endif

        return {
            .X = X,
            .Y = Y,
            .Z = Z};
    }

    /**
     * @brief Batch process version of double img space intersection
     *
     * @param left
     * @param right
     * @return BatchResult
     */
    static BatchResult Solve(const BatchParam &left, const BatchParam &right)
    {
        if (left.image.cols() != 2 || right.image.cols() != 2)
        {
            AGTB_THROW(std::invalid_argument, std::format("Col size of image plane coordinate must be 2 but get [{}, {}]", left.image.cols(), right.image.cols()));
        }

        if (left.image.rows() != right.image.rows())
        {
            AGTB_THROW(std::invalid_argument, std::format("Row size of image plane coordinate must be same but get [{}, {}]", left.image.rows(), right.image.rows()));
        }

        Matrix
            isp_left = left.image,
            isp_right = right.image;

        isp_left.conservativeResize(Eigen::NoChange, 3);
        isp_left.col(2).fill(-left.in.f);
        isp_right.conservativeResize(Eigen::NoChange, 3);
        isp_right.col(2).fill(-right.in.f);

        // Matrix
        //     aux_left = left.ex.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * isp_left.transpose(),
        //     aux_right = right.ex.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * isp_right.transpose();
        Matrix
            aux_left = Transform::Isp2Aux(isp_left, left.ex),
            aux_right = Transform::Isp2Aux(isp_right, right.ex);
        double Xs1 = left.ex.Xs, Xs2 = right.ex.Xs,
               Ys1 = left.ex.Ys, Ys2 = right.ex.Ys,
               Zs1 = left.ex.Zs, Zs2 = right.ex.Zs;
        double BX = Xs2 - Xs1,
               BY = Ys2 - Ys1,
               BZ = Zs2 - Zs1;
        auto
            X1 = aux_left.col(0).array(),
            Y1 = aux_left.col(1).array(),
            Z1 = aux_left.col(2).array(),
            X2 = aux_right.col(0).array(),
            Y2 = aux_right.col(1).array(),
            Z2 = aux_right.col(2).array();
        auto
            N1 = (BX * Z2 - BZ * X2) / (X1 * Z2 - Z1 * X2),
            N2 = (BX * Z1 - BZ * X1) / (X1 * Z2 - Z1 * X2);
        auto X = Xs1 + BX + N2 * X2;
        auto Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0;
        auto Z = Zs1 + BZ + N2 * Z2;

#if (AGTB_DEBUG)
        IO::PrintEigen(aux_left, "Aux 1");
        IO::PrintEigen(aux_right, "Aux 2");
        IO::PrintEigen(N1.matrix(), "N1");
        IO::PrintEigen(N2.matrix(), "N2");
#endif

        BatchResult res(X.rows(), 3);
        res.col(0) << X.matrix();
        res.col(1) << Y.matrix();
        res.col(2) << Z.matrix();
        return res;
    }

#if (AGTB_EXP)
    constexpr static CollinearityEquationStyle equation_style = SpaceResection::equation_style;
    using Equation = SpaceResection::Equation;
    using InverseMethod = SpaceResection::InverseMethod;
    using Simplify = SpaceResection::Simplify;
    template <InverseMethod __inverse_method, Simplify __simplify>
    using Config = SpaceResection::Config<__inverse_method, __simplify>;

    template <InverseMethod __inverse_method, Simplify __simplify>
    static Result Solve(const Param &left, const Param &right, int max_loop = 50, double threshold = 3e-5)
    {
#if (AGTB_NOTE)
#warning "This may be wrong"
#endif
        Result xyz = Solve(left, right);

        while (max_loop-- > 0)
        {
            auto [c_left, r_left] = MakeCoefficientAndResidual<__simplify>(left, xyz);
            auto [c_right, r_right] = MakeCoefficientAndResidual<__simplify>(right, xyz);
            Matrix coeff(4, 3), residual(4, 1);
            coeff << c_left, c_right;
            residual << r_left, r_right;

            Matrix correction = Linalg::CorrectionOlsSolve(coeff, residual);
#if (AGTB_DEBUG)
            IO::PrintEigen(coeff, "coeff");
            IO::PrintEigen(residual, "residual");
            IO::PrintEigen(correction, "correction");
#endif
            xyz.X -= correction(0, 0);
            xyz.Y -= correction(1, 0);
            xyz.Z -= correction(2, 0);
#if (AGTB_DEBUG)
            IO::PrintEigen(XYZ2Mat(xyz), "XYZ");
#endif
            if (std::abs(correction(0, 0)) < threshold &&
                std::abs(correction(1, 0)) < threshold &&
                std::abs(correction(2, 0)) < threshold)
            {
                break;
            }
        }

        return xyz;
    }

    template <typename __config>
    static Result Solve(const Param &left, const Param &right, int max_loop = 50, double threshold = 3e-5)
    {
        return Solve<__config::inverse_method, __config::simplify>(left, right, max_loop, threshold);
    }

private:
    template <Simplify __simplify>
    static auto MakeCoefficientAndResidual(const Param &param, const Result &xyz)
    {
        using namespace detail::SpaceIntersection;

        const ExteriorOrientationElements &ex = param.ex;
        const InteriorOrientationElements &in = param.in;
        const Matrix rotate = Transform::Ex2YXZ(ex);
        const Matrix obj = Transform::XYZ2Mat31(xyz).transpose();
        const Matrix transformed_obj = Transform::Aux2Isp(Transform::Obj2Aux(obj, ex), rotate);
        const Matrix transformed_photo = Transform::Isp2Img(transformed_obj, internal);
        const Matrix coeff = SpaceIntersectionCoefficient<__simplify>(rotate, transformed_obj, transformed_photo, ex, in);
        const Matrix residual = ResidualMatrix(XY2Mat(param), transformed_photo);
#if (AGTB_DEBUG)
        IO::PrintEigen(Linalg::CsTranslate(obj, ex.Xs, ex.Ys, ex.Zs), "img aux coord");
        IO::PrintEigen(transformed_obj, "img sp coord");
        IO::PrintEigen(transformed_photo, "calc photo");
        IO::PrintEigen(Transform::XY2Mat21(param).transpose(), "photo");
#endif
        return std::make_tuple(coeff, residual);
    }

    static Matrix XYZ2Mat(const Result &xyz)
    {
        Matrix mat(1, 3);
        mat << xyz.X, xyz.Y, xyz.Z;
        return mat;
    }

    static Matrix XY2Mat(const Param &xy)
    {
        Matrix mat(1, 2);
        mat << xy.x, xy.y;
        return mat;
    }
#endif
};

AGTB_PHOTOGRAMMETRY_END

#endif