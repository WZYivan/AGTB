#include "AGTB/Geodesy/Projection/GaussKruger.hpp"
#include "AGTB/Utils/Angles.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <print>

bool areAlmostEqual(double a, double b, double epsilon = 1e-10)
{
    return std::abs(a - b) < epsilon;
}

void GK_Zone()
{
    using namespace AGTB::Geodesy::Projection::GaussKruger;
    using namespace AGTB::Utils::Angles;
    using namespace AGTB::Geodesy;

    std::cout << "Running Gauss-Kruger Projection Tests...\n";

    std::cout << "Testing ZoneInterval::D6...\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(0.0)) == 0);
    std::cout << "Zone D6 (0.0) = 0: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(1.49)) == 0);
    std::cout << "Zone D6 (1.49) = 0: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(1.51)) == 1);
    std::cout << "Zone D6 (1.51) = 1: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(3.0)) == 0);
    std::cout << "Zone D6 (3.0) = 0: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(4.49)) == 0);
    std::cout << "Zone D6 (4.49) = 0: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(4.51)) == 1);
    std::cout << "Zone D6 (4.51) = 1: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(6.0)) == 1);
    std::cout << "Zone D6 (6.0) = 1: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(7.49)) == 1);
    std::cout << "Zone D6 (7.49) = 1: OK\n";

    assert(Zone<ZoneInterval::D6>(FromDMS(7.51)) == 2);
    std::cout << "Zone D6 (7.51) = 2: OK\n";

    GeodeticLongitude cl1_d6 = CenterLongitude<ZoneInterval::D6>(FromDMS(0.1));
    assert(areAlmostEqual(ToDegrees(cl1_d6), -3.0));
    std::cout << "CenterLongitude D6 (for longitude 0.1 -> Zone 0) = -3.0: OK\n";

    GeodeticLongitude cl2_d6 = CenterLongitude<ZoneInterval::D6>(FromDMS(1.51));
    assert(areAlmostEqual(ToDegrees(cl2_d6), 3.0));
    std::cout << "CenterLongitude D6 (for longitude 1.51 -> Zone 1) = 3.0: OK\n";

    GeodeticLongitude cl3_d6 = CenterLongitude<ZoneInterval::D6>(FromDMS(7.51));
    assert(areAlmostEqual(ToDegrees(cl3_d6), 9.0));
    std::cout << "CenterLongitude D6 (for longitude 7.51 -> Zone 2) = 9.0: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D6>(1)), 3.0));
    std::cout << "CenterLongitude D6 (Zone 1) = 3.0: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D6>(2)), 9.0));
    std::cout << "CenterLongitude D6 (Zone 2) = 9.0: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D6>(0)), -3.0));
    std::cout << "CenterLongitude D6 (Zone 0) = -3.0: OK\n";

    std::cout << "\nTesting ZoneInterval::D3...\n";

    assert(Zone<ZoneInterval::D3>(FromDMS(0.0)) == 1);
    std::cout << "Zone D3 (0.0) = 1: OK\n";

    assert(Zone<ZoneInterval::D3>(FromDMS(1.5)) == 1);
    std::cout << "Zone D3 (1.5) = 1: OK\n";

    assert(Zone<ZoneInterval::D3>(FromDMS(3.0)) == 2);
    std::cout << "Zone D3 (3.0) = 2: OK\n";

    assert(Zone<ZoneInterval::D3>(FromDMS(123.45)) == 42);
    std::cout << "Zone D3 (123.45) = 42: OK\n";

    assert(Zone<ZoneInterval::D3>(FromDMS(-179.99)) == -58);
    std::cout << "Zone D3 (-179.99) = -58: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D3>(1)), 3.0));
    std::cout << "CenterLongitude D3 (Zone 1) = 3.0: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D3>(2)), 6.0));
    std::cout << "CenterLongitude D3 (Zone 2) = 6.0: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D3>(0)), 0.0));
    std::cout << "CenterLongitude D3 (Zone 0) = 0.0: OK\n";

    assert(areAlmostEqual(ToDegrees(CenterLongitude<ZoneInterval::D3>(-1)), -3.0));
    std::cout << "CenterLongitude D3 (Zone -1) = -3.0: OK\n";

    GeodeticLongitude cl1_d3 = CenterLongitude<ZoneInterval::D3>(FromDMS(1.49));
    assert(areAlmostEqual(ToDegrees(cl1_d3), 3.0));
    std::cout << "CenterLongitude D3 (for longitude 1.49 -> Zone 1) = 3.0: OK\n";

    GeodeticLongitude cl2_d3 = CenterLongitude<ZoneInterval::D3>(FromDMS(3.01));
    assert(areAlmostEqual(ToDegrees(cl2_d3), 6.0));
    std::cout << "CenterLongitude D3 (for longitude 3.01 -> Zone 2) = 6.0: OK\n";

    GeodeticLongitude cl3_d3 = CenterLongitude<ZoneInterval::D3>(FromDMS(-1.49));
    assert(areAlmostEqual(ToDegrees(cl3_d3), 3.0));
    std::cout << "CenterLongitude D3 (for longitude -1.49 -> Zone 1) = 3.0: OK\n";

    std::cout << "\nAll tests passed!\n";
}

int main()
{
    // GK_Zone();

    namespace ag = AGTB::Geodesy;
    namespace agpg = ag::Projection::GaussKruger;
    namespace au = AGTB::Utils;
    namespace aua = au::Angles;

    auto [x, y] = agpg::InverseSolver<ag::Ellipsoid::Krasovski, ag::EllipsoidBasedOption::General>::Invoke(0.0, 0.0);

    std::println("x = {}\ny = {}\n", x, y);
}