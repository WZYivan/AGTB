#ifndef __AGTB_CONTAINER_PROPERTY_TREE_HPP__
#define __AGTB_CONTAINER_PROPERTY_TREE_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Concept.hpp"
#include <boost/property_tree/ptree.hpp>
#include <ranges>
#include <map>
#include <set>
#include <boost/assign.hpp>

AGTB_CONTAINER_BEGIN

using PropTree = boost::property_tree::ptree;
using PropPath = boost::property_tree::path;
using PropPathSet = std::set<std::string>;
using PropPathAliasMap = std::map<std::string, PropPathSet>;

struct PTree
{
    template <typename __value_type, typename __ptree>
    static bool HasValue(const __ptree &ptree, const PropPath &path)
    {
        return ptree.template get_optional<__value_type>(path).has_value();
    }

    template <typename __value_type, typename __ptree>
    static void ValidateValue(const __ptree &ptree, const std::string &path)
    {
        if (!HasValue<__value_type>(ptree, path))
        {
            AGTB_THROW(JsonKeyError, path);
        }
    }

    template <typename __value_type, typename __ptree>
    static void ValidateValue(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
        if (HasValue<__value_type>(ptree, path))
        {
            return;
        }

        if (!map.contains(path))
        {
            AGTB_THROW(JsonKeyError, std::format("({}) or alias", path));
        }

        for (const auto &alias : map.at(path))
        {
            if (HasValue<__value_type>(ptree, alias))
            {
                return;
            }
        }

        AGTB_THROW(JsonKeyError, std::format("({}) or alias", path));
    }

    template <typename __ptree>
    static bool HasMap(const __ptree &ptree, const PropPath &path)
    {
        return ptree.get_child_optional(path).has_value();
    }

    template <typename __ptree>
    static void ValidateMap(const __ptree &ptree, const std::string &path)
    {
        if (!HasMap(ptree, path))
        {
            AGTB_THROW(JsonKeyError, path);
        }
    }

    template <typename __ptree>
    static void ValidateMap(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
        if (HasMap(ptree, path))
        {
            return;
        }

        if (!map.contains(path))
        {
            AGTB_THROW(JsonKeyError, std::format("({}) or alias", path));
        }

        for (const auto &alias : map.at(path))
        {
            if (HasMap(ptree, alias))
            {
                return;
            }
        }

        AGTB_THROW(JsonKeyError, std::format("({}) or alias", path));
    }

    template <typename __ptree>
    static bool HasArray(const __ptree &ptree, const PropPath &path)
    {
        auto opt = ptree.get_child_optional(path);
        return opt.has_value() && opt.get().begin()->first.empty();
    }

    template <typename __ptree>
    static void ValidateArray(const __ptree &ptree)
    {
        ValidateMap<__ptree>(ptree, "");
    }

    template <typename __ptree>
    static void ValidateArray(const __ptree &ptree, const std::string &path)
    {
        if (!HasArray(ptree, path))
        {
            AGTB_THROW(JsonKeyError, path);
        }
    }

    template <typename __ptree>
    static void ValidateArray(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
        if (HasArray(ptree, path))
        {
            return;
        }

        if (!map.contains(path))
        {
            AGTB_THROW(JsonKeyError, std::format("({}) or alias", path));
        }

        for (const auto &alias : map.at(path))
        {
            if (HasArray(ptree, alias))
            {
                return;
            }
        }

        AGTB_THROW(JsonKeyError, std::format("({}) or alias", path));
    }

    template <typename __value_type, typename __ptree>
    static __value_type Value(const __ptree &ptree)
    {
        return ptree.template get_value<__value_type>();
    }

    template <typename __value_type, typename __ptree>
    static __value_type Value(const __ptree &ptree, const PropPath &path)
    {
        return ptree.template get<__value_type>(path);
    }

    template <typename __value_type, typename __ptree>
    static __value_type Value(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
        if (HasValue<__value_type, __ptree>(ptree, path))
        {
            return Value<__value_type, __ptree>(ptree, path);
        }

        for (const auto &key : map.at(path))
        {
            if (HasValue<__value_type, __ptree>(ptree, key))
            {
                return Value<__value_type, __ptree>(ptree, key);
            }
        }
    }

    template <typename __value_type, typename __ptree>
    static __value_type Value(const __ptree &ptree, const PropPath &path, const __value_type &default_value)
    {
        return ptree.template get<__value_type>(path, default_value);
    }

    template <typename __ptree>
    static auto MapView(const __ptree &ptree)
    {
        return std::ranges::subrange(ptree.begin(), ptree.end());
    }

    template <typename __ptree>
    static auto MapView(const __ptree &ptree, const PropPath &path)
    {
        return MapView(ptree.get_child(path));
    }

