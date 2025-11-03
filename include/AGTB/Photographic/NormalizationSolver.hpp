#ifndef AGTB_PHOTOGRAPHIC_NORMALIZATION_SOLVER_HPP
#define AGTB_PHOTOGRAPHIC_NORMALIZATION_SOLVER_HPP

#include "../details/Macros.hpp"
#include "Base.hpp"

AGTB_PHOTOGRAPHIC_BEGIN

enum class NormalizationMethod : size_t
{
    Cholesky,
    SVD
};

namespace NormalizationSolve
{
    namespace details
    {
        Matrix Cholesky(const Matrix &A)
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

        Matrix SVD(const Matrix &A)
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

    }

    template <NormalizationMethod mtd>
    void Invoke(Matrix &N, const Matrix &A)
    {
        if constexpr (mtd == NormalizationMethod::Cholesky)
        {
            N = details::Cholesky(A);
        }
        else if constexpr (mtd == NormalizationMethod::SVD)
        {
            N = details::SVD(A);
        }
        else
        {
            static_assert(false, "invoke with invalid tag");
        }
    }
};

AGTB_PHOTOGRAPHIC_END

#endif