#include <AGTB/Geodesy_V2/Projection/GaussKruger.hpp>

#include <print>

namespace ag = AGTB::Geodesy;

int main()
{
    using namespace ag::Projection::GaussKruger;

    std::println("center lon = {}", CenterLongitude<6.0>(
                                        ag::Longitude(
                                            AGTB::Utils::Angles::FromDMS(2.0)))
                                        .ToAngle()
                                        .ToString());
}