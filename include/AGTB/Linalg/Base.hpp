#ifndef __AGTB_LINALG_BASE_HPP__
#define __AGTB_LINALG_BASE_HPP__

#include "../details/Macros.hpp"

#include <Eigen/Dense>
#include <concepts>

AGTB_LINALG_BEGIN

/**
 * @brief Alias of Eigen::MatrixXd
 *
 */
using Matrix = Eigen::MatrixXd;

/**
 * @brief Options to control Linalg functions
 *
 */
enum class LinalgOption : size_t
{
    Cholesky,
    SVD
};

/**
 * @brief Represents axis
 *
 */
enum class Axis : size_t
{
    X,
    Y,
    Z
};

AGTB_LINALG_END

AGTB_BEGIN

using Linalg::LinalgOption;

AGTB_END

AGTB_LINALG_BEGIN

void FillNaN(Matrix &mat, double val)
{
    mat = mat.array().isNaN().select(val, mat.array());
}

AGTB_LINALG_END

#endif
