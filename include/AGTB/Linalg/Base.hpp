#ifndef __AGTB_LINALG_BASE_HPP__
#define __AGTB_LINALG_BASE_HPP__

#include "../details/Macros.hpp"

#include <Eigen/Dense>
#include <concepts>

AGTB_LINALG_BEGIN

using Matrix = Eigen::MatrixXd;

enum class LinalgOption
{
    Cholesky,
    SVD
};

AGTB_LINALG_END

#endif
