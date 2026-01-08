#ifndef __AGTB_UTILS_ASSERT_HPP__
#define __AGTB_UTILS_ASSERT_HPP__

#include "../details/Macros.hpp"
#include "Concept.hpp"

#include <concepts>
#include <cassert>

AGTB_BEGIN

namespace Assert
{
    template <typename T1, typename T2>
    void SameAs()
    {
        static_assert(std::same_as<T1, T2>);
    }

    template <typename T1, typename T2>
    void NotSameAs()
    {
        static_assert(!std::same_as<T1, T2>);
    }

    template <HasEqual T>
    void Eq(const T &lhv, const T &rhv)
    {
        assert(lhv == rhv);
    }

    template <HasEqual T>
    void NotEq(const T &lhv, const T &rhv)
    {
        assert(!(lhv == rhv));
    }

    template <HasGtAndLt T>
    void Gt(const T &lhv, const T &rhv)
    {
        assert(lhv > rhv);
    }

    template <HasGtAndLt T>
    void GtEq(const T &lhv, const T &rhv)
    {
        assert(!(lhv < rhv));
    }

    template <HasGtAndLt T>
    void Lt(const T &lhv, const T &rhv)
    {
        assert(lhv < rhv);
    }

    template <HasGtAndLt T>
    void LtEq(const T &lhv, const T &rhv)
    {
        assert(!(lhv > rhv));
    }
}

AGTB_END

#endif