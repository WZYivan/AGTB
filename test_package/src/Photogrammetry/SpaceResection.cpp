#include <AGTB/Photogrammetry/SpaceResection.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <sstream>

namespace aei = AGTB::EigenIO;
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
    aei::ReadEigen(iss, all);
    photo = all.leftCols(2),
    obj = all.rightCols(3);

    aei::PrintEigen(photo, "photo"); // mm
    aei::PrintEigen(obj, "obj");
    photo /= 1000; // mm -> m

    ap::InteriorOrientationElements internal{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000,
        .m = 50000};

    ap::SpaceResectionParam p = {
        .interior = internal,
        .photo = std::move(photo),
        .object = std::move(obj)};
    auto result = ap::Solve(p);
    if (result.info == ap::IterativeSolutionInfo::Success)
    {
        std::println(std::cout, "{}", result.ToString());
    }
    else
    {
        throw std::runtime_error("QuickSolve failed");
    }
}