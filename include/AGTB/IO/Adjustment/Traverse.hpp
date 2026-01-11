#ifndef __AGTB_IO_ADJUSTMENT_TRAVERSE_HPP__
#define __AGTB_IO_ADJUSTMENT_TRAVERSE_HPP__

#include "../../Adjustment/Traverse.hpp"
#include "../JSON.hpp"

AGTB_IO_BEGIN

template <>
struct JsonParser<Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>>
    : public PTreeExt::EnableDefAliasBase
{
    using Target = Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>;

    AGTB_JSON_PARSER_DEF_KEY(distances);
    AGTB_JSON_PARSER_DEF_KEY(angles);
    AGTB_JSON_PARSER_DEF_KEY(azi_beg);
    AGTB_JSON_PARSER_DEF_KEY(x_beg);
    AGTB_JSON_PARSER_DEF_KEY(y_beg);

    static std::string Expect()
    {
        return R"(
{
    "distances" : [105.22, 80.18, 129.34, 78.16],
    "angles" : [
        [107, 48, 32], 
        [73, 0, 24], 
        [89, 33, 48], 
        [89, 36, 30]
    ],
    "azi_beg" : [125, 30, 0],
    "x_beg" : 506.32,
    "y_beg" : 215.65
}
        )";
    }

    template <typename __ptree>
    Target ParseConfig(const __ptree &json) const
    {
        const auto &map = AliasMap();
        PTree::ValidateArray(json, Key__distances(), map);
        PTree::ValidateArray(json, Key__angles(), map);
        PTree::ValidateArray(json, Key__azi_beg(), map);
        PTree::ValidateValue<double>(json, Key__x_beg(), map);
        PTree::ValidateValue<double>(json, Key__y_beg(), map);

        return {
            .distances = PTree::ArrayTo<std::vector<double>>(json, Key__distances(), map),
            .angles = PTree::ArrayView(json, Key__angles(), map) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, Key__azi_beg(), map)),
            .x_beg = PTree::Value<double>(json, Key__x_beg(), map),
            .y_beg = PTree::Value<double>(json, Key__y_beg(), map)};
    }

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        PTree::ValidateArray(json, Key__distances());
        PTree::ValidateArray(json, Key__angles());
        PTree::ValidateArray(json, Key__azi_beg());
        PTree::ValidateValue<double>(json, Key__x_beg());
        PTree::ValidateValue<double>(json, Key__y_beg());

        return {
            .distances = PTree::ArrayTo<std::vector<double>>(json, Key__distances()),
            .angles = PTree::ArrayView(json, Key__angles()) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, Key__azi_beg())),
            .x_beg = PTree::Value<double>(json, Key__x_beg()),
            .y_beg = PTree::Value<double>(json, Key__y_beg())};
    }
};

template <>
struct JsonParser<Adjustment::TraverseParam<Adjustment::RouteType::ClosedConnecting>>
    : public PTreeExt::EnableDefAliasBase
{
    using Target = Adjustment::TraverseParam<Adjustment::RouteType::ClosedConnecting>;

    AGTB_JSON_PARSER_DEF_KEY(distances);
    AGTB_JSON_PARSER_DEF_KEY(angles);
    AGTB_JSON_PARSER_DEF_KEY(azi_beg);
    AGTB_JSON_PARSER_DEF_KEY(azi_end);
    AGTB_JSON_PARSER_DEF_KEY(x_beg);
    AGTB_JSON_PARSER_DEF_KEY(y_beg);
    AGTB_JSON_PARSER_DEF_KEY(x_end);
    AGTB_JSON_PARSER_DEF_KEY(y_end);

    static std::string Expect() noexcept
    {
        return R"(
{
    "distances" : [
        63.698, 52.303, 65.584, 56.819, 52.990, 55.375
    ],
    "angles" : [
        [268, 29, 12], 
        [88, 5, 33  ], 
        [91, 53, 17 ], 
        [180, 40, 20], 
        [83, 52, 18 ], 
        [95, 36, 39 ], 
        [271, 22, 53]
    ],
    "azi_beg" : [90, 0, 0],
    "azi_end" : [270, 0, 0],
    "x_beg" : 500,
    "y_beg" : 600,
    "x_end" : 500,
    "y_end" : 600
}
        )";
    }

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        PTree::ValidateArray(json, Key__distances());
        PTree::ValidateArray(json, Key__angles());
        PTree::ValidateArray(json, Key__azi_beg());
        PTree::ValidateArray(json, Key__azi_end());
        PTree::ValidateValue<double>(json, Key__x_beg());
        PTree::ValidateValue<double>(json, Key__y_beg());
        PTree::ValidateValue<double>(json, Key__x_end());
        PTree::ValidateValue<double>(json, Key__y_end());

        return Target{
            .distances = PTree::ArrayTo<std::vector<double>>(json, Key__distances()),
            .angles = PTree::ArrayView(json, Key__angles()) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, Key__azi_beg())),
            .azi_end = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, Key__azi_end())),
            .x_beg = PTree::Value<double>(json, Key__x_beg()),
            .y_beg = PTree::Value<double>(json, Key__y_beg()),
            .x_end = PTree::Value<double>(json, Key__x_end()),
            .y_end = PTree::Value<double>(json, Key__y_end())};
    }

    template <typename __ptree>
    Target ParseConfig(const __ptree &json)
    {
        const auto &map = AliasMap();

        PTree::ValidateArray(json, Key__distances(), map);
        PTree::ValidateArray(json, Key__angles(), map);
        PTree::ValidateArray(json, Key__azi_beg(), map);
        PTree::ValidateArray(json, Key__azi_end(), map);
        PTree::ValidateValue<double>(json, Key__x_beg(), map);
        PTree::ValidateValue<double>(json, Key__y_beg(), map);
        PTree::ValidateValue<double>(json, Key__x_end(), map);
        PTree::ValidateValue<double>(json, Key__y_end(), map);

        return Target{
            .distances = PTree::ArrayTo<std::vector<double>>(json, Key__distances(), map),
            .angles = PTree::ArrayView(json, Key__angles(), map) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, Key__azi_beg(), map)),
            .azi_end = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, Key__azi_end(), map)),
            .x_beg = PTree::Value<double>(json, Key__x_beg(), map),
            .y_beg = PTree::Value<double>(json, Key__y_beg(), map),
            .x_end = PTree::Value<double>(json, Key__x_end(), map),
            .y_end = PTree::Value<double>(json, Key__y_end(), map)};
    }
};
AGTB_IO_END

#endif