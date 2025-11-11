#ifndef __AGTB_ADJUSTMENT_REFINE_CORRECTIONS_HPP__
#define __AGTB_ADJUSTMENT_REFINE_CORRECTIONS_HPP__

#include "Base.hpp"
#include "../Utils/Angles.hpp"
#include <vector>

#include <print>

AGTB_ADJUSTMENT_BEGIN

using Utils::Angles::Angle;

/**
 * @brief Refine corrections according to residual and its sign
 *
 * @tparam value_type
 * @param vec std::vector of corrections
 * @param dif residual
 * @param dif_sign sign of residual. true -> ( dif <0 )
 * @return size_t Index if refined correction.
 */
template <typename value_type>
size_t RefineCorrections(std::vector<value_type> &vec, value_type dif, bool dif_sign) noexcept
{
    if (dif_sign)
    {
        size_t min = 0;
        for (auto i = 1; i != vec.size(); ++i)
        {
            if (vec.at(i) < vec.at(min))
            {
                min = i;
            }
        }
        vec.at(min) = vec.at(min) - dif;
        return min;
    }
    else
    {
        size_t max = 0;
        for (auto i = 1; i != vec.size(); ++i)
        {
            if (vec.at(i) > vec.at(max))
            {
                max = i;
            }
        }
        vec.at(max) = vec.at(max) - dif;
        return max;
    }
}

template <typename value_type>
bool RefineCorrections(std::vector<value_type> &vec, value_type dif, bool dif_sign, int place) noexcept
{
    value_type unit = MinUnit(place);
    dif = TakePlace(dif, place);
    unit = TakePlace(unit, place);
    unit = dif_sign ? -unit : unit;
    int count = dif / unit;

    while (count-- > 0)
    {
        size_t idx = 0;

        if (dif_sign)
        {
            auto min = std::min_element(vec.begin(), vec.end());
            idx = std::distance(vec.begin(), min);
        }
        else
        {
            auto max = std::max_element(vec.begin(), vec.end());
            idx = std::distance(vec.begin(), max);
        }

        vec.at(idx) = TakePlace(vec.at(idx) - unit, place);
    }

    return true;
}

template <>
bool RefineCorrections<Angle>(std::vector<Angle> &vec, Angle dif, bool dif_sign, int place) noexcept
{
    Angle unit = Angle::MinUnit(place);
    dif = Angle(TakePlace(dif.Seconds(), place));
    unit = dif_sign ? -unit : unit;
    int count = dif / unit;

    // std::println("dif = {}, unit = {}, count = {}", dif.ToString(), unit.ToString(), count);

    while (count-- > 0)
    {
        size_t idx = 0;

        if (dif_sign)
        {
            auto min = std::min_element(vec.begin(), vec.end());
            idx = std::distance(vec.begin(), min);
        }
        else
        {
            auto max = std::max_element(vec.begin(), vec.end());
            idx = std::distance(vec.begin(), max);
        }

        vec.at(idx) = (vec.at(idx) - unit).TakePlace(place);
    }

    return true;
}

AGTB_ADJUSTMENT_END

#endif