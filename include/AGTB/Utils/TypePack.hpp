#ifndef __AGTB_UTILS_TYPE_PACK_HPP__
#define __AGTB_UTILS_TYPE_PACK_HPP__

#include "../details/Macros.hpp"

AGTB_UTILS_BEGIN

template <typename... __args>
struct TypePack
{
private:
    using Pack = std::tuple<__args...>;

public:
    template <std::size_t __idx>
    using At = std::tuple_element_t<__idx, Pack>;
};

AGTB_UTILS_END

AGTB_BEGIN

using Utils::TypePack;

AGTB_END

#endif