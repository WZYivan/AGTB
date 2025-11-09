#include <AGTB/Photogrammetry/SpaceIntersection.hpp>

namespace ap = AGTB::Photogrammetry;

int main()
{
    ap::SpaceIntersectionParam p{};
    ap::SpaceIntersectionSolveResult r = ap::Solve(p);
}