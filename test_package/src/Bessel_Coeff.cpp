#include <AGTB/Geodesy/Solution/Bessel.hpp>

namespace agsb = AGTB::Geodesy::Solution::Bessel;
namespace agsbc = agsb::Coefficients;
namespace ag = AGTB::Geodesy;
using agsbc::Tag;
// using impl_def = agsbc::Impl<ag::Ellipsoid::Krasovski, Tag, Tag::General>;
// using impl_simp = agsbc::Impl<ag::Ellipsoid::Krasovski, Tag, Tag::Specified>;

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
    using impl_def = agsbc::Impl<ellipsoid, Tag, Tag::General>;
    using impl_simp = agsbc::Impl<ellipsoid, Tag, Tag::Specified>;

    std::println("{:=^50}", msg);
    print_impl<impl_def>(pow_cosA0_2, "default");
    print_impl<impl_simp>(pow_cosA0_2, "simplified");
}

enum class my_coeff_tag
{
    General,
    Specified,
    CustomMethod
};

AGTB_DEFAULT_IMPL_CUSTOM_TAG_GENERAL(my_coeff_tag);
AGTB_DEFAULT_IMPL_CUSTOM_TAG_ELLIPSOID_SPECIFIED(my_coeff_tag, ag::Ellipsoid::Krasovski);
using my_impl_def = agsbc::Impl<ag::Ellipsoid::Krasovski, my_coeff_tag, my_coeff_tag::General>;
using my_impl_simp = agsbc::Impl<ag::Ellipsoid::Krasovski, my_coeff_tag, my_coeff_tag::Specified>;

template <>
struct agsbc::Impl<ag::Ellipsoid::IE1975, my_coeff_tag, my_coeff_tag::CustomMethod>
{
    static auto A_B_C(double)
    {
        return std::make_tuple(0.0, 0.0, 0.0);
    }
    static auto alpha_beta(double)
    {
        return std::make_tuple(0.0, 0.0);
    }
};
using my_impl_cus = agsbc::Impl<ag::Ellipsoid::IE1975, my_coeff_tag, my_coeff_tag::CustomMethod>;

int main()
{
    double pow_cosA0_2 = 0.628'156'465;
    compare_coeff_impl<ag::Ellipsoid::Krasovski>(pow_cosA0_2, "Krasovski");
    compare_coeff_impl<ag::Ellipsoid::IE1975>(pow_cosA0_2, "IE1975");
    std::println("{:=^50}", "new impl tag");
    print_impl<my_impl_def>(pow_cosA0_2, "my impl for Krasovski default");
    print_impl<my_impl_simp>(pow_cosA0_2, "my impl for Krasovski simplified");
    print_impl<my_impl_cus>(pow_cosA0_2, "my impl for Krasovski custom");
}