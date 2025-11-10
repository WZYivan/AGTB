#ifndef __AGTB_UTILS_CONCEPT_HPP__
#define __AGTB_UTILS_CONCEPT_HPP__

#include "../details/Macros.hpp"

AGTB_UTILS_BEGIN

/**
 * @brief Type with static member Invoke
 *
 * @tparam T constrained type
 * @tparam RT return type of Invoke
 * @tparam PT parameter type pack of Invoke
 */
template <typename T, typename RT, typename... PT>
concept InvokerConcept = requires(PT... p) {
    { T::Invoke(p...) } -> std::convertible_to<RT>;
};

template <typename T, typename... Tp>
concept IsOneOf = (std::is_same_v<T, Tp> || ...);

AGTB_UTILS_END

#endif