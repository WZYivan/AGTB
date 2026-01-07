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
    if constexpr (SupportParse<__ptree, JsonParser<__target>, __target>)
    {
        return JsonParser<__target>::Parse(json);
    }
    else
    {
        return JsonParser<__target>{}.ParseConfig(json);
    }
}

template <typename __target, typename __ptree>
__target ParseJson(const __ptree &json, const JsonParser<__target> &parser)
    requires SupportParseFromJson<__ptree, JsonParser<__target>, __target>
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

#define AGTB_THROW_IN_JSON_PARSER(__key) \
    AGTB_THROW(::AGTB::Errors::JsonKeyError, std::format("Key: [{}] not found. We expect json like below:\n{}\n", __key, JsonParser ::Expect()))

#define AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(__ptree, __key, __type)  \
    if (!::AGTB::Container::PTree::HasValue<__type>(__ptree, __key)) \
    {                                                                \
        AGTB_THROW_IN_JSON_PARSER(__key);                            \
    }

#define AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(__ptree, __key)  \
    if (!::AGTB::Container::PTree::HasArray(__ptree, __key)) \
    {                                                        \
        AGTB_THROW_IN_JSON_PARSER(__key);                    \
    }

#define AGTB_JSON_PARSER_VALIDATE_MAP_KEY(__ptree, __key)  \
    if (!::AGTB::Container::PTree::HasMap(__ptree, __key)) \
    {                                                      \
        AGTB_THROW_IN_JSON_PARSER(__key);                  \
    }

AGTB_IO_END

#endif