#ifndef __AGTB_IO_JSON_HPP__
#define __AGTB_IO_JSON_HPP__

#include "../Container/PropertyTree.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <filesystem>

AGTB_IO_BEGIN

template <typename __ptree>
void ReadJson(std::filesystem::path fpath, __ptree &root)
{
    if (!std::filesystem::exists(fpath))
    {
        AGTB_THROW(std::invalid_argument, std::format("file `{}` not exists", fpath.string()));
    }
    boost::property_tree::read_json(fpath, root);
}

template <typename __ptree>
void ReadJson(std::basic_istream<typename __ptree::key_type::value_type> &is, __ptree &root)
{
    boost::property_tree::read_json(is, root);
}

template <typename __ptree>
void PrintJson(const __ptree &json, const std::string &filename, const std::locale &loc = std::locale(), bool pretty = true)
{
    boost::property_tree::write_json(filename, json, loc, pretty);
}

template <typename __ptree>
void PrintJson(const __ptree &json, std::basic_ostream<typename __ptree::key_type::value_type> &os = std::cout, bool pretty = true)
{
    boost::property_tree::write_json(os, json, pretty);
}

template <typename __ptree, typename __parser, typename __target>
concept SupportParse = requires(__ptree json) {
    { __parser::Parse(json) } -> std::convertible_to<__target>;
};

template <typename __ptree, typename __parser, typename __target>
concept SupportParseConfig = requires(__parser parser, __ptree json) {
    { parser.ParseConfig(json) } -> std::convertible_to<__target>;
};

template <typename __parser>
concept GiveExpectJsonFormat = requires {
    { __parser::Expect() } -> std::convertible_to<std::string>;
};

template <typename __ptree, typename __parser, typename __target>
concept SupportParseFromJson =
    (SupportParse<__ptree, __parser, __target> ||
     SupportParseConfig<__ptree, __parser, __target>) &&
    GiveExpectJsonFormat<__parser>;

template <typename __target>
struct JsonParser
{
    AGTB_TEMPLATE_NOT_SPECIALIZED();
};

template <typename __target, typename __ptree>
__target ParseJson(const __ptree &json)
    requires SupportParseFromJson<__ptree, JsonParser<__target>, __target>
{
    try
    {
        if constexpr (SupportParse<__ptree, JsonParser<__target>, __target>)
        {
            return JsonParser<__target>::Parse(json);
        }
        else
        {
            return JsonParser<__target>{}.ParseConfig(json);
        }
    }
    catch (JsonKeyError &e)
    {
        AGTB_THROW(JsonKeyError, std::format("{}\nWe expect:\n{}", e.what(), JsonParser<__target>::Expect()));
    }
}

template <typename __target, typename __ptree>
__target ParseJson(const __ptree &json, const JsonParser<__target> &parser)
    requires SupportParseFromJson<__ptree, JsonParser<__target>, __target>
{
    try
    {
        if constexpr (SupportParseConfig<__ptree, JsonParser<__target>, __target>)
        {
            return parser.ParseConfig(json);
        }
        else
        {
            return JsonParser<__target>::Parse(json);
        }
    }
    catch (JsonKeyError &e)
    {
        AGTB_THROW(JsonKeyError, std::format("{}\nWe expect:\n{}", e.what(), JsonParser<__target>::Expect()));
    }
}

/**
 * @brief This macro is used to literally def the default key you expect in json file.
 * For example, you expect a "key" in json file, then you write `AGTB_JSON_PARSER_DEF_KEY(key)`
 * inside the specialized `JsonParser`, and you can use static method `Key__key()` to access
 * this key, it will literally return "key". For others, such as "aboba", `Key__aboba` returns
 * "aboba". With this macro, `JsonParser` is more friendly to those using IDE with hint and work with `PTree::EnableDefAliasBase`.
 * In such situation, you want to parse json to a AGTB defined type tah not familiar with and define alias of its internal defined key,
 * just get corresponding instantiation, call `Key__*()`, and you get the key!
 *
 */
#define AGTB_JSON_PARSER_DEF_KEY(__key)        \
    static std::string Key__##__key() noexcept \
    {                                          \
        return #__key;                         \
    }

AGTB_IO_END

#endif