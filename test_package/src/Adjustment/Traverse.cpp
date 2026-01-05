#include <AGTB/Adjustment/Traverse.hpp>
#include <AGTB/Adjustment/Tolerance.hpp>
#include <print>

namespace aa = AGTB::Adjustment;

int main()
{
    aa::TraverseParam<aa::RouteType::ClosedLoop> p1{
        .distances = {55.150, 66.521, 53.729, 66.661},
        .angles = {{90, 57, 35}, {89, 9, 46}, {92, 3, 38}, {87, 50, 2}},
        .azi_beg = {125, 30, 0},
        .x_beg = 500.00,
        .y_beg = 600.00};
    aa::TraverseAdjustResult r1 = aa::Adjust(p1, 3, 0);
    std::println("{}", aa::AdjustmentTable(p1, r1));
}
