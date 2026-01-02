#ifndef __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__
#define __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__

#include "Base.hpp"
#include "../Linalg/RotationMatrix.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

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

    static Result Solve(const Param &left, const Param &right)
    {
        Matrix image_space_coord_1(3, 1), image_space_coord_2(3, 1);
        image_space_coord_1 << left.x, left.y, -left.in.f;
        image_space_coord_2 << right.x, right.y, -right.in.f;
        Matrix
            image_space_aux_1 = left.ex.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * image_space_coord_1,
            image_space_aux_2 = right.ex.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * image_space_coord_2;
        double Xs1 = left.ex.Xs, Xs2 = right.ex.Xs,
               Ys1 = left.ex.Ys, Ys2 = right.ex.Ys,
               Zs1 = left.ex.Zs, Zs2 = right.ex.Zs;
        double BX = Xs2 - Xs1,
               BY = Ys2 - Ys1,
               BZ = Zs2 - Zs1;
        auto &i1 = image_space_aux_1, i2 = image_space_aux_2;
        double X1 = i1(0, 0), Y1 = i1(1, 0), Z1 = i1(2, 0),
               X2 = i2(0, 0), Y2 = i1(1, 0), Z2 = i2(2, 0);
        double N1 = (BX * Z2 - BZ * X2) / (X1 * Z2 - Z1 * X2),
               N2 = (BX * Z1 - BZ * X1) / (X1 * Z2 - Z1 * X2);
        double X = Xs1 + BX + N2 * X2,
               Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0,
               Z = Zs1 + BZ + N2 * Z2;

#if (AGTB_DEBUG)
        std::println("N1 = {}, N2 = {}", N1, N2);
        IO::PrintEigen(image_space_aux_1, "Aux 1");
        IO::PrintEigen(image_space_aux_2, "Aux 2");
#endif

        return {
            .X = X,
            .Y = Y,
            .Z = Z};
    }

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
            image_left = left.image,
            image_right = right.image;

        image_left.conservativeResize(Eigen::NoChange, 3);
        image_left.col(2).fill(-left.in.f);
        image_right.conservativeResize(Eigen::NoChange, 3);
        image_right.col(2).fill(-right.in.f);

        Matrix
            image_space_aux_left = left.ex.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * image_left.transpose(),
            image_space_aux_right = right.ex.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * image_right.transpose();
        double Xs1 = left.ex.Xs, Xs2 = right.ex.Xs,
               Ys1 = left.ex.Ys, Ys2 = right.ex.Ys,
               Zs1 = left.ex.Zs, Zs2 = right.ex.Zs;
        double BX = Xs2 - Xs1,
               BY = Ys2 - Ys1,
               BZ = Zs2 - Zs1;
        auto
            X1 = image_space_aux_left.row(0).array(),
            Y1 = image_space_aux_left.row(1).array(),
            Z1 = image_space_aux_left.row(2).array(),
            X2 = image_space_aux_right.row(0).array(),
            Y2 = image_space_aux_right.row(1).array(),
            Z2 = image_space_aux_right.row(2).array();
        auto
            N1 = (BX * Z2 - BZ * X2) / (X1 * Z2 - Z1 * X2),
            N2 = (BX * Z1 - BZ * X1) / (X1 * Z2 - Z1 * X2);
        auto X = Xs1 + BX + N2 * X2;
        auto Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0;
        auto Z = Zs1 + BZ + N2 * Z2;

#if (AGTB_DEBUG)
        IO::PrintEigen(X1.matrix(), "X1");
        IO::PrintEigen(Y1.matrix(), "Y1");
        IO::PrintEigen(Z1.matrix(), "Z1");
        IO::PrintEigen(X2.matrix(), "X2");
        IO::PrintEigen(Y2.matrix(), "Y2");
        IO::PrintEigen(Z2.matrix(), "Z2");
        IO::PrintEigen(N1.matrix(), "N1");
        IO::PrintEigen(N2.matrix(), "N2");
#endif

        BatchResult res(X.rows(), 3);
        res.col(0) << X.matrix();
        res.col(1) << Y.matrix();
        res.col(2) << Z.matrix();
        return res;
    }
};

AGTB_PHOTOGRAMMETRY_END

#endif