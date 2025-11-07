#ifndef __AGTB_COORDINATE_SYSTEM_TRANSFORM_HPP__
#define __AGTB_COORDINATE_SYSTEM_TRANSFORM_HPP__

#include "Base.hpp"
#include "RotationMatrix.hpp"

AGTB_LINALG_BEGIN

#define CheckIsCoordinateMatrixValid(__XYZ)                                                         \
    if (__XYZ.rows() == 0 || __XYZ.cols() != 3)                                                     \
    {                                                                                               \
        AGTB_THROW(std::invalid_argument, "Input matrix must have 3 columns refer to X, Y and Z."); \
    }

template <Axis ax1, Axis ax2, Axis ax3>
Matrix CsRotationMatrix(double a1, double a2, double a3)
{
    return RotateByAxis<ax1>(a1) * RotateByAxis<ax2>(a2) * RotateByAxis<ax3>(a3);
}

Matrix CsTranslate(const Matrix &XYZ, double x, double y, double z)
{
    CheckIsCoordinateMatrixValid(XYZ);

    Matrix translated(XYZ.rows(), XYZ.cols());

    translated.col(0) = XYZ.col(0).array() - x;
    translated.col(1) = XYZ.col(1).array() - y;
    translated.col(2) = XYZ.col(2).array() - z;

    return translated;
}

Matrix CsRotateInverse(const Matrix &XYZ, const Matrix &rotate)
{
    CheckIsCoordinateMatrixValid(XYZ);
    // new = R * ori -> (X, Y, Z).T => XYZ * R.T
    // ori = R.T * new -> (X, Y, Z).T => XYZ * R
    // ori -> (X, Y, Z).T
    return XYZ * rotate;
}

Matrix CsRotateForward(const Matrix &XYZ, const Matrix &rotate)
{
    CheckIsCoordinateMatrixValid(XYZ);
    // new = R * ori -> (X, Y, Z).T => XYZ * R.T
    // ori = R.T * new -> (X, Y, Z).T => XYZ * R
    // ori -> (X, Y, Z).T
    return XYZ * rotate.transpose();
}

Matrix CsScale(const Matrix &XYZ, double scale)
{
    CheckIsCoordinateMatrixValid(XYZ);

    return (XYZ.array() * scale).matrix();
}

AGTB_LINALG_END

#endif