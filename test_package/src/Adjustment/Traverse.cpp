// #include <AGTB/Adjustment/Traverse.hpp>
// #include <AGTB/Utils/Angles.hpp> // for class `Angle`
// #include <print>

// namespace aa = AGTB::Adjustment;
// using AGTB::Utils::Angles::Angle;

// int main()
// {
//     aa::TraverseParam<aa::RouteType::ClosedLoop> p1{
//         .distances = {105.22, 80.18, 129.34, 78.16},
//         .angles = {{107, 48, 32}, {73, 0, 24}, {89, 33, 48}, {89, 36, 30}},
//         .azi_beg = {125, 30, 0},
//         .x_beg = 506.32,
//         .y_beg = 215.65};
//     for (Angle &a : p1.angles)
//     {
//         a = a.TakePlace(0);
//     }
//     auto r1 = aa::Adjust(p1, 2, 0);
//     std::println(">>> ClosedLoop:\n{}", aa::AdjustmentTable(p1, r1));

//     aa::TraverseParam<aa::RouteType::Connecting> p2{
//         .distances = {225.85, 139.03, 172.57, 100.07, 102.48},
//         .angles = {{99, 1, 0.0}, Angle(167, 45, 36), {123, 11, 24}, Angle(189, 20, 36), Angle(179, 59, 18), {129, 27, 24}},
//         .azi_beg = Angle(237, 59, 30),
//         .azi_end = Angle(46, 45, 24),
//         .x_beg = 2507.65,
//         .y_beg = 1215.64,
//         .x_end = 2166.70,
//         .y_end = 1757.28};
//     for (Angle &a : p2.angles)
//     {
//         a = a.TakePlace(0);
//     }
//     auto r2 = aa::Adjust(p2, 2, 0);

//     std::println(">>> Connecting:\n{}", aa::AdjustmentTable(p2, r2));
// }

#include <AGTB/Adjustment/Traverse.hpp>
#include <AGTB/Adjustment/Tolerance.hpp>
#include <print>

namespace aa = AGTB::Adjustment;

int main()
{
    // 1115
    // aa::TraverseParam<aa::RouteType::ClosedConnecting> p{
    //     .distances = {63.698, 52.318, 65.584, 56.819, 52.990, 55.375},
    //     .angles = {{268, 29, 12}, {88, 5, 33}, {91, 53, 17}, {180, 40, 42}, {83, 52, 34}, {95, 38, 8}, {271, 22, 53}},
    //     .azi_beg = {90, 0, 0},
    //     .azi_end = {270, 0, 0},
    //     .x_beg = 500,
    //     .y_beg = 600,
    //     .x_end = 500,
    //     .y_end = 600};
    // aa::TraverseAdjustResult r = aa::Adjust(p, 3, 0);
    // std::println(">>> 1\n{}", aa::AdjustmentTable(p, r));

    // aa::TraverseParam<aa::RouteType::ClosedConnecting> p2{
    //     .distances = {63.698, 52.320, 65.573, 56.809, 52.976, 55.361},
    //     .angles = {{268, 29, 12}, {88, 5, 33}, {91, 51, 49}, {180, 39, 20}, {83, 51, 18}, {95, 36, 39}, {271, 22, 53}},
    //     .azi_beg = {90, 0, 0},
    //     .azi_end = {270, 0, 0},
    //     .x_beg = 500,
    //     .y_beg = 600,
    //     .x_end = 500,
    //     .y_end = 600};
    // aa::TraverseAdjustResult r2 = aa::Adjust(p2, 3, 0);
    // std::println(">>> 2\n{}", aa::AdjustmentTable(p2, r2));

    aa::TraverseParam<aa::RouteType::ClosedConnecting> p3{
        .distances = {63.698, 52.303, 65.584, 56.819, 52.990, 55.375},
        .angles = {{268, 29, 12}, {88, 5, 33}, {91, 53, 17}, {180, 40, 20}, {83, 52, 18}, {95, 36, 39}, {271, 22, 53}},
        .azi_beg = {90, 0, 0},
        .azi_end = {270, 0, 0},
        .x_beg = 500,
        .y_beg = 600,
        .x_end = 500,
        .y_end = 600};
    aa::TraverseAdjustResult r3 = aa::Adjust(p3, 3, 0);
    std::println(">>> 3\n{}", aa::AdjustmentTable(p3, r3));

    using Tp = aa::EvaluateToleranceTParam<aa::EvaluateTarget::EDT, aa::ToleranceCatagory::Level, 1>;
    Tp::Result er3 = aa::EvaluateTolerance<Tp>(r3);

    std::println(">>> e3\n K = {}\n f = {}\n", er3.K ? "OK" : "Bad", er3.f_bate ? "OK" : "Bad");
}
