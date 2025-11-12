#include <AGTB/Geodesy_V2/Ellipsoid/Geometry.hpp>
#include <print>

namespace ag = AGTB::Geodesy;
namespace au = AGTB::Utils;

int main()
{
    ag::Latitude B(au::Angles::FromDMS(30));
    ag::CurvatureRadiusCollection rc =
        ag::PrincipleCurvatureRadii<ag::EllipsoidType::Krasovski>(B);
    std::println("M = {}, N = {}, R = {}", rc.M, rc.N, rc.R());
}