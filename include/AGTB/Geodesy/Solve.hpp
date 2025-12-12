#ifndef __AGTB_GEODESY_SOLVE_HPP__
#define __AGTB_GEODESY_SOLVE_HPP__

#include "Solution/Gauss.hpp"

AGTB_GEODESY_BEGIN

enum class Solutions
{
    Gauss
};

template <Solutions __solution>
struct Solver
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
};

namespace SolverConfigConcept
{
    template <typename T>
    concept Gauss = requires {
        { T::ellipsoid } -> std::convertible_to<Ellipsoids>;
        { T::unit } -> std::convertible_to<Units>;
        typename T::ForwardResult;
        typename T::InverseResult;
    };
}

template <>
struct Solver<Solutions::Gauss>
{
    template <Ellipsoids __ellipsoid, Units __unit>
    struct Config
    {
        constexpr static Ellipsoids ellipsoid = __ellipsoid;
        constexpr static Units unit = __unit;
        using ForwardResult = typename Solution::Gauss::ForwardResult<unit>;
        using InverseResult = typename Solution::Gauss::InverseResult;
    };

    template <Units __unit>
    using ForwardResult = typename Solution::Gauss::ForwardResult<__unit>;
    using InverseResult = typename Solution::Gauss::InverseResult;

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

    template <SolverConfigConcept::Gauss __config>
    static inline typename __config::ForwardResult Forward(Longitude<__config::unit> L, Latitude<__config::unit> B, double S, Angle a_forward, double epsilon = 1e-5)
    {
        return Forward<__config::ellipsoid, __config::unit>(L, B, S, a_forward, epsilon);
    }

    template <SolverConfigConcept::Gauss __config>
    static inline typename __config::InverseResult Inverse(Longitude<__config::unit> L1, Latitude<__config::unit> B1, Longitude<__config::unit> L2, Latitude<__config::unit> B2)
    {
        return Inverse<__config::ellipsoid, __config::unit>(L1, B1, L2, B2);
    }
};

AGTB_GEODESY_END

#endif