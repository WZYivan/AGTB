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
    aat::TraverseParam p{
        .distances = {105.22, 80.18, 129.34, 78.16},
        .angles = {Angle(107, 48, 30), Angle(73, 0, 24), Angle(89, 33, 48), Angle(89, 36, 30)},
        .azi_beg = Angle(125, 30, 0),
        .x_beg = 506.32,
        .y_beg = 215.65};
    aat::ClosedLoopSolver cls(p);
    cls.Solve(2);

    std::println("{}", aat::SolveResultOf(cls));
}