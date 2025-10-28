#include <AGTB/Geodesy/Base.hpp>
#include <print>
#include <iostream>

namespace ag = AGTB::Geodesy;

template <ag::EllipsoidParam E>
void PE()
{
    std::println(
        R"(
a={}
b={}
c={}
alpha={}
e1_2={}
e2_2={}
)",
        E::a, E::b, E::c, E::alpha, E::e1_2, E::e2_2);
}

AGTB_DEFINE_QUICK_ELLIPSOID(my_ellipsoid, 6378245.0, 6356863.018773047);

template <ag::GeodeticLatitudeParam L>
void PL(L l)
{
    std::println(
        R"(
B={}
t={}
tau_2={}
W={}
V={}
)",
        l.B, l.t, l.tau_2, l.W, l.V);
}

int main()
{
    using ag::Ellipsoid::Krasovski;

    std::cout << "Krasovski" << std::endl;
    PE<Krasovski>();

    std::cout << "my ellipsoid" << std::endl;
    PE<my_ellipsoid>();

    std::cout << "Constants at B(45.0)" << std::endl;
    PL(ag::GeodeticLatitudeConstants<Krasovski>(45));
}
