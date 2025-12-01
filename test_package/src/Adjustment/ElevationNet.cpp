#include <AGTB/Adjustment/ElevationNet.hpp>
#include <AGTB/IO/Eigen.hpp>

#include <print>

using namespace AGTB::Adjustment;
using AGTB::IO::PrintEigen;
using Elevation::ParseElevationNetParam;
using Elevation::SolveParsedParam;

int main()
{
    ElevationParam<RouteType::Net> param1{
        {{"h1", "A", "P1", 1.003, 1},
         {"h2", "P1", "P2", 0.501, 2},
         {"h3", "C", "P2", 0.503, 2},
         {"h4", "B", "P1", 0.505, 1}},
        {{"A", 11.000},
         {"B", 11.500},
         {"C", 12.008}}};

    ElevationParam<RouteType::Net> param2{
        {{"1", "A", "B", 5.835, 3.5},
         {"2", "B", "C", 3.782, 2.7},
         {"3", "A", "C", 9.640, 4.0},
         {"4", "D", "C", 7.384, 3.0},
         {"5", "A", "D", 2.270, 2.5}},
        {{"A", 237.483}},
        10.0};

    auto res = Adjust(param2);

    for (auto &[k, v] : res.section)
    {
        std::println("{}, {}", k, v);
    }

    for (auto &[k, v] : res.station)
    {
        std::println("{}, {}", k, v);
    }
}
