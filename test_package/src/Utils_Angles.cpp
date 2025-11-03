#include "AGTB/Utils/Angles.hpp" // 假设头文件路径
#include <iostream>
#include <cmath>
#include <numbers>
#include <cassert>
#include <stdexcept>

// Helper function to compare doubles with tolerance
bool are_equal(double a, double b, double epsilon = 1e-10)
{
    return std::abs(a - b) < epsilon;
}

void test_constants()
{
    std::cout << "Testing Constants...\n";
    assert(are_equal(AGTB::Utils::Angles::Deg2Rad, std::numbers::pi / 180.0));
    assert(are_equal(AGTB::Utils::Angles::Rad2Deg, 180.0 / std::numbers::pi));
    assert(are_equal(AGTB::Utils::Angles::Pi, std::numbers::pi));
    std::cout << "  Constants OK\n\n";
}

void test_IsValidMS()
{
    std::cout << "Testing IsValidMS...\n";
    assert(AGTB::Utils::Angles::IsValidMS(0, 0));                   // Valid
    assert(AGTB::Utils::Angles::IsValidMS(59, 59.999999999999999)); // Valid
    assert(!AGTB::Utils::Angles::IsValidMS(60, 0));                 // Invalid minutes
    assert(!AGTB::Utils::Angles::IsValidMS(0, 60));                 // Invalid seconds
    assert(!AGTB::Utils::Angles::IsValidMS(-1, 0));                 // Invalid minutes
    assert(!AGTB::Utils::Angles::IsValidMS(0, -1));                 // Invalid seconds
    std::cout << "  IsValidMS OK\n\n";
}

void test_FromDMS()
{
    std::cout << "Testing FromDMS...\n";

    // Test positive DMS
    double rad_pos = AGTB::Utils::Angles::FromDMS(45, 30, 15.5);
    double expected_pos = (45.0 + 30.0 / 60.0 + 15.5 / 3600.0) * AGTB::Utils::Angles::Deg2Rad;
    assert(are_equal(rad_pos, expected_pos));
    std::cout << "  FromDMS(45, 30, 15.5) = " << rad_pos << " (expected ~" << expected_pos << ")\n";

    // Test negative DMS
    double rad_neg = AGTB::Utils::Angles::FromDMS(-45, 30, 15.5);
    double expected_neg = -(45.0 + 30.0 / 60.0 + 15.5 / 3600.0) * AGTB::Utils::Angles::Deg2Rad; // Note the minus sign
    assert(are_equal(rad_neg, expected_neg));
    std::cout << "  FromDMS(-45, 30, 15.5) = " << rad_neg << " (expected ~" << expected_neg << ")\n";

    // Test zero
    double rad_zero = AGTB::Utils::Angles::FromDMS(0, 0, 0);
    assert(are_equal(rad_zero, 0.0));
    std::cout << "  FromDMS(0, 0, 0) = " << rad_zero << "\n";

    // Test edge case: 59 minutes, 59.999... seconds
    double rad_edge = AGTB::Utils::Angles::FromDMS(10, 59, 59.99999999999);
    double expected_edge = (10.0 + 59.0 / 60.0 + 59.999999999999999 / 3600.0) * AGTB::Utils::Angles::Deg2Rad;
    assert(are_equal(rad_edge, expected_edge));
    std::cout << "  FromDMS(10, 59, 59.999...) OK\n";

    // Test invalid input (this should throw)
    try
    {
        AGTB::Utils::Angles::FromDMS(0, 60, 0); // Should throw
        assert(false);                          // Should not reach here
    }
    catch (const std::domain_error &)
    {
        std::cout << "  FromDMS(0, 60, 0) correctly threw domain_error\n";
    }

    try
    {
        AGTB::Utils::Angles::FromDMS(0, 0, 60); // Should throw
        assert(false);                          // Should not reach here
    }
    catch (const std::domain_error &)
    {
        std::cout << "  FromDMS(0, 0, 60) correctly threw domain_error\n";
    }

    std::cout << "  FromDMS OK\n\n";
}

