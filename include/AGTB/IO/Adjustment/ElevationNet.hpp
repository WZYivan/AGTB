#ifndef __AGTB_IO_ADJUSTMENT_ELEVATION_NET_HPP__
#define __AGTB_IO_ADJUSTMENT_ELEVATION_NET_HPP__

#include "../../Adjustment/ElevationNet.hpp"
#include "../JSON.hpp"

AGTB_IO_BEGIN

template <>
struct JsonParser<Adjustment::ElevationNet>
{
    using Target = Adjustment::ElevationNet;

    static Target Parse(const Json &json)
    {
        {
            AGTB_THROW_IF_JSON_ARRAY_KEY_INVALID(json, "vertices");
            AGTB_THROW_IF_JSON_ARRAY_KEY_INVALID(json, "edges");
            const auto &vert = (*json.ArrayView("vertices").begin());
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(vert, "name", std::string);
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(vert, "elev", double);
            const auto &edge = (*json.ArrayView("edges").begin());
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(edge, "name", std::string);
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(edge, "from", std::string);
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(edge, "to", std::string);
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(edge, "dif", double);
            AGTB_THROW_IF_JSON_VALUE_KEY_INVALID(edge, "len", double);
        }

        Target target{};
        for (const auto &vert : json.ArrayView("vertices"))
        {
            auto name = vert.Value<std::string>("name");
            auto elev = vert.Value<double>("elev");
            target.AddVertex(name, {elev, true});
        }

        auto edge_adder = target.AddEdge(true);
        for (const auto &edge : json.ArrayView("edges"))
        {
            auto name = edge.Value<std::string>("name");
            auto from = edge.Value<std::string>("from");
            auto to = edge.Value<std::string>("to");
            auto dif = edge.Value<double>("dif");
            auto len = edge.Value<double>("len");
            edge_adder(from, to, name, {dif, len});
        }

        return target;
    }
};

AGTB_IO_END

#endif