#define AGTB_ENABLE_DEBUG

#include <AGTB/Photogrammetry/SpaceResection.hpp>
#include <AGTB/Photogrammetry/SpaceIntersection.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <print>

namespace ap = AGTB::Photogrammetry;
namespace aio = AGTB::IO;

int main()
{
    double f = 165.37034;
    ap::ExteriorOrientationElements
        ex_left{-6911.42788, 4181.15686, 157.77319, 0.34831, -0.30914, 0.08136},
        ex_right{-6922.01146, 4203.66508, 151.62205, 0.38231, -0.33532, 0.08277};
    ap::InteriorOrientationElements
        in_left{.f = f},
        in_right{.f = f};
    ap::SpaceIntersectionParam
        sip_left{ex_left, in_left, -2.99493, 98.31321},
        sip_right{ex_right, in_right, 115.30009, 106.80757};
    auto [X, Y, Z] = ap::Solve(sip_left, sip_right);
    std::println("X = {}, Y = {}, Z = {}", X, Y, Z);
}