void test_ToDegrees()
{
    std::cout << "Testing ToDegrees...\n";
    double rad = std::numbers::pi / 2; // 90 degrees
    double deg = AGTB::Utils::Angles::ToDegrees(rad);
    assert(are_equal(deg, 90.0));
    std::cout << "  ToDegrees(pi/2) = " << deg << "\n";

    rad = -std::numbers::pi / 4; // -45 degrees
    deg = AGTB::Utils::Angles::ToDegrees(rad);
    assert(are_equal(deg, -45.0));
    std::cout << "  ToDegrees(-pi/4) = " << deg << "\n";
    std::cout << "  ToDegrees OK\n\n";
}

void test_ToSeconds()
{
    std::cout << "Testing ToSeconds...\n";
    double rad = std::numbers::pi / 6; // 30 degrees = 30 * 3600 = 108000 seconds
    double sec = AGTB::Utils::Angles::ToSeconds(rad);
    assert(are_equal(sec, 108000.0));
    std::cout << "  ToSeconds(pi/6) = " << sec << "\n";
    std::cout << "  ToSeconds OK\n\n";
}

void test_ToDMS()
{
    std::cout << "Testing ToDMS...\n";

    // Test positive radian -> DMS
    double rad_pos = AGTB::Utils::Angles::FromDMS(45, 30, 15.5); // Get radian value for 45:30:15.5
    auto [d_pos, m_pos, s_pos] = AGTB::Utils::Angles::ToDMS(rad_pos);
    assert(d_pos == 45.0 && m_pos == 30.0 && are_equal(s_pos, 15.5));
    std::cout << "  ToDMS(FromDMS(45, 30, 15.5)) = (" << d_pos << ", " << m_pos << ", " << s_pos << ")\n";

    // Test negative radian -> DMS
    double rad_neg = AGTB::Utils::Angles::FromDMS(-45, 30, 15.5); // Get radian value for -45:30:15.5
    auto [d_neg, m_neg, s_neg] = AGTB::Utils::Angles::ToDMS(rad_neg);
    // According to the code: sign is applied to degrees part
    assert(d_neg == -45.0 && m_neg == 30.0 && are_equal(s_neg, 15.5)); // Minutes and seconds remain positive
    std::cout << "  ToDMS(FromDMS(-45, 30, 15.5)) = (" << d_neg << ", " << m_neg << ", " << s_neg << ")\n";

    // Test zero
    auto [d_zero, m_zero, s_zero] = AGTB::Utils::Angles::ToDMS(0.0);
    assert(are_equal(d_zero, 0.0) && are_equal(m_zero, 0.0) && are_equal(s_zero, 0.0));
    std::cout << "  ToDMS(0) = (" << d_zero << ", " << m_zero << ", " << s_zero << ")\n";

    // Test small positive angle
    double small_rad = 0.0001; // Very small positive angle
    auto [d_small, m_small, s_small] = AGTB::Utils::Angles::ToDMS(small_rad);
    // Should be close to 0d 0m X.XXs where X.XX is the seconds equivalent of small_rad
    double expected_deg = AGTB::Utils::Angles::ToDegrees(small_rad);
    double expected_sec = expected_deg * 3600;
    assert(d_small == 0.0 && m_small == 0.0 && are_equal(s_small, expected_sec));
    std::cout << "  ToDMS(" << small_rad << ") = (" << d_small << ", " << m_small << ", " << s_small << ")\n";

    // Test small negative angle
    double small_neg_rad = -0.0001; // Very small negative angle
    auto [d_s_neg, m_s_neg, s_s_neg] = AGTB::Utils::Angles::ToDMS(small_neg_rad);
    // Should be close to -0d 0m X.XXs
    double expected_neg_deg = AGTB::Utils::Angles::ToDegrees(small_neg_rad);
    double expected_neg_sec = std::abs(expected_neg_deg) * 3600;                       // Use absolute value for seconds
    assert(d_s_neg == -0.0 && m_s_neg == 0.0 && are_equal(s_s_neg, expected_neg_sec)); // d_s_neg might print as 0 or -0 depending on implementation
    std::cout << "  ToDMS(" << small_neg_rad << ") = (" << d_s_neg << ", " << m_s_neg << ", " << s_s_neg << ")\n";

    std::cout << "  ToDMS OK\n\n";
}

