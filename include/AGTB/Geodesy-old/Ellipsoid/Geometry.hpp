#ifndef __AGTB_GEODESY_ELLIPSOID_GEOMETRY_HPP__
#define __AGTB_GEODESY_ELLIPSOID_GEOMETRY_HPP__

#include "Base.hpp"
#include "../Base/GeoLatLon.hpp"
#include "../ExMath/PreCorrection.hpp"

AGTB_GEODESY_BEGIN

template <typename ellipsoid_geometry>
concept EllipsoidGeometryConcept = requires {
    { ellipsoid_geometry::a } -> std::convertible_to<double>;
    { ellipsoid_geometry::b } -> std::convertible_to<double>;
    { ellipsoid_geometry::c } -> std::convertible_to<double>;
    { ellipsoid_geometry::alpha } -> std::convertible_to<double>;
    { ellipsoid_geometry::e1_2 } -> std::convertible_to<double>;
    { ellipsoid_geometry::e2_2 } -> std::convertible_to<double>;
    { ellipsoid_geometry::ellipsoid_type } -> std::convertible_to<EllipsoidType>;
};

template <EllipsoidType __ellipsoid_type>
struct EllipsoidGeometryBase
{
    constexpr static EllipsoidType ellipsoid_type = __ellipsoid_type;
};

/**
 * @brief Basic geometry parameters of an ellipsoid
 *
 * @tparam __ellipsoid_type
 */
template <EllipsoidType __ellipsoid_type>
struct EllipsoidGeometry : public EllipsoidGeometryBase<__ellipsoid_type>
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
};

template <>
struct EllipsoidGeometry<EllipsoidType::Krasovski> : public EllipsoidGeometryBase<EllipsoidType::Krasovski>
{
    constexpr static double
        a = 6'378'245,
        b = 6'356'863.018'773'047'3,
        c = 6'399'698.901'782'771'0,
        alpha = 1.0 / 298.3,
        e1_2 = 0.006'693'421'622'966,
        e2_2 = 0.006'738'525'414'683;
};

template <>
struct EllipsoidGeometry<EllipsoidType::IE1975> : public EllipsoidGeometryBase<EllipsoidType::IE1975>
{
    constexpr static double
        a = 6'378'140,
        b = 6'356'755.288'157'528'7,
        c = 6'399'596.651'988'010'5,
        alpha = 1.0 / 298.257,
        e1_2 = 0.006'694'384'999'588,
        e2_2 = 0.006'739'501'819'473;
};

template <>
struct EllipsoidGeometry<EllipsoidType::WGS84> : public EllipsoidGeometryBase<EllipsoidType::WGS84>
{
    constexpr static double
        a = 6'378'137,
        b = 6'356'752.314'2,
        c = 6'399'593.625'8,
        alpha = 1 / 298.257'223'563,
        e1_2 = 0.006'694'379'990'13,
        e2_2 = 0.006'739'496'742'77;
};

template <>
struct EllipsoidGeometry<EllipsoidType::CGCS2000> : public EllipsoidGeometryBase<EllipsoidType::CGCS2000>
{
    constexpr static double
        a = 6'378'137,
        b = 6'356'752.314'1,
        c = 6'399'593.625'9,
        alpha = 1 / 298.257'222'101,
        e1_2 = 0.006'694'380'022'90,
        e2_2 = 0.006'739'496'775'48;
};

/**
 * @brief More constants associated to latitude
 *
 * @tparam __ellipsoid_geometry
 */
template <EllipsoidGeometryConcept __ellipsoid_geometry>
struct LatitudeConstants
{
    double
        B,
        t,
        nu_2,
        W,
        V;
    constexpr LatitudeConstants(Latitude _B) : B(_B.Rad())
    {
        t = gcem::tan(B);
        nu_2 = __ellipsoid_geometry::e2_2 * gcem::pow(gcem::cos(B), 2);
        W = gcem::sqrt(1 - __ellipsoid_geometry::e1_2 * gcem::pow(gcem::sin(B), 2));
        V = gcem::sqrt(1 + nu_2);
    }
};

template <typename T>
concept LatitudeConstantsConcept = requires(T t) {
    { t.B } -> std::convertible_to<double>;
    { t.t } -> std::convertible_to<double>;
    { t.nu_2 } -> std::convertible_to<double>;
    { t.W } -> std::convertible_to<double>;
    { t.V } -> std::convertible_to<double>;
};

