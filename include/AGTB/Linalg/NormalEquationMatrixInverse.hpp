#ifndef __AGTB_LINALG_NORMAL_EQUATION_MATRIX_INVERSE_HPP__
#define __AGTB_LINALG_NORMAL_EQUATION_MATRIX_INVERSE_HPP__

#include "Base.hpp"

AGTB_LINALG_BEGIN

namespace NormalEquationMatrixInverseSolve
{
    template <LinalgOption opt>
    struct Impl
    {
        static Matrix Invoke(const Matrix &A)
        {
            AGTB_NOT_IMPLEMENT();
        }
    };

    template <>
    struct Impl<LinalgOption::Cholesky>
    {
        static Matrix Invoke(const Matrix &A)
        {
            Matrix AtA = A.transpose() * A;
            Eigen::LLT<Matrix> llt(AtA);
            if (llt.info() == Eigen::Success)
            {
                Matrix invAtA = llt.solve(Matrix::Identity(AtA.rows(), AtA.cols()));
                return invAtA;
            }
            else
            {
                Eigen::LDLT<Matrix> ldlt(AtA);
                Matrix invAtA = ldlt.solve(Matrix::Identity(AtA.rows(), AtA.cols()));
                return invAtA;
            }
        }
    };

    template <>
    struct Impl<LinalgOption::SVD>
    {
        static Matrix Invoke(const Matrix &A)
        {
            Matrix AtA = A.transpose() * A;
            Eigen::JacobiSVD<Matrix> svd(AtA, Eigen::ComputeFullU | Eigen::ComputeFullV);
            const Eigen::VectorXd &singular_values = svd.singularValues();

            double tolerance = singular_values(0) * std::max(AtA.rows(), AtA.cols()) * 1e-12;

            Eigen::VectorXd inv_singular_values(singular_values.size());
            for (int i = 0; i < singular_values.size(); ++i)
            {
                inv_singular_values(i) = (singular_values(i) > tolerance) ? 1.0 / singular_values(i) : 0.0;
            }

            Matrix invAtA = svd.matrixV() *
                            inv_singular_values.asDiagonal() *
                            svd.matrixU().transpose();
            return invAtA;
        }
    };
}

template <LinalgOption opt>
    requires InvokerConcept<NormalEquationMatrixInverseSolve::Impl<opt>, Matrix, const Matrix &>
using NormalEquationMatrixInverseSolver = NormalEquationMatrixInverseSolve::Impl<opt>;

AGTB_LINALG_END

#endif