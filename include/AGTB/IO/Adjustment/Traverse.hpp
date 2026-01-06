#ifndef __AGTB_IO_ADJUSTMENT_TRAVERSE_HPP__
#define __AGTB_IO_ADJUSTMENT_TRAVERSE_HPP__

#include "../../Adjustment/Traverse.hpp"
#include "../JSON.hpp"

AGTB_IO_BEGIN

template <>
struct JsonParser<Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>>
{
    using Target = Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop>;

    static Target Parse(const Json &json)
    {
        return DoParse(json, "distances", "angles", "azi_beg", "x_beg", "y_beg");
    }

    JsonParser(std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
        : dis(distances), ang(angles), ab(azi_beg), x(x_beg), y(y_beg)
    {
    }
    ~JsonParser() = default;

    Target ParseConfig(const Json &json) const
    {
        return DoParse(json, dis, ang, ab, x, y);
    }

private:
    std::string dis, ang, ab, x, y;

    static Target DoParse(const Json &json, std::string distances, std::string angles, std::string azi_beg, std::string x_beg, std::string y_beg)
    {
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, distances);
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, angles);
        AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, azi_beg);
        AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(json, x_beg, double);
        AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(json, y_beg, double);

        return {
            .distances = json.Container<std::vector<double>>(distances),
            .angles = json.ArrayView(angles) |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(sub.template ToContainer<std::vector<double>>()); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(json.Container<std::vector<double>>(azi_beg)),
            .x_beg = json.Value<double>(x_beg),
            .y_beg = json.Value<double>(y_beg)};
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

AGTB_IO_END

#endif