#ifndef __AGTB_IO_JSON_HPP__
#define __AGTB_IO_JSON_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Concept.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <filesystem>
#include <generator>

AGTB_IO_BEGIN

namespace detail::JsonIO
{
    namespace fs = std::filesystem;
    namespace pt = boost::property_tree;

    template <typename __ptree>
        requires std::same_as<UnqualifiedType<__ptree>, pt::ptree>
    class PTreeWrapper
    {
    private:
        __ptree root;

    public:
        PTreeWrapper(const __ptree &ptree) : root(ptree)
        {
        }
        ~PTreeWrapper() = default;

        const __ptree &Unwrap() const
        {
            return root;
        }

        __ptree &Unwrap()
        {
            return root;
        }

        /**
         * @brief Give a key, get its view.
         *
         * @tparam __self
         * @param self
         * @param path
         * @return auto
         */
        template <typename __self>
        auto ArrayView(this __self &&self, const pt::path &path)
        {
            auto &range = self.root.get_child(path);
            return std::ranges::subrange(range.begin(), range.end()) |
                   std::views::transform([](auto &kv) -> decltype(auto)
                                         { return PTreeWrapper(kv.second); });
        }

        /**
         * @brief Convert this to view.
         *
         * @tparam __self
         * @param self
         * @return auto
         */
        template <typename __self>
        auto ToArrayView(this __self &&self)
        {
            return std::ranges::subrange(self.root.begin(), self.root.end()) |
                   std::views::transform([](auto &kv) -> decltype(auto)
                                         { return PTreeWrapper(kv.second); });
        }

        /**
         * @brief Give a key, convert its view to a container and return it.
         *
         * @tparam __container
         * @tparam __container::value_type
         * @param self
         * @param path
         * @return __container
         */
        template <typename __container, typename __value_type = typename __container::value_type>
        __container Container(this auto &&self, const pt::path &path)
        {
            return self.ArrayView(path) |
                   std::views::transform([](const auto &v) -> double
                                         { return v.template ToValue<__value_type>(); }) |
                   std::ranges::to<__container>();
        }

        /**
         * @brief Give a key, convert its view to a container and return it.
         *
         * @tparam __container
         * @tparam __value_type
         * @param self
         * @param path
         * @return __container<__value_type>
         */
        template <template <typename> typename __container, typename __value_type>
        __container<__value_type> Container(this auto &&self, const pt::path &path)
        {
            return self.template Container<__container<__value_type>>(path);
        }

        /**
         * @brief Convert this to container.
         *
         * @tparam __container
         * @tparam __container::value_type
         * @param self
         * @return __container
         */
        template <typename __container, typename __value_type = typename __container::value_type>
        __container ToContainer(this auto &&self)
        {
            return self.ToArrayView() |
                   std::views::transform([](const auto &v) -> double
                                         { return v.template ToValue<__value_type>(); }) |
                   std::ranges::to<__container>();
        }

        /**
         * @brief Convert this to container.
         *
         * @tparam __container
         * @tparam __container::value_type
         * @param self
         * @return __container
         */
        template <template <typename> typename __container, typename __value_type>
        __container<__value_type> ToContainer(this auto &&self)
        {
            return self.template ToContainer<__container<__value_type>>();
        }

        bool HasArray(const pt::path &path) const
        {
            return root.get_child_optional(path).has_value();
        }

        template <typename __value_type>
        __value_type ToValue() const
        {
            return root.template get_value<__value_type>();
        }

        template <typename __value_type>
        __value_type Value(const pt::path &path) const
        {
            return root.template get<__value_type>(path);
        }

        template <typename __value_type>
        __value_type Value(const pt::path &path, const __value_type &default_value) const
        {
            return root.template get<__value_type>(path, default_value);
        }

        template <typename __value_type>
        bool HasValue(const pt::path &path) const
        {
            return root.template get_optional<__value_type>(path).has_value();
        }

        std::string ToString(bool pretty = true) const
        {
            std::ostringstream oss;
            pt::write_json(oss, root, pretty);
            return oss.str();
        }
    };

    template <typename __json, typename __parser, typename __target>
    concept SupportParseFromJson = requires(__json json) {
        { __parser::Parse(json) } -> std::convertible_to<__target>;
    } || requires(__parser parser, __json json) {
        { parser.ParseConfig(json) } -> std::convertible_to<__target>;
    };
}

/**
 * @brief A wrapper of boost::property_tree::ptree
 *
 */
using Json = detail::JsonIO::PTreeWrapper<detail::JsonIO::pt::ptree>;

Json ReadJson(const detail::JsonIO::fs::path &fpath)
{
    if (!detail::JsonIO::fs::exists(fpath))
    {
        AGTB_THROW(std::invalid_argument, std::format("file `{}` not exists", fpath.string()));
    }

    detail::JsonIO::pt::ptree root{};
    detail::JsonIO::pt::read_json(fpath, root);

    return Json(root);
}

void WriteJson(const Json &json, const std::string &filename, const std::locale &loc = std::locale(), bool pretty = true)
{
    detail::JsonIO::pt::write_json(filename, json.Unwrap(), loc, pretty);
}

/**
 * @brief Specialize this struct to define how to parse a type from json. Defaultly using static method `Parse`.
 *
 * @tparam __target
 */
template <typename __target>
struct JsonParser
{
    AGTB_TEMPLATE_NOT_SPECIFIED();
};

/**
 * @brief Default parse.
 *
 * @tparam __target
 * @param json
 * @return __target
 */
template <typename __target>
__target ParseJson(const Json &json)
    requires detail::JsonIO::SupportParseFromJson<Json, JsonParser<__target>, __target>
{
    return JsonParser<__target>::Parse(json);
}

/**
 * @brief Parse using your config. You can add custom members to your `JsonParser` and custom behavior of `ParseConfig`. Pass a
 * object to this function, it will use `ParseConfig` to parse.
 *
 * @tparam __target
 * @param json
 * @param parser
 * @return __target
 */
template <typename __target>
__target ParseJson(const Json &json, const JsonParser<__target> &parser)
    requires detail::JsonIO::SupportParseFromJson<Json, JsonParser<__target>, __target>
{
    return parser.ParseConfig(json);
}

#define AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(__json_obj, __key, __type) \
    if (!__json_obj.HasValue<__type>(__key))                            \
    {                                                                   \
        AGTB_THROW(JsonKeyError, __key);                                \
    }

#define AGTB_THROW_IF_JSON_ARRAY_KEY_INVALID(__json_obj, __key) \
    if (!__json_obj.HasArray(__key))                            \
    {                                                           \
        AGTB_THROW(JsonKeyError, __key);                        \
    }

AGTB_IO_END

#endif