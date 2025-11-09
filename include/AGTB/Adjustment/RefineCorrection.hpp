#ifndef __AGTB_ADJUSTMENT_REFINE_CORRECTIONS_HPP__
#define __AGTB_ADJUSTMENT_REFINE_CORRECTIONS_HPP__

#include "Base.hpp"
#include <vector>

AGTB_ADJUSTMENT_BEGIN

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

AGTB_ADJUSTMENT_END

#endif