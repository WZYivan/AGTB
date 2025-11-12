#ifndef __AGTB_GEODESY_PROJECTION_GAUSS_KRUGER_HPP__
#define __AGTB_GEODESY_PROJECTION_GAUSS_KRUGER_HPP__

#include "../Ellipsoid/Geometry.hpp"
#include "../../Utils/Angles.hpp"

AGTB_GEODESY_BEGIN

namespace Projection::GaussKruger
{
    using Utils::Angles::FromDMS;

    class ZoneInterval
    {
    public:
        double interval;

        constexpr ZoneInterval(double val) : interval(val) {}
        constexpr ~ZoneInterval() = default;

        friend constexpr bool operator==(const ZoneInterval &a, const ZoneInterval &b)
        {
            return ApproxEq(a.interval, b.interval);
        }
        friend constexpr bool operator<(const ZoneInterval &a, const ZoneInterval &b)
        {
            return a.interval < b.interval;
        }
    };

    constexpr ZoneInterval Z3(3.0), Z6(6.0);

    template <ZoneInterval interval>
    Longitude CenterLongitude(int zone)
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    }

    template <>
    Longitude CenterLongitude<Z6>(int zone)
    {
        return FromDMS(6 * zone - 3);
    }

    template <>
    Longitude CenterLongitude<Z3>(int zone)
    {
        return FromDMS(3 * zone);
    }

    template <ZoneInterval interval>
    int Zone(Longitude L)
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    }

    template <>
    int Zone<Z6>(Longitude L)
    {
        return int(L.Rad() / FromDMS(6.0)) + 1;
    }

    template <>
    int Zone<Z3>(Longitude L)
    {
        double rad_d3 = FromDMS(3.0);
        return int(L.Rad() / rad_d3) + gcem::fmod(L.Rad(), rad_d3) > FromDMS(1.5) ? 1 : 0;
    }

    template <ZoneInterval Z>
    Longitude CenterLongitude(Longitude l)
    {
        return CenterLongitude<Z>(Zone<Z>(l));
    }
};

AGTB_GEODESY_END

#endif