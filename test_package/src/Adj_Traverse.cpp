#include <AGTB/Adjustment/Traverse.hpp>
#include <print>
namespace aa = AGTB::Adjustment;
namespace aat = aa::Traverse;
using aat::Angle;

template <typename vec>
void NL(const vec &v)
{
    for (auto &e : v)
    {
        std::println("{}", e);
    }
    std::println("{:=^50}", "");
}

void NL()
{
    std::println("{:=^50}", "");
}

int main()
{
    aat::TraverseParam p1{
        .shape = aat::TraverseShape::Closed,
        .distances = {105.22, 80.18, 129.34, 78.16},
        .angles = {Angle(107, 48, 30), Angle(73, 0, 24), Angle(89, 33, 48), Angle(89, 36, 30)},
        .azi_beg = Angle(125, 30, 0),
        .x_beg = 506.32,
        .y_beg = 215.65};
    aat::ClosedAdjustor ca(p1);
    ca.Solve(2);

    std::println(">>> Closed:\n{}", aat::SolveResultOf(ca));

    aat::TraverseParam p2{
        .shape = aat::TraverseShape::Attached,
        .distances = {225.85, 139.03, 172.57, 100.07, 102.48},
        .angles = {Angle(99, 1, 0), Angle(167, 45, 36), Angle(123, 11, 24), Angle(189, 20, 36), Angle(179, 59, 18), Angle(129, 27, 24)},
        .azi_beg = Angle(237, 59, 30),
        .azi_end = Angle(46, 45, 24),
        .x_beg = 2507.65,
        .y_beg = 1215.64,
        .x_end = 2166.70,
        .y_end = 1757.28};
    aat::AttachedAdjustor aa(p2);
    aa.Solve(2);

    std::println(">>> Attached:\n{}", aat::SolveResultOf(aa));
}