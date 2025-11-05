#include <AGTB/Geodesy/MeridianArc.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>
#include <cmath>

namespace ag = AGTB::Geodesy;
namespace age = ag::Ellipsoid;
namespace au = AGTB::Utils;
namespace aua = au::Angles;

template <ag::EllipsoidConcept e, ag::EllipsoidBasedOption opt>
void test_meridian_length(ag::GeodeticLatitude B)
{
    using Solver = ag::MeridianArcSolver<e, opt>;
    double arc_length = Solver::Forward(B);
    std::println("Latitude (deg): {}, Meridian Arc Length: {} m",
                 au::Angles::ToDegrees(static_cast<double>(B)),
                 arc_length);
}

template <ag::EllipsoidConcept e>
void test_inverse_length_general(ag::GeodeticLatitude B_expected, double threshold)
{
    using Solver = ag::MeridianArcSolver<e, ag::EllipsoidBasedOption::General>;

    double len = Solver::Forward(B_expected);
    std::println("Calculated Length for {}(deg): {} m", static_cast<double>(B_expected) * 180.0 / M_PI, len);

    double B_computed = Solver::Inverse(len, threshold).Value();
    std::println("Computed Latitude from Length: {} rad ({} deg)",
                 B_computed, B_computed * 180.0 / M_PI);
    std::println("Expected Latitude: {} rad ({} deg)",
                 static_cast<double>(B_expected), static_cast<double>(B_expected) * 180.0 / M_PI);
    std::println("Difference (dms): {}", aua::Angle::FromRad(std::abs(static_cast<double>(B_expected) - B_computed)).ToString());
    std::println("---");
}

template <ag::EllipsoidConcept e>
void test_inverse_length_specified(ag::GeodeticLatitude B_expected, double threshold)
{
    using Solver = ag::MeridianArcSolver<e, ag::EllipsoidBasedOption::Specified>;

    double len = Solver::Forward(B_expected);
    std::println("Calculated Length for {}(deg): {} m", static_cast<double>(B_expected) * 180.0 / M_PI, len);

    double B_computed = Solver::Inverse(len, threshold).Value();
    std::println("Computed Latitude from Length: {} rad ({} deg)",
                 B_computed, B_computed * 180.0 / M_PI);
    std::println("Expected Latitude: {} rad ({} deg)",
                 static_cast<double>(B_expected), static_cast<double>(B_expected) * 180.0 / M_PI);
    std::println("Difference (dms): {}", aua::Angle::FromRad(std::abs(static_cast<double>(B_expected) - B_computed)).ToString());
    std::println("---");
}

int main()
{
    ag::GeodeticLatitude B_45_deg = au::Angles::FromDMS(45.0);
    ag::GeodeticLatitude B_0_deg = au::Angles::FromDMS(0.0);
    ag::GeodeticLatitude B_90_deg = au::Angles::FromDMS(90.0);
    ag::GeodeticLatitude B_30_deg = au::Angles::FromDMS(30.0);

    std::println("\n--- Testing Specified Meridian Arc Length Calculation ---");

    std::println("\nIE1975 Ellipsoid (Specified Option):");
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_45_deg);
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_0_deg);
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_90_deg);
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_30_deg);

    std::println("\n--- Testing Inverse Meridian Arc Calculation Iterative Method ---");

    std::println("Testing IE1975 (Specified Option) Inverse:");
    test_inverse_length_specified<age::IE1975>(B_30_deg, 1e-2);

    std::println("Testing Krasovski (Specified Option) Inverse:");
    test_inverse_length_specified<age::Krasovski>(B_30_deg, 1);
    test_inverse_length_specified<age::Krasovski>(B_30_deg, 1e-1);
    test_inverse_length_specified<age::Krasovski>(B_30_deg, 1e-2);

    std::println("Testing IE1975 (General Option) Inverse with higher precision:");
    test_inverse_length_general<age::IE1975>(B_30_deg, 1e-5);
    test_inverse_length_general<age::Krasovski>(B_30_deg, std::numeric_limits<double>::epsilon());
    test_inverse_length_general<age::Krasovski>(B_45_deg, std::numeric_limits<double>::epsilon());

    // std::println("{}",
    //              ag::MeridianArcLengthCoefficient<age::IE1975, ag::EllipsoidBasedOption::General>::ToString());
    // std::println("{}",
    //              ag::PrincipleCurvatureCoefficient<age::IE1975, ag::EllipsoidBasedOption::General>::ToString());
    // std::println("{}",
    //              ag::MeridianArcLengthCoefficient<age::IE1975, ag::EllipsoidBasedOption::Specified>::ToString());
    // std::println("{}",
    //              ag::PrincipleCurvatureCoefficient<age::IE1975, ag::EllipsoidBasedOption::Specified>::ToString());

    return 0;
}