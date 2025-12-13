#ifndef __AGTB_GEODESY_SOLVE_HPP__
#define __AGTB_GEODESY_SOLVE_HPP__

#include "Solution/Gauss.hpp"
#include "Solution/Bessel.hpp"

AGTB_GEODESY_BEGIN

enum class Solutions
{
    Gauss,
    Bessel
};

template <Solutions __solution>
struct Solver
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
};

namespace Solution
{
    template <typename T>
    concept ConfigConcept = requires {
        { T::ellipsoid } -> std::convertible_to<Ellipsoids>;
        { T::unit } -> std::convertible_to<Units>;
        typename T::ForwardResult;
        typename T::InverseResult;
        typename T::Lat;
        typename T::Lon;
    };

    template <Ellipsoids __ellipsoid, Units __unit>
    struct Config
    {
        constexpr static Ellipsoids ellipsoid = __ellipsoid;
        constexpr static Units unit = __unit;
        using ForwardResult = typename Solution::ForwardResult<unit>;
        using InverseResult = typename Solution::InverseResult;
        using Lat = Latitude<__unit>;
        using Lon = Longitude<__unit>;
    };
}

template <>
struct Solver<Solutions::Gauss>
{
    constexpr static Solutions solve_method = Solutions::Gauss;

    template <Ellipsoids __ellipsoid, Units __unit>
    using Config = typename Solution::Config<__ellipsoid, __unit>;

    template <Units __unit>
    using ForwardResult = typename Solution::ForwardResult<__unit>;
    using InverseResult = typename Solution::InverseResult;

    template <Ellipsoids __ellipsoid, Units __unit>
    static inline ForwardResult<__unit> Forward(Longitude<__unit> L, Latitude<__unit> B, double S, Angle a_forward, double epsilon = 1e-5)
    {
        return Solution::Gauss::ForwardSolve<__ellipsoid, __unit>(L, B, S, a_forward, epsilon);
    }

    template <Ellipsoids __ellipsoid, Units __unit>
    static inline InverseResult Inverse(Longitude<__unit> L1, Latitude<__unit> B1, Longitude<__unit> L2, Latitude<__unit> B2)
    {
        return Solution::Gauss::InverseSolve<__ellipsoid, __unit>(L1, B1, L2, B2);
    }

    template <Solution::ConfigConcept __config>
    static inline typename __config::ForwardResult Forward(__config::Lon L, __config::Lat B, double S, Angle a_forward, double epsilon = 1e-5)
    {
        return Forward<__config::ellipsoid, __config::unit>(L, B, S, a_forward, epsilon);
    }

    template <Solution::ConfigConcept __config>
    static inline typename __config::InverseResult Inverse(__config::Lon L1, __config::Lat B1, __config::Lon L2, __config::Lat B2)
    {
        return Inverse<__config::ellipsoid, __config::unit>(L1, B1, L2, B2);
    }
};

template <>
struct Solver<Solutions::Bessel>
{
    constexpr static Solutions solve_method = Solutions::Bessel;

    template <Ellipsoids __ellipsoid, Units __unit>
    using Config = typename Solution::Config<__ellipsoid, __unit>;

    template <Units __unit>
    using ForwardResult = typename Solution::ForwardResult<__unit>;
    using InverseResult = typename Solution::InverseResult;

    template <Ellipsoids __ellpsoid, Units __unit>
    inline static ForwardResult<__unit> Forward(Longitude<__unit> L, Latitude<__unit> B, double S, Angle a_forward)
    {
        return Solution::Bessel::ForwardSolve<__ellpsoid, __unit>(L, B, S, a_forward);
    }

    template <Solution::ConfigConcept __config>
    inline static __config::ForwardResult Forward(__config::Lon L, __config::Lat B, double S, Angle a_forward)
    {
        return Forward<__config::ellipsoid, __config::unit>(L, B, S, a_forward);
    }

    template <Ellipsoids __ellipsoid, Units __unit>
    static inline InverseResult Inverse(Longitude<__unit> L1, Latitude<__unit> B1, Longitude<__unit> L2, Latitude<__unit> B2, double epsilon = 1e-5)
    {
        return Solution::Bessel::InverseSolve<__ellipsoid, __unit>(L1, B1, L2, B2, epsilon);
    }

    template <Solution::ConfigConcept __config>
    static inline typename __config::InverseResult Inverse(__config::Lon L1, __config::Lat B1, __config::Lon L2, __config::Lat B2, double epsilon = 1e-5)
    {
        return Inverse<__config::ellipsoid, __config::unit>(L1, B1, L2, B2, epsilon);
    }
};

AGTB_GEODESY_END

#endif