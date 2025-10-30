#include <AGTB/Geodesy/PrincipleCurvature.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>

namespace ag = AGTB::Geodesy;
namespace age = ag::Ellipsoid;
namespace au = AGTB::Utils;

template <ag::EllipsoidConcept e>
void print_MN(ag::GeodeticLatitude B)
{
    auto [M, N] = ag::PrincipleCurvatureRadii<e>(B);
    std::println("M={}\nN={}\n", M, N);
}

int main()
{
    ag::GeodeticLatitude B = au::Angles::FromDegrees(45);
    print_MN<age::Krasovski>(B);
}