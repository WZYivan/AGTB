#ifndef __AGTB_UTILS_CONCEPT_HPP__
#define __AGTB_UTILS_CONCEPT_HPP__

#include "../details/Macros.hpp"

#include <type_traits>

AGTB_BEGIN

namespace detail
{
    template <typename T>
    struct Unqualified
    {
        using Type = std::remove_cvref_t<T>;
    };
}

/**
 * @brief Type with static member Invoke
 *
 * @tparam T constrained type
 * @tparam RT return type of Invoke
 * @tparam PT parameter type pack of Invoke
 */
// template <typename T, typename RT, typename... PT>
// concept InvokerConcept = requires(PT... p) {
//     { T::Invoke(p...) } -> std::convertible_to<RT>;
// };

template <typename T, typename... Tp>
concept IsOneOf = (std::is_same_v<T, Tp> || ...);

template <typename T>
using UnqualifiedType = detail::Unqualified<T>::Type;

AGTB_END

#endif