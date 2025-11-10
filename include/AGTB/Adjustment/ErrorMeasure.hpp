#ifndef __AGTB_ADJUSTMENT_ERROR_MEASURE_HPP__
#define __AGTB_ADJUSTMENT_ERROR_MEASURE_HPP__

#include "Base.hpp"

#include "../Linalg/Base.hpp"

AGTB_ADJUSTMENT_BEGIN

/**
 * @brief RMSE
 *
 * @param V corrections
 * @param n number of known variable
 * @param t number of unknown variable
 * @return double
 */
double MeanRootSquareError(const Matrix &V, int n, int t)
{
    return ((V.transpose() * V) / (n - t)).cwiseSqrt()(0);
}

/**
 * @brief
 *
 * @param rmse
 * @param N Matrix of normalization equation (A.T * A).Inv
 * @return Matrix
 */
Matrix ErrorMatrix(double rmse, const Matrix &N)
{
    return rmse * N.cwiseSqrt();
}

AGTB_ADJUSTMENT_END

#endif