#include <AGTB/Geodesy/MeridianArc.hpp>
#include <AGTB/Utils/Angles.hpp>
#include <print>
#include <cmath> // For std::abs if needed for comparisons

namespace ag = AGTB::Geodesy;
namespace age = ag::Ellipsoid;
namespace au = AGTB::Utils;

// Test the Length function for both General and Specified options
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
void test_inverse_length_general(ag::GeodeticLatitude B_expected, double threshold = std::numeric_limits<double>::epsilon())
{
    // Calculate the length for the expected latitude using the General solver
    using Solver = ag::MeridianArcSolver<e, ag::EllipsoidBasedOption::Specified>;
    double len = Solver::Forward(B_expected);
    std::println("Calculated Length for {}(deg): {} m", static_cast<double>(B_expected) * 180.0 / M_PI, len);

    // Now, try to solve for the latitude given the length
    double B_computed = Solver::Inverse(len, threshold).Value();
    std::println("Computed Latitude from Length: {} rad ({} deg)",
                 B_computed, B_computed * 180.0 / M_PI);
    std::println("Expected Latitude: {} rad ({} deg)",
                 static_cast<double>(B_expected), static_cast<double>(B_expected) * 180.0 / M_PI);
    std::println("Difference (rad): {}", std::abs(static_cast<double>(B_expected) - B_computed));
    std::println("---");
}

int main()
{
    // Define test latitude: 45 degrees
    ag::GeodeticLatitude B_45_deg = au::Angles::FromDMS(45.0);
    ag::GeodeticLatitude B_0_deg = au::Angles::FromDMS(0.0);   // Equator
    ag::GeodeticLatitude B_90_deg = au::Angles::FromDMS(90.0); // North Pole (may be extreme)

    // std::println("--- Testing Meridian Arc Length Calculation ---");

    // // Test Length function with General option (Krasovski & IE1975)
    // std::println("Krasovski Ellipsoid (General Option):");
    // test_meridian_length<age::Krasovski, ag::EllipsoidBasedOption::General>(B_45_deg);
    // test_meridian_length<age::Krasovski, ag::EllipsoidBasedOption::General>(B_0_deg);
    // test_meridian_length<age::Krasovski, ag::EllipsoidBasedOption::General>(B_90_deg);

    // std::println("\nIE1975 Ellipsoid (General Option):");
    // test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::General>(B_45_deg);
    // test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::General>(B_0_deg);
    // test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::General>(B_90_deg);

    std::println("\n--- Testing Specified Meridian Arc Length Calculation ---");

    // Test Length function with Specified option (Krasovski & IE1975)
    std::println("Krasovski Ellipsoid (Specified Option):");
    test_meridian_length<age::Krasovski, ag::EllipsoidBasedOption::Specified>(B_45_deg);
    test_meridian_length<age::Krasovski, ag::EllipsoidBasedOption::Specified>(B_0_deg);
    test_meridian_length<age::Krasovski, ag::EllipsoidBasedOption::Specified>(B_90_deg);

    std::println("\nIE1975 Ellipsoid (Specified Option):");
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_45_deg);
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_0_deg);
    test_meridian_length<age::IE1975, ag::EllipsoidBasedOption::Specified>(B_90_deg);

    std::println("\n--- Testing Inverse Meridian Arc Calculation (General Option Only) ---");

    // Test Inverse Length function (only available for General option)
    std::println("Testing Krasovski (General) Inverse:");
    test_inverse_length_general<age::Krasovski>(B_45_deg, 1e-10);

    std::println("Testing IE1975 (General) Inverse:");
    test_inverse_length_general<age::IE1975>(B_45_deg, 1e-10);

    // Test with a smaller threshold for higher precision
    std::println("Testing Krasovski (General) Inverse with higher precision:");
    test_inverse_length_general<age::Krasovski>(B_0_deg, 1e-12);
    test_inverse_length_general<age::Krasovski>(B_45_deg, 1e-12);

    std::println("Testing IE1975 (General) Inverse with higher precision:");
    test_inverse_length_general<age::IE1975>(B_0_deg, 1e-12);
    test_inverse_length_general<age::IE1975>(B_45_deg, 1e-12);

    return 0;
}