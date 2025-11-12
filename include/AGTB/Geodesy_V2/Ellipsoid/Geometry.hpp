#ifndef __AGTB_GEODESY_ELLIPSOID_GEOMETRY_HPP__
#define __AGTB_GEODESY_ELLIPSOID_GEOMETRY_HPP__

#include "Base.hpp"
#include "../Base/GeoLatLon.hpp"

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

template <EllipsoidType __type>
struct EllipsoidGeometryBase
{
    constexpr static EllipsoidType ellipsoid_type = __type;
};

template <EllipsoidType type>
struct EllipsoidGeometry : public EllipsoidGeometryBase<type>
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

template <EllipsoidGeometryConcept __ep>
std::string ToString()
{
    return std::format("a = {}\n"
                       "b = {}\n"
                       "c = {}\n"
                       "alpha = {}\n"
                       "e1_2 = {}\n"
                       "e2_2 = {}\n",
                       __ep::a, __ep::b, __ep::c, __ep::alpha, __ep::e1_2, __ep::e1_2);
}

struct CurvatureRadiusCollection
{
    double M, N;
    double R()
    {
        return gcem::sqrt(M * N);
    }
};

template <EllipsoidType type>
struct PrincipleCurvatureRadiiCoeff
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
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
    template <EllipsoidGeometryConcept ellipsoid, EllipsoidAlgoOption opt>
    struct Impl
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    };

    template <EllipsoidGeometryConcept ellipsoid>
    struct Impl<ellipsoid, EllipsoidAlgoOption::General>
    {
        static CurvatureRadiusCollection Invoke(Latitude B)
        {
            double sinBp2 = gcem::pow(B.Sin(), 2),
                   a = ellipsoid::a,
                   e1_2 = ellipsoid::e1_2,
                   k = 1.0 - e1_2 * sinBp2;
            double M = a * (1.0 - e1_2) * gcem::pow(k, -1.5),
                   N = a * gcem::pow(k, -0.5);
            return {
                .M = M,
                .N = N};
        }
    };

    template <EllipsoidGeometryConcept ellipsoid>
    struct Impl<ellipsoid, EllipsoidAlgoOption::Specified>
    {
        static CurvatureRadiusCollection Invoke(Latitude B)
        {
            using coeff = PrincipleCurvatureRadiiCoeff<ellipsoid::ellipsoid_type>;
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

    template <EllipsoidType type, EllipsoidAlgoOption opt>
    struct CheckImpl
    {
        using __Geometry = EllipsoidGeometry<type>;
        using __Impl = Impl<__Geometry, opt>;

        template <ImplConcept __Impl_P>
        struct DoCheck
        {
        };

        using __Check = DoCheck<__Impl>;
    };

    template <EllipsoidType type, EllipsoidAlgoOption opt>
    using CheckedImpl = CheckImpl<type, opt>::__Impl;
}

template <EllipsoidType type, EllipsoidAlgoOption opt = EllipsoidAlgoOption::General>
CurvatureRadiusCollection PrincipleCurvatureRadii(Latitude B)
{
    using Impl = PrincipleCurvatureRadiiImpl::CheckedImpl<type, opt>;
    return Impl::Invoke(B);
}

namespace QuarterArcImpl
{
    template <typename T>
    concept ImplConcept = requires(Latitude B) {
        { T::Forward(B) } -> std::convertible_to<double>;
    } && requires(double len, double iter_threshold) {
        { T::Inverse(len) } -> std::convertible_to<Latitude>;
        { T::Inverse(len, iter_threshold) } -> std::convertible_to<Latitude>;
    };

    template <EllipsoidGeometryConcept ellipsoid, EllipsoidAlgoOption opt>
    struct ImplBase
    {
        // TODO: Inverse(2)

        static Latitude Inverse(double len_m)
        {
            AGTB_NOT_IMPLEMENT();
        }

        static double Forward(Latitude _B)
        {
            AGTB_NOT_IMPLEMENT();
        }
    };

    template <EllipsoidGeometryConcept ellipsoid, EllipsoidAlgoOption opt>
    struct Impl
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    };

    template <EllipsoidGeometryConcept ellipsoid>
    struct Impl<ellipsoid, EllipsoidAlgoOption::General> : public ImplBase<Impl<ellipsoid, EllipsoidAlgoOption::General>>
    {
        // TODO: Forward. Inverse(1)
    };

    template <EllipsoidGeometryConcept ellipsoid>
    struct Impl<ellipsoid, EllipsoidAlgoOption::Specified> : public ImplBase<Impl<ellipsoid, EllipsoidAlgoOption::Specified>>
    {
        // TODO: Forward. Inverse(1)
    };

    template <EllipsoidType type, EllipsoidAlgoOption opt>
    struct CheckImpl
    {
        using __Geo = EllipsoidGeometry<type>;
        using __Impl = Impl<__Geo, opt>;

        template <ImplConcept __Impl_P>
        struct DoCheck
        {
        };

        using __Check = DoCheck<__Impl>;
    };

    template <EllipsoidType type, EllipsoidAlgoOption opt>
    using CheckedImpl = CheckImpl<type, opt>::__Impl;
}

AGTB_GEODESY_END

#endif