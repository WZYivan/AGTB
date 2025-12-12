#ifndef __AGTB_GEODESY_DATUM_GEOLATLON_HPP__
#define __AGTB_GEODESY_DATUM_GEOLATLON_HPP__

#include "Units.hpp"
#include "../../Utils/Angles.hpp"

#include <gcem.hpp>

AGTB_GEODESY_BEGIN

template <Units __unit, typename __derived>
class GeoLatLonBase
{
private:
    double _val;

public:
    constexpr static Units unit = __unit;
    using Derived = __derived;

    constexpr GeoLatLonBase(double rad) : _val(rad)
    {
    }
    virtual ~GeoLatLonBase() = default;

    constexpr double Rad() const noexcept
    {
        if constexpr (unit == Units::Radian)
        {
            return _val;
        }
        else if constexpr (unit == Units::Degree)
        {
            return _val * deg2rad;
        }
        else if constexpr (unit == Units::Minute)
        {
            return _val * min2rad;
        }
        else if constexpr (unit == Units::Second)
        {
            return _val * sec2rad;
        }
        else
        {
            AGTB_STATIC_THROW("Unknown units");
        }
    }

    static Derived FromAngle(const Utils::Angles::Angle &a)
    {
        if constexpr (unit == Units::Radian)
        {
            return Derived(a.Rad());
        }
        else if constexpr (unit == Units::Degree)
        {
            return Derived(a.Degrees());
        }
        else if constexpr (unit == Units::Minute)
        {
            return Derived(a.Minutes());
        }
        else if constexpr (unit == Units::Second)
        {
            return Derived(a.Seconds());
        }
        else
        {
            AGTB_STATIC_THROW("Unknown units");
        }
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

    Utils::Angles::Angle ToAngle() const noexcept
    {
        return Utils::Angles::Angle::FromRad(Rad());
    }

    std::string ToString() const noexcept
    {
        return ToAngle().ToString();
    }
};

/**
 * @brief Geodetic latitude in (-90, 90)(deg), store in `rad`
 *
 */
template <Units __unit = Units::Radian>
class Latitude : public GeoLatLonBase<__unit, Latitude<__unit>>
{
public:
    constexpr static Units unit = __unit;

    virtual constexpr bool IsValid() const noexcept override
    {
        return gcem::abs(this->Rad()) <= max;
    }

    Latitude(double rad) : GeoLatLonBase<__unit, Latitude>(rad)
    {
        if (!IsValid())
        {
            AGTB_THROW(std::invalid_argument, std::format("Invalid Latitude: \'{}\' => \'{}\'",
                                                          this->Rad(), Angle::FromRad(this->Rad()).ToString()));
        }
    }

    Latitude(double d, double m, double s) : Latitude(FromDMS(d, m, s))
    {
    }

private:
    constexpr static double max = 90 * deg2rad;
};

/**
 * @brief Geodetic longitude in (-180, 180)(deg), store in `rad`
 *
 */
template <Units __unit = Units::Radian>
class Longitude : public GeoLatLonBase<__unit, Longitude<__unit>>
{
public:
    constexpr static Units unit = __unit;

    virtual constexpr bool IsValid() const noexcept override
    {
        return gcem::abs(this->Rad()) <= max;
    }

    Longitude(double rad) : GeoLatLonBase<__unit, Longitude>(rad)
    {
        if (!IsValid())
        {
            AGTB_THROW(std::invalid_argument, std::format("Invalid Longitude: \'{}\' => \'{}\'",
                                                          this->Rad(), Angle::FromRad(this->Rad()).ToString()));
        }
    }

    Longitude(double d, double m, double s) : Longitude(FromDMS(d, m, s))
    {
    }

private:
    constexpr static double max = 180 * deg2rad;
};

AGTB_GEODESY_END

#endif