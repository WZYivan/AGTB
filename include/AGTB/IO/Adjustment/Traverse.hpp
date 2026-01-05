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
        AGTB_THROW_IF_JSON_ARRAY_KEY_INVALID(json, "distances");
        AGTB_THROW_IF_JSON_ARRAY_KEY_INVALID(json, "angles");
        AGTB_THROW_IF_JSON_ARRAY_KEY_INVALID(json, "azi_beg");
        AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(json, "x_beg", double);
        AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(json, "y_beg", double);

        return {
            .distances = json.Container<std::vector<double>>("distances"),
            .angles = json.ArrayView("angles") |
                      std::views::transform([](const auto &sub) -> Angle
                                            { return Angle::FromSpan(sub.template ToContainer<std::vector<double>>()); }) |
                      std::ranges::to<std::vector<Angle>>(),
            .azi_beg = Angle::FromSpan(json.Container<std::vector<double>>("azi_beg")),
            .x_beg = json.Value<double>("x_beg"),
            .y_beg = json.Value<double>("y_beg")};
    }

    JsonParser() = default;
    ~JsonParser() = default;

    Target ParseConfig(const Json &json) const
    {
        return Parse(json);
    }
};

AGTB_IO_END

#endif