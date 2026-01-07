#ifndef __AGTB_CONTAINER_PROPERTY_TREE_HPP__
#define __AGTB_CONTAINER_PROPERTY_TREE_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Concept.hpp"
#include <boost/property_tree/ptree.hpp>
#include <ranges>

AGTB_CONTAINER_BEGIN

using PropTree = boost::property_tree::ptree;
using PropPath = boost::property_tree::path;

struct PTree
{
    template <typename __value_type, typename __ptree>
    static bool HasValue(const __ptree &ptree, const PropPath &path)
    {
        return ptree.template get_optional<__value_type>(path).has_value();
    }

    template <typename __ptree>
    static bool HasMap(const __ptree &ptree, const PropPath &path)
    {
        return ptree.get_child_optional(path).has_value();
    }

    template <typename __ptree>
    static bool HasArray(const __ptree &ptree, const PropPath &path)
    {
        return ptree.get_child_optional(path).has_value();
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

    template <StdContainerLike __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree)
    {
        return ArrayView(ptree) |
               std::views::transform([](const auto &sub)
                                     { return Value<typename __container::value_type>(sub); }) |
               std::ranges::to<__container>();
    }

    template <StdContainerLike __container, typename __ptree>
    static __container ArrayTo(const __ptree &ptree, const PropPath &path)
    {
        return ArrayView(ptree, path) |
               std::views::transform([](const auto &sub)
                                     { return Value<typename __container::value_type>(sub); }) |
               std::ranges::to<__container>();
    }
};

#define AGTB_THROW_IF_PTREE_VALUE_KEY_INVALID(__ptree, __key, __type) \
    if (!::AGTB::Container::PTree::HasValue<__type>(__ptree, __key))  \
    {                                                                 \
        AGTB_THROW(::AGTB::Errors::JsonKeyError, __key);              \
    }

#define AGTB_THROW_IF_PTREE_ARRAY_KEY_INVALID(__ptree, __key) \
    if (!::AGTB::Container::PTree::HasArray(__ptree, __key))  \
    {                                                         \
        AGTB_THROW(::AGTB::Errors::JsonKeyError, __key);      \
    }

#define AGTB_THROW_IF_PTREE_MAP_KEY_INVALID(__ptree, __key) \
    if (!::AGTB::Container::PTree::HasMap(__ptree, __key))  \
    {                                                       \
        AGTB_THROW(::AGTB::Errors::JsonKeyError, __key);    \
    }

AGTB_CONTAINER_END

AGTB_BEGIN

using Container::PropPath;
using Container::PropTree;
using Container::PTree;

AGTB_END

#endif