template <EllipsoidGeometryConcept __ellipsoid_geometry>
std::string ToString()
{
    return std::format("a = {}\n"
                       "b = {}\n"
                       "c = {}\n"
                       "alpha = {}\n"
                       "e1_2 = {}\n"
                       "e2_2 = {}\n",
                       __ellipsoid_geometry::a, __ellipsoid_geometry::b, __ellipsoid_geometry::c, __ellipsoid_geometry::alpha, __ellipsoid_geometry::e1_2, __ellipsoid_geometry::e1_2);
}

/**
 * @brief Curvature radius of meridian(M) and prime vertical(N), also calculate average curvature radius(R) from M, N
 *
 */
struct CurvatureRadiusCollection
{
    double M, N;
    double R()
    {
        return gcem::sqrt(M * N);
    }
};

/**
 * @brief Coefficients to calculate principe curvature
 *
 * @tparam __ellipsoid_type
 */
template <EllipsoidType __ellipsoid_type>
struct PrincipleCurvatureRadiiCoeff
{
    using __ellipsoid_geometry = EllipsoidGeometry<__ellipsoid_type>;
    constexpr static EllipsoidType ellipsoid_type = __ellipsoid_type;

    constexpr static double
        a = __ellipsoid_geometry::a,
        e2 = __ellipsoid_geometry::e1_2,

        m0 = a * (1 - e2),
        m2 = 3.0 / 2.0 * e2 * m0,
        m4 = 5.0 / 4.0 * e2 * m2,
        m6 = 7.0 / 6.0 * e2 * m4,
        m8 = 9.0 / 8.0 * e2 * m6,

        n0 = a,
        n2 = 1.0 / 2.0 * e2 * n0,
        n4 = 3.0 / 4.0 * e2 * n2,
        n6 = 5.0 / 6.0 * e2 * n4,
        n8 = 7.0 / 8.0 * e2 * n6;
};

template <>
struct PrincipleCurvatureRadiiCoeff<EllipsoidType::Krasovski>
{
    constexpr static double
        m0 = 6'335'552.717'00,
        m2 = 63'609.788'33,
        m4 = 532.208'92,
        m6 = 4.156'02,
        m8 = 0.031'30,
        n0 = 6'378'245.000'00,
        n2 = 21'346.141'49,
        n4 = 107.159'04,
        n6 = 0.597'72,
        n8 = 0.003'50;
    constexpr static EllipsoidType ellipsoid_type = EllipsoidType::Krasovski;
};

template <>
struct PrincipleCurvatureRadiiCoeff<EllipsoidType::IE1975>
{
    constexpr static double
        m0 = 6'335'442.275'00,
        m2 = 63'617.835'00,
        m4 = 532.353,
        m6 = 4.158'00,
        m8 = 0.031'00,
        n0 = 6'378'140.000'00,
        n2 = 21'348.862'00,
        n4 = 107.188'00,
        n6 = 0.598'00,
        n8 = 0.003'00;
    constexpr static EllipsoidType ellipsoid_type = EllipsoidType::IE1975;
};

template <typename T>
concept PrincipleCurvatureRadiiCoeffConcept = requires {
    { T::m0 } -> std::convertible_to<double>;
    { T::m2 } -> std::convertible_to<double>;
    { T::m4 } -> std::convertible_to<double>;
    { T::m6 } -> std::convertible_to<double>;
    { T::m8 } -> std::convertible_to<double>;
    { T::n0 } -> std::convertible_to<double>;
    { T::n2 } -> std::convertible_to<double>;
    { T::n4 } -> std::convertible_to<double>;
    { T::n6 } -> std::convertible_to<double>;
    { T::n8 } -> std::convertible_to<double>;
    { T::ellipsoid_type } -> std::convertible_to<EllipsoidType>;
};

template <PrincipleCurvatureRadiiCoeffConcept coeff>
std::string ToString()
{
    return std::format("m0 = {}\n m2 = {}\n m4 = {}\n m6 = {}\n m8 = {}\n"
                       "n0 = {}\n n2 = {}\n n4 = {}\n n6 = {}\n n8 = {}\n",
                       coeff::m0, coeff::m2, coeff::m4, coeff::m6, coeff::m8,
                       coeff::n0, coeff::n2, coeff::n4, coeff::n6, coeff::n8);
}

