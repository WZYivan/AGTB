#include <AGTB/Geodesy/Projection/GaussKruger.hpp>

#include <print>

namespace ag = AGTB::Geodesy;

int main()
{
    using namespace ag::Projection::GaussKruger;
    namespace au = AGTB::Utils;
    using au::Angles::Angle;
    using au::Angles::FromDMS;

    std::println("center lon = {}", CenterLongitude<GaussZoneInterval::D6>(
                                        ag::Longitude(
                                            AGTB::Utils::Angles::FromDMS(2.0)))
                                        .ToAngle()
                                        .ToString());

    using CGCS_G_Proj = Projector<ag::EllipsoidType::CGCS2000>;
    ag::Longitude
        L_from(FromDMS(115, 0));
    ag::Latitude
        B_from(FromDMS(45, 0));

    auto rf =
        CGCS_G_Proj::Forward<GaussZoneInterval::D6>(L_from, B_from);

    auto ri =
        CGCS_G_Proj::Inverse<GaussZoneInterval::D6>(rf.x, rf.y, rf.zone);

    std::println("B_from = {} L_from = {}\n",
                 B_from.ToAngle().ToString(),
                 L_from.ToAngle().ToString());
    std::println("x = {} y = {} ZoneY = {}\n", rf.x, rf.y, rf.ZoneY());
    std::println("B_to = {} L_to = {}\n",
                 ri.B.ToAngle().ToString(),
                 ri.L.ToAngle().ToString());
    std::println("B_dif = {} L_dif = {}\n",
                 Angle::FromRad(ri.B.Rad() - B_from.Rad()).ToString(),
                 Angle::FromRad(ri.L.Rad() - L_from.Rad()).ToString());
}