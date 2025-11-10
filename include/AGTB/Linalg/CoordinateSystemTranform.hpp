#ifndef __AGTB_COORDINATE_SYSTEM_TRANSFORM_HPP__
#define __AGTB_COORDINATE_SYSTEM_TRANSFORM_HPP__

#include "Base.hpp"
#include "RotationMatrix.hpp"

AGTB_LINALG_BEGIN

/**
 * @brief AGTB internal macros to check input `xyz` size
 *
 */
#define __AGTB_CheckIsCoordinateMatrixValid(__XYZ)                                                  \
    if (__XYZ.rows() == 0 || __XYZ.cols() != 3)                                                     \
    {                                                                                               \
        AGTB_THROW(std::invalid_argument, "Input matrix must have 3 columns refer to X, Y and Z."); \
    }

/**
 * @brief Rotation matrix to rotate three axises in specified order
 *
 * @tparam ax1
 * @tparam ax2
 * @tparam ax3
 * @param a1
 * @param a2
 * @param a3
 * @return Matrix
 */
template <Axis ax1, Axis ax2, Axis ax3>
Matrix CsRotationMatrix(double a1, double a2, double a3)
{
    return RotateByAxis<ax1>(a1) * RotateByAxis<ax2>(a2) * RotateByAxis<ax3>(a3);
}

/**
 * @brief Transform coordinates by coordinate system translation
 *
 * @param XYZ
 * @param x
 * @param y
 * @param z
 * @return Matrix
 */
Matrix CsTranslate(const Matrix &XYZ, double x, double y, double z)
{
    __AGTB_CheckIsCoordinateMatrixValid(XYZ);

    Matrix translated(XYZ.rows(), XYZ.cols());

    translated.col(0) = XYZ.col(0).array() - x;
    translated.col(1) = XYZ.col(1).array() - y;
    translated.col(2) = XYZ.col(2).array() - z;

    return translated;
}

/**
 * @brief Transform coordinates by coordinate system rotation(inverse).
 *
 * @param XYZ in new coordinate system
 * @param rotate new -> ori system
 * @return Matrix
 */
Matrix CsRotateInverse(const Matrix &XYZ, const Matrix &rotate)
{
    __AGTB_CheckIsCoordinateMatrixValid(XYZ);
    // new = R.T * ori -> (X, Y, Z).T => XYZ * R
    // ori = R * new -> (X, Y, Z).T => XYZ * R.T <--------
    // ori -> (X, Y, Z).T
    return XYZ * rotate.transpose();
}

/**
 * @brief Transform coordinates by coordinate system rotation(forward).
 *
 * @param XYZ in ori system
 * @param rotate new -> ori system
 * @return Matrix
 */
Matrix CsRotateForward(const Matrix &XYZ, const Matrix &rotate)
{
    __AGTB_CheckIsCoordinateMatrixValid(XYZ);
    // new = R.T * ori -> (X, Y, Z).T => XYZ * R <-------
    // ori = R * new -> (X, Y, Z).T => XYZ * R.T
    // ori -> (X, Y, Z).T
    return XYZ * rotate;
}

/**
 * @brief Transform coordinates by coordinate system scale
 *
 * @param XYZ
 * @param scale
 * @return Matrix
 */
Matrix CsScale(const Matrix &XYZ, double scale)
{
    __AGTB_CheckIsCoordinateMatrixValid(XYZ);

    return (XYZ.array() * scale).matrix();
}

AGTB_LINALG_END

#endif