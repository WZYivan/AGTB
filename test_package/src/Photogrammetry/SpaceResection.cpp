#define AGTB_ENABLE_DEBUG

#include <AGTB/Photogrammetry.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <sstream>

namespace aio = AGTB::IO;
namespace ap = AGTB::Photogrammetry;
namespace al = AGTB::Linalg;

int main()
{

    std::istringstream iss{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};

    ap::Matrix photo(4, 2), obj(4, 3), all(4, 5);
    aio::ReadEigen(iss, all);
    photo = all.leftCols(2),
    obj = all.rightCols(3);

    aio::PrintEigen(photo, "photo"); // mm
    aio::PrintEigen(obj, "obj");
    photo /= 1000; // mm -> m

    ap::InteriorOrientationElements interior{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000,
        .m = 50000};

    ap::SpaceResection::Param p = {
        .interior = interior,
        .image = std::move(photo),
        .object = std::move(obj)};

    // using config = ap::SpaceResection::Config<ap::SpaceResection::InverseMethod::Cholesky, ap::SpaceResection::Simplify::None>;
    // auto result = ap::SpaceResection::Solve<config>(p);
    ap::SpaceResection::Result result = ap::SpaceResection::Solve(p);

    if (result.info == ap::IterativeSolutionInfo::Success)
    {
        std::println(std::cout, "{}", result.ToString());
    }
    else
    {
        throw std::runtime_error("Solve failed");
    }

    auto img_clac = ap::Transform::Obj2Img(p.object, result.exterior, interior);
    aio::PrintEigen(img_clac * 1000, "Calculate Img (mm)");
}