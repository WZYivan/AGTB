#include <AGTB/Geodesy/PrincipleCurvatureRadii.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>

namespace ag = AGTB::Geodesy;
namespace age = ag::Ellipsoid;
namespace au = AGTB::Utils;

template <ag::EllipsoidConcept e>
void print_MN(ag::Latitude B)
{
    auto [M, N] = ag::PrincipleCurvatureRadii<e, ag::EllipsoidBasedOption::General>(B);
    std::println("B(90 deg):\nM={}\nN={}\na={}\nc={}", M, N, e::a, e::c);
}

int main()
{
    ag::Latitude B = au::Angles::FromDMS(90);
    print_MN<age::Krasovski>(B);
}