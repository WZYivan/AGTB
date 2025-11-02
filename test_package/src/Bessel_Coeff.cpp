#include <AGTB/Geodesy/Solution/Bessel.hpp>

namespace agsb = AGTB::Geodesy::Solution::Bessel;
namespace agsbc = agsb::Coefficients;
namespace ag = AGTB::Geodesy;
using Opt = ag::EllipsoidBasedOption;

#include <print>

template <agsbc::ImplConcept impl>
void print_impl(double pow_cosA0_2, std::string msg)
{
    auto [A, B, C] =
        impl::A_B_C(pow_cosA0_2);
    auto [a, b] =
        impl::alpha_beta(pow_cosA0_2);

    std::println("{}:\nA={}\nB={}\nC={}\nalpha={}\nbeta={}\n", msg, A, B, C, a, b);
}

template <ag::EllipsoidConcept ellipsoid>
void compare_coeff_impl(double pow_cosA0_2, std::string msg)
{
    using impl_def = agsbc::Impl<ellipsoid, Opt::General>;
    using impl_simp = agsbc::Impl<ellipsoid, Opt::Specified>;

    std::println("{:=^50}", msg);
    print_impl<impl_def>(pow_cosA0_2, "default");
    print_impl<impl_simp>(pow_cosA0_2, "simplified");
}

int main()
{
    double pow_cosA0_2 = 0.628'156'465;
    compare_coeff_impl<ag::Ellipsoid::Krasovski>(pow_cosA0_2, "Krasovski");
    compare_coeff_impl<ag::Ellipsoid::IE1975>(pow_cosA0_2, "IE1975");
}