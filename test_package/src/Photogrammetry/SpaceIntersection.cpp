#define AGTB_ENABLE_DEBUG

#include <AGTB/Photogrammetry.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>

namespace ap = AGTB::Photogrammetry;
namespace aio = AGTB::IO;
using AGTB::Utils::Angles::deg2rad;

int main()
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

    ap::Matrix image_left(1, 2), image_right(1, 2);
    image_left << -2.99493, 98.31321;
    image_right << 115.30009, 106.80757;

    ap::SpaceIntersection::BatchParam
        sibp_left{
            ex_left, in_left, image_left},
        sibp_right{
            ex_right, in_right, image_right};
    auto batch_result = ap::SpaceIntersection::Solve(sibp_left, sibp_right);
    aio::PrintEigen(batch_result, "Batch intersection");
}