#include <AGTB/Photographic/SpaceResection.hpp>
#include <sstream>

int main()
{
    namespace apss = AGTB::Photographic::SpaceResection::Solve;

    std::istringstream iss{
        "-86.15, -68.99,  36589.41, 25273.32, 2195.17\n"
        "-53.40, 82.21 ,  37631.08, 31324.51, 728.69 \n"
        "-14.78, -76.63,  39100.97, 24934.98, 2386.50\n"
        "10.46 , 64.43 ,  40426.54, 30319.81, 757.31 \n"};
    auto [photo, obj] = apss::ReadMatrix<apss::MatrixLayout::PhotoLeft>(iss);
    apss::Print::Matrix(photo, "photo"); // mm
    apss::Print::Matrix(obj, "obj");
    photo /= 1000; // mm -> m

    apss::InternalElements internal{
        .x0 = 0,
        .y0 = 0,
        .f = 153.24 / 1000,
        .m = 50000};
    auto result = apss::QuickSolve<apss::NormalizationTag::SVD>(internal, photo, obj);
    if (result.info == apss::Info::Success)
    {
        apss::Print::Result(result); // photo in (mm)
    }
    else
    {
        throw std::runtime_error("QuickSolve failed");
    }
}