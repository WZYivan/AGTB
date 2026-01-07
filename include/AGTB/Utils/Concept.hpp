#ifndef __AGTB_UTILS_CONCEPT_HPP__
#define __AGTB_UTILS_CONCEPT_HPP__

#include "../details/Macros.hpp"

#include <type_traits>

AGTB_BEGIN

template <typename T, typename... Tp>
concept IsOneOf = (std::is_same_v<T, Tp> || ...);

template <typename T>
struct Unqualified
{
    using Type = std::remove_cvref_t<T>;
};

template <typename T>
using UnqualifiedType = Unqualified<T>::Type;

template <typename __to_be_qualified, typename __like_this>
struct ConstLike
{
    using Type = std::conditional_t<
        std::is_const_v<__like_this>,
        std::add_const_t<__to_be_qualified>,
        __to_be_qualified>;
};

template <typename __to_be_qualified, typename __like_this>
using ConstLikeType = ConstLike<__to_be_qualified, __like_this>::Type;

template <typename __container>
concept StdContainerLike = requires {
    typename __container::value_type;
};

AGTB_END

#endif