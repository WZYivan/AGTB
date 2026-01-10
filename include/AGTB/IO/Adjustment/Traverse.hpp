#ifndef __AGTB_IO_ADJUSTMENT_TRAVERSE_HPP__
#define __AGTB_IO_ADJUSTMENT_TRAVERSE_HPP__

#include "../../Adjustment/Traverse.hpp"
#include "../JSON.hpp"

AGTB_IO_BEGIN

template <>
struct JsonParser<Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>>
{
    using Target = Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>;

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        return DoParse(json, "distances", "angles", "azi_beg", "x_beg", "y_beg");
    }

    template <typename __ptree>
    Target ParseConfig(const __ptree &json) const
    {
        return DoParse(json, dis, ang, ab, x, y);
    }
    JsonParser(std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
        : dis(distances), ang(angles), ab(azi_beg), x(x_beg), y(y_beg)
    {
    }
    ~JsonParser() = default;

private:
    std::string dis, ang, ab, x, y;

    template <typename __ptree>
    static Target DoParse(const __ptree &json, std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
    {
        PTree::ValidateArray(json, distances);
        PTree::ValidateArray(json, angles);
        PTree::ValidateArray(json, azi_beg);
        PTree::ValidateValue<double>(json, x_beg);
        PTree::ValidateValue<double>(json, y_beg);

        return {
            .distances = PTree::ArrayTo<std::vector<double>>(json, distances),
            .angles = PTree::ArrayView(json, angles) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, azi_beg)),
            .x_beg = PTree::Value<double>(json, x_beg),
            .y_beg = PTree::Value<double>(json, y_beg)};
    }

public:
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
};

template <>
struct JsonParser<Adjustment::TraverseParam<Adjustment::RouteType::ClosedConnecting>>
{
    using Target = Adjustment::TraverseParam<Adjustment::RouteType::ClosedConnecting>;

    template <typename __ptree>
    static void Validate(const __ptree &json)
    {
    }

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        PTree::ValidateArray(json, "distances");
        PTree::ValidateArray(json, "angles");
        PTree::ValidateArray(json, "azi_beg");
        PTree::ValidateArray(json, "azi_end");
        PTree::ValidateValue<double>(json, "x_beg");
        PTree::ValidateValue<double>(json, "y_beg");
        PTree::ValidateValue<double>(json, "x_end");
        PTree::ValidateValue<double>(json, "y_end");

        return Target{
            .distances = PTree::ArrayTo<std::vector<double>>(json, "distances"),
            .angles = PTree::ArrayView(json, "angles") |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(sub)); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, "azi_beg")),
            .azi_end = Angle::FromSpan(PTree::ArrayTo<std::vector<double>>(json, "azi_end")),
            .x_beg = PTree::Value<double>(json, "x_beg"),
            .y_beg = PTree::Value<double>(json, "y_beg"),
            .x_end = PTree::Value<double>(json, "x_end"),
            .y_end = PTree::Value<double>(json, "y_end")};
    }

    static std::string Expect()
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
};
AGTB_IO_END

#endif