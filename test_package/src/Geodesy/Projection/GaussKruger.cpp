#include "AGTB/Geodesy/Projection/GaussKruger.hpp"
#include "AGTB/Utils/Angles.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <print>

bool areAlmostEqual(double a, double b, double epsilon = 1e-10)
{
    return std::abs(a - b) < epsilon;
}

void GK_Zone()
{
    using namespace AGTB::Geodesy::Projection::GaussKruger;
    using namespace AGTB::Utils::Angles;
    using namespace AGTB::Geodesy;

    std::println("{} : {}", 3, Zone<ZoneInterval::D6>(FromDMS(3)));
    std::println("{} : {}", 10, Zone<ZoneInterval::D6>(FromDMS(10)));

    GeodeticLongitude l3(FromDMS(3)), l10(FromDMS(10));

    std::println("{} : {}", 3, Angle::FromRad(CenterLongitude<ZoneInterval::D6>(l3)).ToString());
    std::println("{} : {}", 10, Angle::FromRad(CenterLongitude<ZoneInterval::D6>(l10)).ToString());
}

int main()
{
    // GK_Zone();

    namespace ag = AGTB::Geodesy;
    namespace agpg = ag::Projection::GaussKruger;
    namespace au = AGTB::Utils;
    namespace aua = au::Angles;

    using projector = agpg::Projector<ag::Ellipsoid::CGCS2000, ag::EllipsoidBasedOption::General>;
    using aua::Angle;

    auto rf = projector::Forward(
        aua::FromDMS(114, 0),
        aua::FromDMS(30, 0));

    std::println("x = {}\ny = {}\nZoneY = {}\n", rf.x, rf.y, rf.ZoneY());

    auto ri = projector::Inverse(rf.x, rf.y, rf.zone);

    std::println("\nB = {}\nL= {}", Angle::FromRad(ri.B).ToString(), Angle::FromRad(ri.L).ToString());
}