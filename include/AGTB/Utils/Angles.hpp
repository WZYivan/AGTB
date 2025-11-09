#ifndef __AGTB_UTILS_ANGLES_HPP__
#define __AGTB_UTILS_ANGLES_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Error.hpp"

#include <gcem.hpp>
#include <numbers>
#include <tuple>

AGTB_UTILS_BEGIN

namespace Angles
{
    constexpr double Deg2Rad = std::numbers::pi / 180;
    constexpr double Rad2Deg = 180 / std::numbers::pi;
    constexpr double Pi = std::numbers::pi;

    constexpr inline bool IsValidMS(double minutes, double seconds)
    {
        return !(minutes < 0 || minutes > 59 || seconds < 0 || seconds >= 60);
    }

    constexpr inline double FromDMS(double degrees = 0, double minutes = 0, double seconds = 0)
    {
        if (minutes < 0 || minutes > 59 || seconds < 0 || seconds >= 60)
        {
            AGTB_THROW(std::domain_error, std::format("[{}:{}:{}] is not a valid DMS", degrees, minutes, seconds));
        }

        bool neg = gcem::signbit(degrees);
        degrees = gcem::abs(degrees);
        double all_degrees = (degrees * 3600 + minutes * 60 + seconds) / 3600.0;
        double rad = all_degrees * Deg2Rad;
        return (!neg) ? rad : -rad;
    }

    inline constexpr double
        rad_45d = FromDMS(45),
        rad_90d = FromDMS(90),
        rad_180d = FromDMS(180);

    constexpr inline double ToDegrees(double rad)
    {
        return rad * Rad2Deg;
    }
    constexpr inline double ToSeconds(double rad)
    {
        return ToDegrees(rad) * 3600.0;
    }

    constexpr inline auto ToSignDMS(double rad)
    {
        bool neg = gcem::signbit(rad);
        rad = gcem::abs(rad);
        double all_seconds = ToSeconds(rad);
        double
            degrees = gcem::floor(all_seconds / 3600),
            minutes = gcem::floor((all_seconds - degrees * 3600) / 60),
            seconds = all_seconds - degrees * 3600 - minutes * 60;
        return std::make_tuple(neg, degrees, minutes, seconds);
    }

    constexpr inline auto ToDMS(double rad)
    {
        auto [neg, d, m, s] = ToSignDMS(rad);
        if (!neg)
        {
            return std::make_tuple(d, m, s);
        }

        if (d != 0)
        {
            return std::make_tuple(-d, m, s);
        }
        else if (m != 0)
        {
            return std::make_tuple(d, -m, s);
        }
        else
        {
            return std::make_tuple(d, m, -s);
        }
    }

    constexpr inline double NormalizedSym(double rad)
    {
        double result = gcem::fmod(rad + Pi, 2.0 * Pi);
        if (result < 0)
        {
            result += 2.0 * Pi;
        }
        return result - Pi;
    }

    constexpr inline double NormalizedStd(double rad)
    {
        double result = gcem::fmod(rad, 2.0 * Pi);
        if (result < 0)
        {
            result += 2.0 * Pi;
        }
        return result;
    }

    class Angle
    {
    private:
        double seconds;

        constexpr static double d360 = 3600 * 360;

    public:
        constexpr Angle(double _d, double _m, double _s)
        {
            seconds = gcem::abs(_d) * 3600 + _m * 60 + _s;
            seconds = gcem::signbit(_d) ? -seconds : seconds;
        }
        constexpr Angle(double s)
            : Angle(0, 0, s)
        {
        }
        constexpr Angle()
            : Angle(0, 0, 0)
        {
        }
        constexpr Angle(std::initializer_list<double> il)
            : Angle(
                  il.size() == 3 ? *il.begin() : (AGTB_THROW(Utils::constructor_error, "Initializer list size must be 3"), 0.0),
                  il.size() == 3 ? *(il.begin() + 1) : 0.0,
                  il.size() == 3 ? *(il.begin() + 2) : 0.0)
        {
        }
        static constexpr Angle FromRad(double rad)
        {
            return Angle(ToSeconds(rad));
        }
        ~Angle() = default;

        std::string ToString() const noexcept
        {
            auto [d, m, s] = DMS();
            return std::format("{}d{}m{}s", d, m, s);
        }

        constexpr inline double Degrees() const noexcept
        {
            return seconds / 3600;
        }
        constexpr inline double Minutes() const noexcept
        {
            return gcem::fmod(seconds, 3600.0) / 60;
        }
        constexpr inline double Seconds() const noexcept
        {
            return gcem::fmod(seconds, 60.0);
        }
        std::tuple<double, double, double> DMS() const noexcept
        {
            return ToDMS(Rad());
        }
        constexpr inline double Rad() const noexcept
        {
            return FromDMS(Degrees());
        }

        friend inline Angle operator+(Angle left, Angle right);
        friend inline Angle operator-(Angle left, Angle right);
        friend inline Angle operator*(double left, Angle right);
        friend inline Angle operator*(Angle left, double right);
        friend inline double operator/(Angle left, Angle right);
        friend inline Angle operator/(Angle left, double scale);
        friend inline Angle operator%(Angle left, Angle right);
        friend inline Angle operator-(Angle a);

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
        constexpr double Sign() const noexcept
        {
            return seconds < 0;
        }
        constexpr double Abs() const noexcept
        {
            return gcem::abs(Rad());
        }

        constexpr auto operator<=>(const Angle &rhs) const
        {
            return seconds <=> rhs.seconds;
        }

        constexpr bool operator==(const Angle &rhs) const
        {
            return std::abs(seconds - rhs.seconds) < std::numeric_limits<double>::epsilon();
        }

        Angle NormStd()
        {
            double s = gcem::fmod(seconds, d360);
            if (s < 0)
            {
                s += d360;
            }
            else if (s > d360)
            {
                s -= d360;
            }

            return Angle(s);
        }
    };

    Angle operator+(Angle left, Angle right)
    {
        return Angle(left.seconds + right.seconds);
    }

    Angle operator-(Angle left, Angle right)
    {
        return Angle(left.seconds - right.seconds);
    }

    Angle operator*(double left, Angle right)
    {
        return Angle(right.seconds * left);
    }

    Angle operator*(Angle left, double right)
    {
        return right * left;
    }

    double operator/(Angle left, Angle right)
    {
        return left.seconds / right.seconds;
    }

    Angle operator/(Angle left, double scale)
    {
        return Angle(left.seconds / scale);
    }

    Angle operator%(Angle left, Angle right)
    {
        return Angle(gcem::fmod(left.seconds, right.seconds));
    }

    inline Angle operator-(Angle a)
    {
        return Angle(-a.seconds);
    }

    constexpr Angle
        A180d = Angle(180, 0, 0),
        A90d = Angle(90, 0, 0),
        A45d = Angle(45, 0, 0);

}

AGTB_UTILS_END

#endif