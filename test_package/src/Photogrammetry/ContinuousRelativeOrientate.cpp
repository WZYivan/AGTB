#include <AGTB/Photogrammetry/ContinuousRelativeOrientate.hpp>

namespace ap = AGTB::Photogrammetry;

int main()
{
    ap::ContinuousRelativeOrienteParam p{};
    ap::ContinuousRelativeOrienteResult r = ap::Solve(p);
}