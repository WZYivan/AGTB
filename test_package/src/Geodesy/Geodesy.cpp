#include <AGTB/Geodesy.hpp>

namespace ag = AGTB::Geodesy;

int main()
{
    using Solver = ag::Solver<ag::Solutions::Bessel>;
    using Config = Solver::Config<ag::Ellipsoids::CGCS2000, ag::Units::Radian>;
}