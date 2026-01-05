#define AGTB_ENABLE_DEBUG
#define AGTB_ENABLE_EXP

#include <AGTB/Photogrammetry.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>

namespace ap = AGTB::Photogrammetry;
namespace aio = AGTB::IO;
namespace al = AGTB::Linalg;
using AGTB::Utils::Angles::deg2rad;

int main()
{
    if constexpr (false)
    {
        double f = 165.37034;
        ap::ExteriorOrientationElements
            ex_left{-6911.42788, 4181.15686, 157.77319, 0.34831 * deg2rad, -0.30914 * deg2rad, 0.08136 * deg2rad},
            ex_right{-6922.01146, 4203.66508, 151.62205, 0.38231 * deg2rad, -0.33532 * deg2rad, 0.08277 * deg2rad};
        ap::InteriorOrientationElements
            in_left{.f = f},
            in_right{.f = f};
        ap::SpaceIntersection::Param
            sip_left{ex_left, in_left, -2.99493, 98.31321},
            sip_right{ex_right, in_right, 115.30009, 106.80757};
        auto [X, Y, Z] = ap::SpaceIntersection::Solve(sip_left, sip_right);
        std::println("X = {}, Y = {}, Z = {}", X, Y, Z);

        // ap::Matrix image_left(1, 2), image_right(1, 2);
        // image_left << -2.99493, 98.31321;
        // image_right << 115.30009, 106.80757;

        // ap::SpaceIntersection::BatchParam
        //     sibp_left{
        //         ex_left, in_left, image_left},
        //     sibp_right{
        //         ex_right, in_right, image_right};
        // auto batch_result = ap::SpaceIntersection::Solve(sibp_left, sibp_right);
        // aio::PrintEigen(batch_result, "Batch intersection");

        // using config = ap::SpaceIntersection::Config<ap::SpaceIntersection::InverseMethod::Cholesky, ap::SpaceIntersection::Simplify::None>;
        // sip_left.x / 1000;
        // sip_left.y / 1000;
        // sip_left.in.f / 1000;
        // sip_right.x / 1000;
        // sip_right.y / 1000;
        // sip_right.in.f / 1000;

        // auto [X2, Y2, Z2] = ap::SpaceIntersection::Solve<config>(sip_left, sip_right);
        // std::println("ols: X = {}, Y = {}, Z = {}", X2, Y2, Z2);

        aio::PrintEigen(
            ap::Transform::Obj2Img(ap::Transform::XYZ2Mat13(-6911.69498, 4203.22511, 136.95938), ex_left, in_left), "Img Calc");
    }

    if constexpr (true)
    {
        double f = 150;
        using solver = ap::SpaceIntersection;

        ap::ExteriorOrientationElements
            ex_left{4999.770168, 4999.728897, 2000.002353, 0.00021500, 0.02906441, 0.09524706},
            ex_right{5896.828551, 5070.244316, 2030.443250, 0.01443355, 0.04601826, 0.11046904};
        ap::InteriorOrientationElements
            in_left{.f = f},
            in_right{.f = f};
        ap::Matrix img(5, 4);
        img << 51.758, 80.555, -39.953, 78.463,
            14.618, -0.231, -76.006, 0.036,
            49.88, -0.782, -42.201, -1.022,
            86.14, -1.346, -7.706, -2.112,
            48.035, -79.962, -44.438, -79.736;
        solver::BatchParam
            sibp_left{ex_left, in_left, img.leftCols(2)},
            sibp_right{ex_right, in_right, img.rightCols(2)};
        solver::BatchResult result = solver::Solve(sibp_left, sibp_right);
        ap::Matrix img_left_inv = ap::Transform::Obj2Img(result, ex_left, in_left);
        ap::Matrix img_right_inv = ap::Transform::Obj2Img(result, ex_right, in_right);
        ap::Matrix img_inv(5, 4);
        img_inv.leftCols(2) = img_left_inv;
        img_inv.rightCols(2) = img_right_inv;

        aio::PrintEigen(img, "All img");
        aio::PrintEigen(result, "Batch result");
        aio::PrintEigen(img_inv, "All inv img");

        // solver::Param
        //     ols_left{ex_left, in_left, img(0, 0), img(0, 1)},
        //     ols_right{ex_right, in_right, img(0, 2), img(0, 3)};
        // solver::OlsResult ols_result = solver::OlsSolve(ols_left, ols_right);
        // std::println("Ols[ x = {}, y = {}, z = {}, m0 = {}]", ols_result.coord.X, ols_result.coord.Y, ols_result.coord.Z, ols_result.m0);
        // al::FillNaN(ols_result.sigma, 0.0);
        // aio::PrintEigen(ols_result.sigma, "Error mat");

        solver::Param
            ols_1{ex_left, in_left, img(0, 0), img(0, 1)},
            ols_2{ex_right, in_right, img(0, 2), img(0, 3)};
        solver::OlsResult ols_result = solver::Solve({{ols_1, ols_2}});
        std::println("Ols[ x = {}, y = {}, z = {}, m0 = {}]", ols_result.coord.X, ols_result.coord.Y, ols_result.coord.Z, ols_result.m0);
        al::FillNaN(ols_result.sigma, 0.0);
        aio::PrintEigen(ols_result.sigma, "Error mat");
    }
}