#ifndef __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__
#define __AGTB_PHOTOGRAMMETRY_ANALYTICAL_SPACE_INTERSECTION__

#include "../Base.hpp"
#include "../../Linalg/RotationMatrix.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace Analytical::DoubleImage::SpaceIntersection
{

    struct SpaceIntersectionParam
    {
        ExteriorOrientationElements
            ext1,
            ext2;
        double x1, y1, f1, x2, y2, f2;
    };

    struct SpaceIntersectionResult
    {
        double X, Y, Z;
    };

    SpaceIntersectionResult Solve(const SpaceIntersectionParam &p)
    {
        Matrix image_space_coord_1(3, 1), image_space_coord_2(3, 1);
        image_space_coord_1 << p.x1, p.y1, -p.f1;
        image_space_coord_2 << p.x2, p.y2, -p.f2;
        Matrix
            image_space_aux_1 = p.ext1.RotationMatrix_YXZ_CN() * image_space_coord_1,
            image_space_aux_2 = p.ext2.RotationMatrix_YXZ_CN() * image_space_coord_2;
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

AGTB_PHOTOGRAMMETRY_END

#endif