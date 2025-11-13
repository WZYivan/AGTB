#include <AGTB/Geodesy/Ellipsoid/Geometry.hpp>
#include <print>

namespace ag = AGTB::Geodesy;
namespace au = AGTB::Utils;
using au::Angles::Angle;

int main()
{
    ag::Latitude B(au::Angles::FromDMS(45));
    ag::CurvatureRadiusCollection rc =
        ag::PrincipleCurvatureRadii<ag::EllipsoidType::Krasovski>(B);
    std::println("M = {}, N = {}, R = {}", rc.M, rc.N, rc.R());

    double len = ag::MeridianArcLength<ag::EllipsoidType::Krasovski>(B);
    ag::Latitude B_inv = ag::MeridianArcBottom<ag::EllipsoidType::Krasovski>(len, 1e-5);

    std::println("B_from = {}, len = {}, B_to = {}, B_delta = {}",
                 Angle::FromRad(B.Rad()).ToString(),
                 len,
                 Angle::FromRad(B_inv.Rad()).ToString(),
                 Angle::FromRad(B.Rad() - B_inv.Rad()).ToString());
}