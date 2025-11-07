#ifndef __AGTB_LINALG_CORRECTION_OLS_SOLVE_HPP__
#define __AGTB_LINALG_CORRECTION_OLS_SOLVE_HPP__

#include "Base.hpp"

AGTB_LINALG_BEGIN

Matrix CorrectionOlsSolve(const Matrix &A, const Matrix &L)
{
    return A.colPivHouseholderQr().solve(L);
}

AGTB_LINALG_END

#endif