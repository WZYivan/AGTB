#ifndef __AGTB_UTILS_ANGLES_HPP__
#define __AGTB_UTILS_ANGLES_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Error.hpp"
#include "../Utils/Std/CharConv.hpp"
#include "Math.hpp"

#include <gcem.hpp>
#include <numbers>
#include <tuple>
#include <format>
#include <regex>
#include <charconv>

AGTB_UTILS_BEGIN

namespace Angles
{
    constexpr double Deg2Rad = std::numbers::pi / 180;
    constexpr double Rad2Deg = 180 / std::numbers::pi;
    constexpr double Sec2Rad = Deg2Rad / 3600.0;
    constexpr double Rad2Sec = Rad2Deg * 3600.0;
    constexpr double Pi = std::numbers::pi;

    /**
     * @brief min in [0,59], seconds in [0, 60)
     *
     * @param minutes
     * @param seconds
     * @return true
     * @return false
     */
    constexpr inline bool IsValidMS(double minutes, double seconds)
    {
        return !(minutes < 0 || minutes > 59 || seconds < 0 || seconds >= 60);
    }

    /**
     * @brief Convert dms to rad
     *
     * @param degrees
     * @param minutes
     * @param seconds
     * @return constexpr double
     */
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

    /**
     * @brief Convert rad to degrees
     *
     * @param rad
     * @return constexpr double
     */
    constexpr inline double ToDegrees(double rad)
    {
        return rad * Rad2Deg;
    }

    /**
     * @brief Convert rad to seconds
     *
     * @param rad
     * @return constexpr double
     */
    constexpr inline double ToSeconds(double rad)
    {
        return ToDegrees(rad) * 3600.0;
    }

    /**
     * @brief Convert rad to signed dms
     *
     * @param rad
     * @return constexpr auto
     */
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

    /**
     * @brief Convert rad to dms( auto signed)
     *
     * @param rad
     * @return constexpr auto
     */
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

    /**
     * @brief Normalize rad to (-pi, pi)
     *
     * @param rad
     * @return constexpr double
     */
    constexpr inline double NormalizedSym(double rad)
    {
        double result = gcem::fmod(rad + Pi, 2.0 * Pi);
        if (result < 0)
        {
            result += 2.0 * Pi;
        }
        return result - Pi;
    }

    /**
     * @brief Normalized rad to (0, 2*pi)
     *
     * @param rad
     * @return constexpr double
     */
    constexpr inline double NormalizedStd(double rad)
    {
        double result = gcem::fmod(rad, 2.0 * Pi);
        if (result < 0)
        {
            result += 2.0 * Pi;
        }
        return result;
    }

    /**
     * @brief Represent (d,m,s) angle
     *
     */
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
        /**
         * @brief initializer_list must in size of 3
         *
         * @throw AGTB::Utils::constructor_error : initializer_list.size() != 3
         */
        constexpr Angle(std::initializer_list<double> il)
            : Angle(
                  il.size() == 3 ? *il.begin() : (AGTB_THROW(Utils::constructor_error, "Initializer list size must be 3"), 0.0),
                  il.size() == 3 ? *(il.begin() + 1) : 0.0,
                  il.size() == 3 ? *(il.begin() + 2) : 0.0)
        {
        }
        /**
         * @brief Construct from rad
         *
         * @param rad
         * @return constexpr Angle
         */
        static constexpr Angle FromRad(double rad)
        {
            return Angle(ToSeconds(rad));
        }
        ~Angle() = default;

        /**
         * @brief To string in format "{}d{}m{}s"
         *
         * @return std::string
         */
        std::string ToString() const noexcept
        {
            auto [d, m, s] = DMS();
            return std::format("{}d{}m{}s", d, m, s);
        }

        /**
         * @brief To degrees(double)
         *
         * @return constexpr double
         */
        constexpr inline double Degrees() const noexcept
        {
            return seconds / 3600;
        }
        /**
         * @brief To minutes(double)
         *
         * @return constexpr double
         */
        constexpr inline double Minutes() const noexcept
        {
            return gcem::fmod(seconds, 3600.0) / 60;
        }
        /**
         * @brief To seconds(double)
         *
         * @return constexpr double
         */
        constexpr inline double Seconds() const noexcept
        {
            return gcem::fmod(seconds, 60.0);
        }

        /**
         * @brief Same as Utils::ToDMS
         *
         * @return std::tuple<double, double, double>
         */
        std::tuple<double, double, double> DMS() const noexcept
        {
            return ToDMS(Rad());
        }

        /**
         * @brief Convert this dms to rad
         *
         * @return constexpr double
         */
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

        /**
         * @brief Sign of internal seconds(all) value
         *
         * @return constexpr double
         */
        constexpr double Sign() const noexcept
        {
            return seconds < 0;
        }

        constexpr double Abs() const noexcept
        {
            return gcem::abs(Rad());
        }

        constexpr bool operator==(const Angle &rhs) const noexcept
        {
            return ApproxEq(seconds, rhs.seconds);
        }

        constexpr bool operator!=(const Angle &rhs) const noexcept
        {
            return !(*this == rhs);
        }

        constexpr bool operator>(const Angle &rhs) const noexcept
        {
            return seconds > rhs.seconds;
        }

        constexpr bool operator<(const Angle &rhs) const noexcept
        {
            return seconds < rhs.seconds;
        }
        constexpr bool operator>=(const Angle &rhs) const noexcept
        {
            return seconds >= rhs.seconds;
        }

        constexpr bool operator<=(const Angle &rhs) const noexcept
        {
            return seconds <= rhs.seconds;
        }

        /**
         * @brief Normalize to (0, 360)
         *
         * @return Angle
         */
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

        /**
         * @brief To string in format "dd.mmss..."
         *
         * @param place 0 -> int
         * @return std::string
         */
        std::string ToStringDMS(int place = 0) const noexcept
        {
            auto [d, m, s] = DMS();
            int d_i = Round(d);
            int m_i = Round(m);
            s = AGTB::TakePlace(s, place) * gcem::pow(10, place);
            return std::format("{}.{}{}", d_i, m_i, s);
        }

        /**
         * @brief construct from string "dd.mmss..."
         *
         * @return std::string
         * @throw std::invalid_argument : regex match fail or convert string to double fail
         */
        static Angle FromStringDMS(std::string str)
        {
            std::regex pattern(R"(^([0-9]{1,3}).([0-9]{2})([0-9]{2,})$)");
            std::smatch match;

            if (std::regex_match(str, match, pattern))
            {
                double d, m, s;
                std::string d_str = match[1].str(),
                            m_str = match[2].str(),
                            s_str = match[3].str();

                if (
                    !(Utils::FromString(d_str, d) &&
                      Utils::FromString(m_str, m) &&
                      Utils::FromString(s_str, s)))
                {
                    AGTB_THROW(std::invalid_argument, "Can't convert input str to double");
                }

                if (s_str.size() > 2)
                {
                    s /= gcem::pow(10, s_str.size() - 2);
                }

                return Angle(d, m, s);
            }
            else
            {
                AGTB_THROW(std::invalid_argument, R"(^([0-9]{1,3}).([0-9]{2})([0-9]{2,})$ : match failed)");
            }
        }

        constexpr Angle TakePlace(int place)
        {
            return Angle(AGTB::TakePlace(seconds, place));
        }

        static constexpr Angle MinUnit(int place)
        {
            return Angle(
                AGTB::TakePlace(
                    AGTB::MinUnit(place), place));
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