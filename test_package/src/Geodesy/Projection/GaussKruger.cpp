#include <AGTB/Geodesy/Project.hpp>
#include <print>

int main()
{
    namespace ag = AGTB::Geodesy;
    namespace au = AGTB::Utils;
    using au::Angles::Angle;

    using Tp = ag::GaussProjectTParam<ag::EllipsoidType::CGCS2000, ag::GaussZoneInterval::D6>;

    for (int i = 1; i != 6; ++i)
    {
        ag::Longitude
            L_from(115, 0, 0);
        ag::Latitude
            B_from(15 * i, 0, 0);

        Tp::GeoCoord geo_coord_from{
            L_from,
            B_from};

        Tp::GaussCoord gauss_proj_coord =
            ag::Project<Tp>(geo_coord_from);

        Tp::GeoCoord geo_coord_to =
            ag::Project<Tp>(gauss_proj_coord);

        std::println("B_from = {} L_from = {}",
                     B_from.ToAngle().ToString(),
                     L_from.ToAngle().ToString());
        std::println("x = {} y = {} ZoneY = {}", gauss_proj_coord.x, gauss_proj_coord.y, gauss_proj_coord.ZoneY());
        std::println("B_to = {} L_to = {}",
                     geo_coord_to.B.ToAngle().ToString(),
                     geo_coord_to.L.ToAngle().ToString());
        std::println("B_dif = {} L_dif = {}\n",
                     Angle::FromRad(geo_coord_to.B.Rad() - B_from.Rad()).ToString(),
                     Angle::FromRad(geo_coord_to.L.Rad() - L_from.Rad()).ToString());
    }
}