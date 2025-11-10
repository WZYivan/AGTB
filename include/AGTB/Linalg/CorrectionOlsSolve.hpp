#ifndef __AGTB_LINALG_CORRECTION_OLS_SOLVE_HPP__
#define __AGTB_LINALG_CORRECTION_OLS_SOLVE_HPP__

#include "Base.hpp"

AGTB_LINALG_BEGIN

/**
 * @brief To equation `Ax - L = V`, x = (A.T * A).Inverse * (A.T * L). Also equals to QR solution of Linear system (A, L).
 *
 * @param A
 * @param L
 * @return Matrix
 */
Matrix CorrectionOlsSolve(const Matrix &A, const Matrix &L)
{
    return A.colPivHouseholderQr().solve(L);
}

AGTB_LINALG_END

#endif