void test_Normalization()
{
    std::cout << "Testing Normalization...\n";

    // Test NormalizedStd [0, 2π)
    double input_2pi_plus = 2 * std::numbers::pi + 0.1;
    double norm_std_plus = AGTB::Utils::Angles::NormalizedStd(input_2pi_plus);
    assert(are_equal(norm_std_plus, 0.1)); // Should wrap to 0.1
    std::cout << "  NormalizedStd(2π + 0.1) = " << norm_std_plus << "\n";

    double input_neg = -0.1;
    double norm_std_neg = AGTB::Utils::Angles::NormalizedStd(input_neg);
    assert(are_equal(norm_std_neg, 2 * std::numbers::pi - 0.1)); // Should wrap to 2π - 0.1
    std::cout << "  NormalizedStd(-0.1) = " << norm_std_neg << "\n";

    double input_just_over_2pi = 2 * std::numbers::pi;
    double norm_std_2pi = AGTB::Utils::Angles::NormalizedStd(input_just_over_2pi);
    assert(are_equal(norm_std_2pi, 0.0)); // 2π should normalize to 0
    std::cout << "  NormalizedStd(2π) = " << norm_std_2pi << "\n";

    // Test NormalizedSym [-π, π)
    double input_pi_plus = std::numbers::pi + 0.1;
    double norm_sym_plus = AGTB::Utils::Angles::NormalizedSym(input_pi_plus);
    assert(are_equal(norm_sym_plus, -std::numbers::pi + 0.1)); // Should wrap to -π + 0.1
    std::cout << "  NormalizedSym(π + 0.1) = " << norm_sym_plus << "\n";

    double input_neg_pi_minus = -std::numbers::pi - 0.1;
    double norm_sym_neg_minus = AGTB::Utils::Angles::NormalizedSym(input_neg_pi_minus);
    assert(are_equal(norm_sym_neg_minus, std::numbers::pi - 0.1)); // Should wrap to π - 0.1
    std::cout << "  NormalizedSym(-π - 0.1) = " << norm_sym_neg_minus << "\n";

    double input_neg_pi = -std::numbers::pi;
    double norm_sym_neg_pi = AGTB::Utils::Angles::NormalizedSym(input_neg_pi);
    assert(are_equal(norm_sym_neg_pi, std::numbers::pi)); // -π should normalize to +π (as per range [-π, π))
    std::cout << "  NormalizedSym(-π) = " << norm_sym_neg_pi << "\n";

    double input_just_under_neg_pi = -std::numbers::pi + 1e-15; // Slightly greater than -π
    double norm_sym_just_under_neg_pi = AGTB::Utils::Angles::NormalizedSym(input_just_under_neg_pi);
    assert(are_equal(norm_sym_just_under_neg_pi, input_just_under_neg_pi)); // Should stay in range
    std::cout << "  NormalizedSym(-π + ε) ≈ " << norm_sym_just_under_neg_pi << "\n";

    std::cout << "  Normalization OK\n\n";
}

void test_Predefined_Constants()
{
    std::cout << "Testing Predefined Constants (rad_45d, rad_90d, rad_180d)...\n";
    // These should be equal to the result of calling FromDMS
    assert(are_equal(AGTB::Utils::Angles::rad_45d, AGTB::Utils::Angles::FromDMS(45)));
    assert(are_equal(AGTB::Utils::Angles::rad_90d, AGTB::Utils::Angles::FromDMS(90)));
    assert(are_equal(AGTB::Utils::Angles::rad_180d, AGTB::Utils::Angles::FromDMS(180)));
    std::cout << "  rad_45d = " << AGTB::Utils::Angles::rad_45d << "\n";
    std::cout << "  rad_90d = " << AGTB::Utils::Angles::rad_90d << "\n";
    std::cout << "  rad_180d = " << AGTB::Utils::Angles::rad_180d << "\n";
    std::cout << "  Predefined Constants OK\n\n";
}

int main()
{
    std::cout << "Starting AGTB Angles Tests...\n\n";

    test_constants();
    test_IsValidMS();
    test_FromDMS();
    test_ToDegrees();
    test_ToSeconds();
    test_ToDMS();
    test_Normalization();
    test_Predefined_Constants();

    std::cout << "All Tests Passed!\n";

    return 0;
}