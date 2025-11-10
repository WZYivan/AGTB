#include <AGTB/Photogrammetry/SpaceIntersection.hpp>

namespace ap = AGTB::Photogrammetry;

int main()
{
    ap::SpaceIntersectionParam p{};
    ap::SpaceIntersectionResult r = ap::Solve(p);
}