    template <typename __ptree>
    static auto MapView(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
        if (HasMap(ptree, path))
        {
            return MapView(ptree, path);
        }

        for (const auto &key : map.at(path))
        {
            if (HasMap(ptree, key))
            {
                return MapView(ptree, key);
            }
        }
    }

    template <typename __ptree>
    static auto ArrayView(const __ptree &ptree)
    {
        return MapView(ptree) |
               std::views::transform([](const auto &kv)
                                     { return kv.second; });
    }

    template <typename __ptree>
    static auto ArrayView(const __ptree &ptree, const PropPath &path)
    {
        return MapView(ptree, path) |
               std::views::transform([](const auto &kv)
                                     { return kv.second; });
    }

    template <typename __ptree>
    static auto ArrayView(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
#if (AGTB_DEBUG)
        std::println("{}", "into function with map");
#endif
        if (HasArray(ptree, path))
        {
#if (AGTB_DEBUG)
            std::println("Array key: {}", path);
#endif
            return ArrayView(ptree, path);
        }

        for (const auto &key : map.at(path))
        {
            if (HasArray(ptree, key))
            {
#if (AGTB_DEBUG)
                std::println("Array key: {}", key);
#endif
                return ArrayView(ptree, key);
            }
        }
    }

#if (AGTB_EXP) || (true)
    template <HasTypeName::ValueType __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree)
    {
        return ArrayView(ptree) |
               std::views::transform([](const auto &sub)
                                     { return Value<ExtractTypeName::ValueType<__container>>(sub); }) |
               std::ranges::to<__container>();
    }

    template <HasTypeName::ValueType __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree, const PropPath &path)
    {
        return ArrayView(ptree, path) |
               std::views::transform([](const auto &sub)
                                     { return Value<ExtractTypeName::ValueType<__container>>(sub); }) |
               std::ranges::to<__container>();
    }

    template <HasTypeName::ValueType __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree, const std::string &path, const PropPathAliasMap &map)
    {
        return ArrayView(ptree, path, map) |
               std::views::transform([](const auto &sub)
                                     { return Value<ExtractTypeName::ValueType<__container>>(sub); }) |
               std::ranges::to<__container>();
    }
#else
    template <typename __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree)
    {
        return ArrayView(ptree) |
               std::views::transform([](const auto &sub)
                                     { return Value<typename __container::value_type>(sub); }) |
               std::ranges::to<__container>();
    }

    template <typename __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree, const PropPath &path)
    {
        return ArrayView(ptree, path) |
               std::views::transform([](const auto &sub)
                                     { return Value<typename __container::value_type>(sub); }) |
               std::ranges::to<__container>();
    }
#endif
};

namespace PTreeExt
{
    class PropPathAliasMapSpecializedInserter
    {
    private:
        PropPathAliasMap &map;
        std::string key;

    public:
        ~PropPathAliasMapSpecializedInserter() = default;
        PropPathAliasMapSpecializedInserter(PropPathAliasMap &_map, const std::string &_key) : map(_map), key(_key)
        {
            if (!map.contains(key))
            {
                map.insert_or_assign(key, PropPathSet{});
            }
        }

        PropPathAliasMapSpecializedInserter operator()(const std::string &alias)
        {
            auto &set = map.at(key);
            if (set.contains(key))
            {
                return *this;
            }
            set.insert(alias);
            return *this;
        }
    };

    auto DefAlias(PropPathAliasMap &map, const std::string &key)
    {
        return PropPathAliasMapSpecializedInserter(map, key);
    }

    class EnableDefAliasBase
    {
    private:
        PropPathAliasMap PTreeExt_EnableDefAliasBase_m_map;

    public:
        EnableDefAliasBase() = default;
        virtual ~EnableDefAliasBase() = default;

        auto DefAlias(const std::string &key)
        {
            return PTreeExt::DefAlias(AliasMap(), key);
        }

        const PropPathAliasMap &AliasMap() const noexcept
        {
            return PTreeExt_EnableDefAliasBase_m_map;
        }

        PropPathAliasMap &AliasMap() noexcept
        {
            return PTreeExt_EnableDefAliasBase_m_map;
        }
    };

    template <typename T>
    concept DefAliasEnabled = requires(T t) {
        { t.AliasMap() } -> std::convertible_to<PropPathAliasMap>;
        { t.DefAlias(std::string("key")) } -> std::convertible_to<PropPathAliasMapSpecializedInserter>;
    };
}

AGTB_CONTAINER_END

AGTB_BEGIN

using Container::PropPath;
using Container::PropPathAliasMap;
using Container::PropTree;
using Container::PTree;
namespace PTreeExt = Container::PTreeExt;

AGTB_END

#endif