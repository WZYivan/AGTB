#include <AGTB/Geodesy/Project.hpp>
#include <print>
#include <fstream>

namespace ag = AGTB::Geodesy;
namespace au = AGTB::Utils;
namespace aua = au::Angles;

template <ag::EllipsoidType __ellipsoid_type>
std::string to_string()
{
    if constexpr (__ellipsoid_type == ag::EllipsoidType::CGCS2000)
    {
        return "CGCS2000";
    }
    else if constexpr (__ellipsoid_type == ag::EllipsoidType::IE1975)
    {
        return "IE1975";
    }
    else if constexpr (__ellipsoid_type == ag::EllipsoidType::Krasovski)
    {
        return "Krasovski";
    }
    else if constexpr (__ellipsoid_type == ag::EllipsoidType::WGS84)
    {
        return "WGS84";
    }
    else
    {
        AGTB_STATIC_THROW("NONE ELLIPSOID");
    }
}

template <ag::EllipsoidType __ellipsoid_type>
void generate_gauss_project_latitude_residual()
{
    std::string name = to_string<__ellipsoid_type>();
    // std::string fname = std::format("../dat/gauss_kruger/gauss_kruger_bottom_residual_len_div_1e7_{}_tc.dat", name);
    // std::string fname = std::format("../dat/gauss_kruger/gauss_kruger_bottom_residual_len_div_1e7_{}_oc.dat", name);
    std::string fname = std::format("../dat/gauss_kruger/gauss_kruger_lat_residual_X_{}.dat", name);
    std::ofstream ofs{fname, std::ios::out};

    using Tp = ag::GaussProjectTParam<__ellipsoid_type, ag::GaussZoneInterval::D6>;

    for (int i = 1; i != 90; ++i)
    {
        using ag::MeridianArcBottom;
        using ag::MeridianArcLength;

        ag::Latitude lat(i, 0, 0);
        ag::Longitude lon(116, 0, 0);

        typename Tp::GeoCoord geo_coord_from{lon, lat};
        typename Tp::GaussCoord gauss_proj_coord = ag::Project<Tp>(geo_coord_from);
        typename Tp::GeoCoord geo_coord_to = ag::Project<Tp>(gauss_proj_coord);

        double lat_residual_sec = (lat.Rad() - geo_coord_to.B.Rad()) * aua::rad2sec;

        ofs << std::format("{} {}\n", gauss_proj_coord.x / 1e7, lat_residual_sec);
    }
}

int main()
{
    generate_gauss_project_latitude_residual<ag::EllipsoidType::CGCS2000>();
    generate_gauss_project_latitude_residual<ag::EllipsoidType::IE1975>();
    generate_gauss_project_latitude_residual<ag::EllipsoidType::WGS84>();
    generate_gauss_project_latitude_residual<ag::EllipsoidType::Krasovski>();
}