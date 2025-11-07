#ifndef __AGTB_ADJUSTMENT_ERROR_MEASURE_HPP__
#define __AGTB_ADJUSTMENT_ERROR_MEASURE_HPP__

#include "Base.hpp"

#include "../Linalg/Base.hpp"

AGTB_ADJUSTMENT_BEGIN

double MeanRootSquareError(const Matrix &V, int n, int t)
{
    return ((V.transpose() * V) / (2 * n - t)).cwiseSqrt()(0);
}

Matrix ErrorMatrix(double rmse, const Matrix &N)
{
    return rmse * N.cwiseSqrt();
}

AGTB_ADJUSTMENT_END

#endif