namespace PrincipleCurvatureRadiiImpl
{
    template <EllipsoidGeometryConcept __ellipsoid_geometry, EllipsoidAlgoOption __algo_opt>
    struct Impl
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    };

    template <EllipsoidGeometryConcept __ellipsoid_geometry>
    struct Impl<__ellipsoid_geometry, EllipsoidAlgoOption::General>
    {
        static CurvatureRadiusCollection Invoke(Latitude B)
        {
            double sinBp2 = gcem::pow(B.Sin(), 2),
                   a = __ellipsoid_geometry::a,
                   e1_2 = __ellipsoid_geometry::e1_2,
                   k = 1.0 - e1_2 * sinBp2;
            double M = a * (1.0 - e1_2) * gcem::pow(k, -1.5),
                   N = a * gcem::pow(k, -0.5);
            return {
                .M = M,
                .N = N};
        }
    };

    template <EllipsoidGeometryConcept __ellipsoid_geometry>
    struct Impl<__ellipsoid_geometry, EllipsoidAlgoOption::Specified>
    {
        static CurvatureRadiusCollection Invoke(Latitude B)
        {
            using coeff = PrincipleCurvatureRadiiCoeff<__ellipsoid_geometry::ellipsoid_type>;
            double
                k = B.Sin(),
                k2 = gcem::pow(k, 2),
                k4 = gcem::pow(k, 4),
                k6 = gcem::pow(k, 6),
                k8 = gcem::pow(k, 8),

                m0 = coeff::m0,
                m2 = coeff::m2,
                m4 = coeff::m4,
                m6 = coeff::m6,
                m8 = coeff::m8,

                n0 = coeff::n0,
                n2 = coeff::n2,
                n4 = coeff::n4,
                n6 = coeff::n6,
                n8 = coeff::n8,

                M = m0 + m2 * k2 + m4 * k4 + m6 * k6 + m8 * k8,
                N = n0 + n2 * k2 + n4 * k4 + n6 * k6 + n8 * k8;
            return {.M = M, .N = N};
        }
    };

    template <typename T>
    concept ImplConcept = requires(Latitude B) {
        { T::Invoke(B) } -> std::convertible_to<CurvatureRadiusCollection>;
    };

    template <EllipsoidType __ellipsoid_type, EllipsoidAlgoOption __algo_opt>
    struct CheckImpl
    {
        using __Geometry = EllipsoidGeometry<__ellipsoid_type>;
        using __Impl = Impl<__Geometry, __algo_opt>;

        template <ImplConcept __Impl_P>
        struct DoCheck
        {
        };

        using __Check = DoCheck<__Impl>;
    };

    template <EllipsoidType __ellipsoid_type, EllipsoidAlgoOption __algo_opt>
    using CheckedImpl = CheckImpl<__ellipsoid_type, __algo_opt>::__Impl;
}

/**
 * @brief Calculate principle curvature radii in latitude
 *
 * @tparam __ellipsoid_type
 * @tparam __algo_opt
 * @param B latitude
 * @return `CurvatureRadiusCollection`
 */
template <EllipsoidType __ellipsoid_type, EllipsoidAlgoOption __algo_opt = EllipsoidAlgoOption::General>
CurvatureRadiusCollection PrincipleCurvatureRadii(Latitude B)
{
    using Impl = PrincipleCurvatureRadiiImpl::CheckedImpl<__ellipsoid_type, __algo_opt>;
    return Impl::Invoke(B);
}

/**
 * @brief Coefficients to calculate a quarter of meridian length in latitude
 *
 * @tparam `Coeff` `PrincipleCurvatureRadiiCoeff`
 */
template <PrincipleCurvatureRadiiCoeffConcept Coeff>
struct QuarterArcCoeff
{
private:
    constexpr static double
        m0 = Coeff::m0,
        m2 = Coeff::m2,
        m4 = Coeff::m4,
        m6 = Coeff::m6,
        m8 = Coeff::m8;

public:
    constexpr static double
        a0 = (m0 +
              m2 / 2.0 +
              m4 * 3.0 / 8.0 +
              m6 * 5.0 / 16.0 +
              m8 * 35.0 / 128.0) *
             Utils::Angles::deg2rad,
        a2 = (m2 / 2.0 +
              m4 / 2.0 +
              m6 * 15.0 / 32.0 +
              m8 * 7.0 / 16.0) /
             2.0,
        a4 = (m4 / 8.0 +
              m6 * 3.0 / 16.0 +
              m8 * 7.0 / 32.0) /
             4.0,
        a6 = (m6 / 32.0 +
              m8 / 16.0) /
             6.0;
};

template <>
struct QuarterArcCoeff<
    PrincipleCurvatureRadiiCoeff<EllipsoidType::Krasovski>>
{
    constexpr static double
        a0 = 111'134.861,
        a2 = 16'036.480,
        a4 = 16.828,
        a6 = 0.022;
};

