#ifndef __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__
#define __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__

#include "Base.hpp"
#include "../Linalg/RotationMatrix.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace SpaceIntersection
{
    struct SpaceIntersectionIsolatedParam
    {
        ExteriorOrientationElements ex;
        InteriorOrientationElements in;
        double x, y;
    };

    using SpaceIntersectionParam = SpaceIntersectionIsolatedParam;

    using SpaceIntersectionPairedParam = std::pair<SpaceIntersectionParam, SpaceIntersectionParam>;

    struct SpaceIntersectionResult
    {
        double X, Y, Z;
    };

    /**
     * @brief Space Intersection Algorithm. f and image coordinate must in same unit, such as mm or m.
     *
     * @param left
     * @param right
     * @return SpaceIntersectionResult [X, Y, Z]
     */
    SpaceIntersectionResult Solve(const SpaceIntersectionIsolatedParam &left, const SpaceIntersectionIsolatedParam &right)
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
#endif

        return {
            .X = X,
            .Y = Y,
            .Z = Z};
    }

    SpaceIntersectionResult Solve(const SpaceIntersectionPairedParam &pair)
    {
        return Solve(pair.first, pair.second);
    }
}

using SpaceIntersection::Solve;
using SpaceIntersection::SpaceIntersectionParam;
using SpaceIntersection::SpaceIntersectionResult;

AGTB_PHOTOGRAMMETRY_END

#endif