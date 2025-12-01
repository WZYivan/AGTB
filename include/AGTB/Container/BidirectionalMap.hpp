#ifndef __AGTB_CONTAINER_BIDIRECTIONAL_MAP_HPP__
#define __AGTB_CONTAINER_BIDIRECTIONAL_MAP_HPP__

#include "../details/Macros.hpp"

#include <map>

AGTB_CONTAINER_BEGIN

AGTB_FILE_NOT_IMPLEMENT();

template <
    template <typename K, typename V> typename __map_template,
    typename __key_type,
    typename __value_type>
class BidirectionalMap
{
public:
    using key_type = __key_type;
    using value_type = __value_type;

    template <typename K, typename V>
    using map_template = __map_template<K, V>;

    using kv_map_type = map_template<key_type, value_type>;
    using vk_map_type = map_template<value_type, key_type>;

    using iterator = typename kv_map_type::iterator;

private:
    kv_map_type kv;
    vk_map_type vk;

public:
    ~BidirectionalMap() = default;
    BidirectionalMap() = default;

    template <typename __self>
    auto UnWrap(this __self &&self) -> std::conditional_t<std::is_const_v<__self>, const kv_map_type &, kv_map_type &>
    {
        return self.kv;
    }

    template <typename __self>
    decltype(auto) ValueOf(this __self &&self, const key_type &k)
    {
        return self.kv.at(k);
    }

    template <typename __self>
    decltype(auto) KeyOf(this __self &&self, const value_type &v)
    {
        return self.vk.at(v);
    }

    inline decltype(auto) InsertOrAssign(const key_type &k, value_type &&v)
    {
        vk.insert_or_assign(v, k);
        return kv.insert_or_assign(k, v);
    }

    inline decltype(auto) InsertOrAssign(key_type &&k, value_type &&v)
    {
        vk.insert_or_assign(v, k);
        return kv.insert_or_assign(k, v);
    }

    inline decltype(auto) Erase(const key_type &k)
    {
        const value_type &v = ValueOf(k);
        vk.erase(v);
        return kv.erase(k);
    }

    template <typename __self>
    inline decltype(auto) Begin(this __self &&self)
    {
        return std::make_tuple(self.vk.begin(), self.kv.begin());
    }
};

AGTB_CONTAINER_END

AGTB_BEGIN

using Container::BidirectionalMap;

AGTB_END

#endif