template <>
struct QuarterArcCoeff<
    PrincipleCurvatureRadiiCoeff<EllipsoidType::IE1975>>
{
    constexpr static double
        a0 = 111'133.005,
        a2 = 16'038.528,
        a4 = 16.833,
        a6 = 0.022;
};

template <typename T>
concept QuarterArcCoeffConcept = requires {
    { T::a0 } -> std::convertible_to<double>;
    { T::a2 } -> std::convertible_to<double>;
    { T::a4 } -> std::convertible_to<double>;
    { T::a6 } -> std::convertible_to<double>;
};

template <QuarterArcCoeffConcept Coeff>
std::string ToString()
{
    return std::format(" a0 = {}\n a2 = {}\n a4 = {}\n a6 = {}",
                       Coeff::a0, Coeff::a2, Coeff::a4, Coeff::a6);
}

namespace QuarterArcImpl
{
    template <typename T>
    concept ImplConcept = requires(Latitude B) {
        { T::Forward(B) } -> std::convertible_to<double>;
    } && requires(double len, double iter_threshold, bool enable_precorrection) {
        { T::Inverse(len) } -> std::convertible_to<Latitude>;
        { T::Inverse(len, iter_threshold, enable_precorrection) } -> std::convertible_to<Latitude>;
    };

    template <PrincipleCurvatureRadiiCoeffConcept PCR>
    struct Impl
    {
        static Latitude Inverse(double len, double iter_threshold, bool enable_precorrectoin)
        {
            using coeff = QuarterArcCoeff<PCR>;

            using Utils::Angles::FromDMS;

            double Bf_cur = FromDMS(len / coeff::a0), Bf_next = Bf_cur, FB = 0; // rad
            double dB = 0.0;
            if (enable_precorrectoin)
            {
                dB = PreCorrection::MeridianArcBottom<PCR::ellipsoid_type>(len);
            }

            if (ApproxEq(Bf_cur, 0.0))
            {
                return Bf_cur;
            }

            do
            {
                Bf_cur = Bf_next;
                FB =
                    -coeff::a2 * gcem::sin(2 * Bf_cur) +
                    coeff::a4 * gcem::sin(4 * Bf_cur) -
                    coeff::a6 * gcem::sin(6 * Bf_cur);
                Bf_next = FromDMS((len - FB) / coeff::a0); // deg -> rad
            } while (Bf_next - Bf_cur >= iter_threshold);
            return Bf_cur + dB; // return rad
        }

        static Latitude Inverse(double len)
        {
            AGTB_NOT_IMPLEMENT();
        }

        static double Forward(Latitude _B)
        {
            using coeff = QuarterArcCoeff<PCR>;

            double a0 = coeff::a0,
                   a2 = coeff::a2,
                   a4 = coeff::a4,
                   a6 = coeff::a6;
            double B = _B.Rad(),
                   sin2B = gcem::sin(2 * B),
                   sin4B = gcem::sin(4 * B),
                   sin6B = gcem::sin(6 * B);
            double X = a0 * Utils::Angles::ToDegrees(B) -
                       a2 * sin2B +
                       a4 * sin4B -
                       a6 * sin6B;
            return X;
        }
    };

    template <EllipsoidType __ellipsoid_type>
    struct CheckImpl
    {
        using __PCR = PrincipleCurvatureRadiiCoeff<__ellipsoid_type>;
        using __Impl = Impl<__PCR>;

        template <ImplConcept __Impl_P>
        struct DoCheck
        {
        };

        using __Check = DoCheck<__Impl>;
    };

    template <EllipsoidType __ellipsoid_type>
    using CheckedImpl = CheckImpl<__ellipsoid_type>::__Impl;
}

/**
 * @brief Forward solve a quarter of meridian length from bottom point latitude
 *
 * @tparam __ellipsoid_type
 * @param B
 * @return double
 */
template <EllipsoidType __ellipsoid_type>
double MeridianArcLength(Latitude B)
{
    using Impl = QuarterArcImpl::CheckedImpl<__ellipsoid_type>;
    return Impl::Forward(B);
}

/**
 * @brief Inverse solve bottom point latitude from a quarter of meridian length
 *
 * @tparam __ellipsoid_type
 * @tparam __precorrection_level
 * @param len
 * @param iter_threshold
 * @param enable_precorrectoin
 * @return `Latitude` of bottom point
 */
template <EllipsoidType __ellipsoid_type>
Latitude MeridianArcBottom(double len, double iter_threshold, bool enable_precorrectoin = true)
{
    using Impl = QuarterArcImpl::CheckedImpl<__ellipsoid_type>;
    return Impl::Inverse(len, iter_threshold, enable_precorrectoin);
}

AGTB_GEODESY_END

#endif