#ifndef __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__
#define __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__

#include "Base.hpp"
#include "../Linalg/RotationMatrix.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace SpaceIntersection
{

    struct SpaceIntersectionParam
    {
        ExteriorOrientationElements
            ext1,
            ext2;
        double x1, y1, f1, x2, y2, f2;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);
            std::format_to(sbb,
                           "{:=^100}\n"
                           "{:-^50}\n{}\n"
                           "{:-^50}\n{}\n"
                           "{:-^50}\n"
                           " x1 = {}\n y1 = {}\n f1 = {}\n"
                           "{:-^50}\n"
                           "x2 = {}\n y2 = {}\n f2 = {}\n",
                           " Space Intersection Param ",
                           " Exterior Orientation Elements <1> ", ext1.ToString(),
                           " Exterior Orientation Elements <2> ", ext2.ToString(),
                           " Image Coordinate and f <1> ", x1, y1, f1,
                           " Image Coordinate and f <2> ", x2, y2, f2);
            return sb;
        }
    };

    struct SpaceIntersectionResult
    {
        double X, Y, Z;

        std::string ToString() const noexcept
        {
            return std::format("{:=^100}\n"
                               " X = {}\n Y = {}\n Z = {}",
                               " Space Intersection Solve Result",
                               X, Y, Z);
        }
    };

    SpaceIntersectionResult Solve(const SpaceIntersectionParam &p)
    {
        Matrix image_space_coord_1(3, 1), image_space_coord_2(3, 1);
        image_space_coord_1 << p.x1, p.y1, -p.f1;
        image_space_coord_2 << p.x2, p.y2, -p.f2;
        Matrix
            image_space_aux_1 = p.ext1.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * image_space_coord_1,
            image_space_aux_2 = p.ext2.ToRotationMatrix<Linalg::Axis::Y, Linalg::Axis::X, Linalg::Axis::Z>() * image_space_coord_2;
        double Xs1 = p.ext1.Xs, Xs2 = p.ext2.Xs,
               Ys1 = p.ext1.Ys, Ys2 = p.ext2.Ys,
               Zs1 = p.ext1.Zs, Zs2 = p.ext2.Zs;
        double BX = Xs2 - Xs1,
               BY = Ys2 - Ys1,
               BZ = Zs2 - Zs1;
        auto &i1 = image_space_aux_1, i2 = image_space_aux_2;
        double X1 = i1(0, 0), Y1 = i1(1, 0), Z1 = i1(2, 0),
               X2 = i2(0, 0), Y2 = i1(1, 0), Z2 = i2(2, 0);
        double N1 = (BX * Z2 - BZ * X2) / (Xs1 * Z2 - Z1 * X2),
               N2 = (BX * Z1 - BZ * X1) / (Xs1 * Z2 - Z1 * X2);
        double X = Xs1 + BX + N2 * X2,
               Y = Ys1 + (N1 * Y1 + N2 * Y2 + BY) / 2.0,
               Z = Zs1 + BZ + N2 * Z2;
        return {
            .X = X,
            .Y = Y,
            .Z = Z};
    }
}

using SpaceIntersection::Solve;
using SpaceIntersection::SpaceIntersectionParam;
using SpaceIntersection::SpaceIntersectionResult;

AGTB_PHOTOGRAMMETRY_END

#endif