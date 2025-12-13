#include <print>
#include <AGTB/Geodesy/Solve.hpp>

namespace ag = AGTB::Geodesy;
namespace sb = ag::Solution::Bessel;

int main()
{
    using solver = ag::Solver<ag::Solutions::Bessel>;
    using config = solver::Config<ag::Ellipsoids::Krasovski, ag::Units::Radian>;

    config::Lat B1(47, 46, 52.647'0);
    config::Lon L1(35, 49, 36.330'0);
    AGTB::Angle A1(44, 12, 13.664);
    double S = 44'797.282'6;

    config::ForwardResult rf = solver::Forward<config>(L1, B1, S, A1);
    std::println("L2 = {}, B2 = {}, A21 = {}", rf.L.ToString(), rf.B.ToString(), rf.a_backward.ToString());

    config::Lat Bf1(30, 30, 0), Bf2(-37, 43, 44.1);
    config::Lon Lf1(114, 20, 0), Lf2(51, 16, 32.5);
    // config::InverseResult ri = solver::Inverse<config>(Lf1, Bf1, Lf2, Bf2, 1e-10);
    config::InverseResult ri = solver::Inverse<config>(L1, B1, rf.L, rf.B);
    std::println("A12 = {}, A21 = {}, S = {}", ri.a_forwards.ToString(), ri.a_backwards.ToString(), ri.s);
}