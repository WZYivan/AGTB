#ifndef __AGTB_GEODESY_BASE_GEO_LAT_LON_HPP__
#define __AGTB_GEODESY_BASE_GEO_LAT_LON_HPP__

#include "../../details/Macros.hpp"
#include "../../Utils/Angles.hpp"

#include "gcem.hpp"

AGTB_GEODESY_BEGIN

template <std::floating_point T, typename Derived>
class GeoLatLonBase
{
private:
    T _rad;

public:
    constexpr GeoLatLonBase(T rad) : _rad(rad)
    {
    }
    virtual ~GeoLatLonBase() = default;
    constexpr T Rad() const noexcept
    {
        return _rad;
    }
    virtual constexpr bool IsValid() const noexcept = 0;

    constexpr double Sin() const noexcept
    {
        return gcem::sin(Rad());
    }
    constexpr double Cos() const noexcept
    {
        return gcem::cos(Rad());
    }
    constexpr double Tan() const noexcept
    {
        return gcem::tan(Rad());
    }

    Utils::Angles::Angle ToAngle()
    {
        return Utils::Angles::Angle::FromRad(Rad());
    }

    static Derived FromAngle(const Utils::Angles::Angle &a)
    {
        return Derived(a.Rad());
    }
};

class Latitude : public GeoLatLonBase<double, Latitude>
{
public:
    virtual constexpr bool IsValid() const noexcept override
    {
        return gcem::abs(Rad()) <= max;
    }

    Latitude(double rad) : GeoLatLonBase<double, Latitude>(rad)
    {
        if (!IsValid())
        {
            AGTB_THROW(std::invalid_argument, std::format("Invalid Latitude: \'{}\' => \'{}\'",
                                                          Rad(), Utils::Angles::Angle::FromRad(Rad()).ToString()));
        }
    }

    Latitude(double d, double m, double s) : Latitude(Utils::Angles::FromDMS(d, m, s))
    {
    }

private:
    constexpr static double max = Utils::Angles::FromDMS(90);
};

class Longitude : public GeoLatLonBase<double, Longitude>
{
public:
    virtual constexpr bool IsValid() const noexcept override
    {
        return gcem::abs(Rad()) <= max;
    }

    Longitude(double rad) : GeoLatLonBase<double, Longitude>(rad)
    {
        if (!IsValid())
        {
            AGTB_THROW(std::invalid_argument, std::format("Invalid Longitude: \'{}\' => \'{}\'",
                                                          Rad(), Utils::Angles::Angle::FromRad(Rad()).ToString()));
        }
    }

    Longitude(double d, double m, double s) : Longitude(Utils::Angles::FromDMS(d, m, s))
    {
    }

private:
    constexpr static double max = Utils::Angles::FromDMS(180);
};

AGTB_GEODESY_END

#endif