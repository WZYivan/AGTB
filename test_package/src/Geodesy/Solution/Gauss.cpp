#include <AGTB/Geodesy/Solve.hpp>
#include <print>

namespace ag = AGTB::Geodesy;

int main()
{

    using solver = ag::Solver<ag::Solutions::Gauss>;
    using config = solver::Config<ag::Ellipsoids::CGCS2000, ag::Units::Radian>;

    ag::Latitude<>
        B1(47, 46, 52.647'0),
        B2(48, 4, 9.638'4);
    ag::Longitude<>
        L1(35, 49, 36.330'0),
        L2(36, 14, 45.050'5);

    std::println("L1 = {}, B1 = {}, L2 = {}, B2 = {}", L1.ToString(), B1.ToString(), L2.ToString(), B2.ToString());
    config::InverseResult ri = solver::Inverse<config>(L1, B1, L2, B2);

    std::println("S = {}, A12 = {}, A21 = {}",
                 ri.s,
                 ri.a_forwards.ToString(),
                 ri.a_backwards.ToString());

    config::ForwardResult rf = solver::Forward<config>(L1, B1, ri.s, ri.a_forwards);

    std::println("B2 = {}, L2 = {}, A21 = {}",
                 rf.B.ToString(),
                 rf.L.ToString(),
                 rf.a_backward.ToString());

    double
        dl = (rf.L.Rad() - L2.Rad()) * AGTB::rad2sec,
        db = (rf.B.Rad() - B2.Rad()) * AGTB::rad2sec,
        da = (rf.a_backward.Rad() - ri.a_backwards.Rad()) * AGTB::rad2sec;
    std::println("dl = {}, db = {}, da = {}", dl, db, da);
}