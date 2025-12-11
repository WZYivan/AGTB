#include <AGTB/Geodesy/Project.hpp>
#include <print>
#include <fstream>

int main()
{
    namespace ag = AGTB::Geodesy;
    namespace au = AGTB::Utils;
    using au::Angles::Angle;

    using projector = ag::Projector<ag::GeoCS::Geodetic, ag::ProjCS::GaussKruger>;
    using config = projector::Config<ag::Ellipsoids::CGCS2000, ag::GaussZoneInterval::D6, ag::Units::Radian>;
    using GeoCoord = config::geo_coord;
    using ProjCoord = config::proj_coord;

    for (int i = 1; i != 90; ++i)
    {
        ag::Longitude<config::unit>
            L_from(115, 0, 0);
        ag::Latitude<config::unit>
            B_from(i, 0, 0);

        GeoCoord geo_coord_from{
            L_from,
            B_from};

        ProjCoord gauss_proj_coord =
            projector::Project<config>(geo_coord_from);

        GeoCoord geo_coord_to =
            projector::Project<config>(gauss_proj_coord);

        // std::println("B_from = {} L_from = {}",
        //              B_from.ToAngle().ToString(),
        //              L_from.ToAngle().ToString());
        // std::println("x = {} y = {} ZoneY = {}", gauss_proj_coord.x, gauss_proj_coord.y, gauss_proj_coord.ZoneY());
        // std::println("B_to = {} L_to = {}",
        //              geo_coord_to.B.ToAngle().ToString(),
        //              geo_coord_to.L.ToAngle().ToString());
        // std::println("B_dif = {} L_dif = {}\n",
        //              Angle::FromRad(geo_coord_to.B.Rad() - B_from.Rad()).ToString(),
        //              Angle::FromRad(geo_coord_to.L.Rad() - L_from.Rad()).ToString());
    }

    using K_config = projector::Config<ag::Ellipsoids::Krasovski, ag::GaussZoneInterval::D3, ag::Units::Radian>;

    ag::Longitude<ag::Units::Radian> Lc(117, 0, 0);

    K_config::proj_coord src{
        .x = 1'944'359.609,
        .y = 240'455.456'3,
        .zone = ag::GaussProjZone<K_config::zone_interval>(Lc),
    };

    auto tar = projector::ReProjectTo<K_config>(src, src.zone + 1);

    std::println("X = {}, Y = {}, Zone = {}", src.x, src.y, src.zone);
    std::println("X = {}, Y = {}, Zone = {}", tar.x, tar.y, tar.zone);
}