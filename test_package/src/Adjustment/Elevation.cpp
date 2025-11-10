#include <AGTB/Adjustment/Elevation.hpp>
#include <print>
namespace aa = AGTB::Adjustment;

int main()
{
    aa::ElevationParam<aa::RouteType::Closed> p1{
        .distances = {0.8, 0.5, 1.2, 0.5, 1.0},
        .h = {0.230, 0.260, -0.550, -0.450, 0.490},
        .H_beg = 12.000};
    aa::ElevationAdjustResult r1 = aa::Adjust(p1, 3);
    std::println(">>> Closed \n{}", aa::AdjustmentTable(p1, r1));

    aa::ElevationParam<aa::RouteType::Connecting> p2{
        .distances = {1.6, 2.1, 1.7, 2.0},
        .h = {2.331, 2.813, -2.224, 1.430},
        .H_beg = 45.286,
        .H_end = 49.579};
    aa::ElevationAdjustResult r2 = aa::Adjust(p2, 3);
    std::println(">>> Connecting \n{}", aa::AdjustmentTable(p2, r2));
}