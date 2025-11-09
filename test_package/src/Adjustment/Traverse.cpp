#include <AGTB/Adjustment/Traverse.hpp>
#include <AGTB/Utils/Angles.hpp> // for class `Angle`
#include <print>

namespace aa = AGTB::Adjustment;
using AGTB::Utils::Angles::Angle;

int main()
{
    aa::TraverseParam<aa::RouteType::Closed> p1{
        .distances = {105.22, 80.18, 129.34, 78.16},
        .angles = {{107, 48, 30}, {73, 0, 24}, {89, 33, 48}, {89, 36, 30}},
        .azi_beg = {125, 30, 0},
        .x_beg = 506.32,
        .y_beg = 215.65};
    auto r1 = aa::Adjust(p1, 2);
    std::println(">>> Closed:\n{}", aa::TraverseAdjustTable(p1, r1));

    aa::TraverseParam<aa::RouteType::Connecting> p2{
        .distances = {225.85, 139.03, 172.57, 100.07, 102.48},
        .angles = {{99, 1, 0.0}, Angle(167, 45, 36), {123, 11, 24}, Angle(189, 20, 36), Angle(179, 59, 18), {129, 27, 24}},
        .azi_beg = Angle(237, 59, 30),
        .azi_end = Angle(46, 45, 24),
        .x_beg = 2507.65,
        .y_beg = 1215.64,
        .x_end = 2166.70,
        .y_end = 1757.28};
    auto r2 = aa::Adjust(p2, 2);

    std::println(">>> Connecting:\n{}", aa::TraverseAdjustTable(p2, r2));
}