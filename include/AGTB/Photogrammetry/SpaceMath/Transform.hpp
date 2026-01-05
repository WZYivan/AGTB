#ifndef __AGTB_PHOTOGRAMMETRY_SAPCE_MATH_TRANSFORM__
#define __AGTB_PHOTOGRAMMETRY_SAPCE_MATH_TRANSFORM__

#include <functional>

#include "../Base.hpp"

AGTB_PHOTOGRAMMETRY_BEGIN

namespace Transform
{

    Matrix Ex2YXZ(const ExteriorOrientationElements &ex)
    {
        using Linalg::Axis;
        return ex.ToRotationMatrix<Axis::Y, Axis::X, Axis::Z>();
    }

    Matrix XY2Mat21(double x, double y)
    {
        Matrix mat(2, 1);
        mat << x, y;
        return mat;
    }

    Matrix XYZ2Mat31(double x, double y, double z)
    {
        Matrix mat(3, 1);
        mat << x, y, z;
        return mat;
    }

    Matrix XY2Mat12(double x, double y)
    {
        return XY2Mat21(x, y).transpose();
    }

    Matrix XYZ2Mat13(double x, double y, double z)
    {
        return XYZ2Mat31(x, y, z).transpose();
    }

    Matrix Obj2Aux(const Matrix &obj, const ExteriorOrientationElements &ex)
    {
        return Linalg::CsTranslate(obj, ex.Xs, ex.Ys, ex.Zs);
    }

    Matrix Aux2Isp(const Matrix &aux, const Matrix &rotate)
    {
        return Linalg::CsRotateForward(aux, rotate);
    }

    Matrix Aux2Isp(
        const Matrix &aux, const ExteriorOrientationElements &ex,
        std::function<Matrix(const ExteriorOrientationElements &)> calc_rotate = Ex2YXZ)
    {
        return Aux2Isp(aux, calc_rotate(ex));
    }

    Matrix Isp2Img(const Matrix &img_sp, const InteriorOrientationElements &in)
    {
        Matrix img(img_sp.rows(), 2);
        const auto &XBar = img_sp.col(0).array(),
                   &YBar = img_sp.col(1).array(),
                   &ZBar = img_sp.col(2).array();
        const double f = in.f;
        img.col(0) = -f * (XBar / ZBar).matrix(); // x
        img.col(1) = -f * (YBar / ZBar).matrix(); // y
        return img;
    }

    Matrix Obj2Img(
        const Matrix &obj,
        const ExteriorOrientationElements &ex,
        const InteriorOrientationElements &in,
        const Matrix &rotate)
    {
        return Isp2Img(Aux2Isp(Obj2Aux(obj, ex), rotate), in);
    }

    Matrix Obj2Img(
        const Matrix &obj,
        const ExteriorOrientationElements &ex,
        const InteriorOrientationElements &in,
        std::function<Matrix(const ExteriorOrientationElements &)> calc_rotate = Ex2YXZ)
    {
        return Isp2Img(Aux2Isp(Obj2Aux(obj, ex), calc_rotate(ex)), in);
    }

    Matrix Isp2Aux(const Matrix &isp, const Matrix &rotate)
    {
        return Linalg::CsRotateInverse(isp, rotate);
    }

    Matrix Isp2Aux(
        const Matrix &isp, const ExteriorOrientationElements &ex,
        std::function<Matrix(const ExteriorOrientationElements &)> calc_rotate = Ex2YXZ)
    {
        return Isp2Aux(isp, calc_rotate(ex));
    }

    Matrix Aux2Obj(const Matrix &aux, const ExteriorOrientationElements &ex)
    {
        return Linalg::CsTranslate(aux, -ex.Xs, -ex.Ys, -ex.Zs);
    }
}

AGTB_PHOTOGRAMMETRY_END

#endif