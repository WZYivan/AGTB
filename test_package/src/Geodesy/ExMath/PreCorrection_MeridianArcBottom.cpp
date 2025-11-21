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
    // std::string fname = std::format("../dat/meridian_arc/meridian_arc_bottom_residual_len_div_1e7_{}_tc.dat", name);
    std::string fname = std::format("../dat/meridian_arc/meridian_arc_bottom_residual_len_div_1e7_{}_oc.dat", name);
    // std::string fname = std::format("../dat/meridian_arc/meridian_arc_bottom_residual_len_div_1e7_{}.dat", name);
    std::ofstream ofs{fname, std::ios::out};

    for (int i = 1; i != 90; ++i)
    {
        using ag::MeridianArcBottom;
        using ag::MeridianArcLength;

        ag::Latitude lat(i, 0, 0);
        double len = MeridianArcLength<__ellipsoid_type>(lat);
        ag::Latitude B = MeridianArcBottom<__ellipsoid_type>(len, 1e-5);

        ofs << std::format("{} {}\n", len / 1e7, aua::ToSeconds(lat.Rad() - B.Rad()));
    }
}

int main()
{
    generate_gauss_project_latitude_residual<ag::EllipsoidType::CGCS2000>();
    generate_gauss_project_latitude_residual<ag::EllipsoidType::IE1975>();
    generate_gauss_project_latitude_residual<ag::EllipsoidType::WGS84>();
    generate_gauss_project_latitude_residual<ag::EllipsoidType::Krasovski>();
}