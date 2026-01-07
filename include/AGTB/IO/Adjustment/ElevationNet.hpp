#ifndef __AGTB_IO_ADJUSTMENT_ELEVATION_NET_HPP__
#define __AGTB_IO_ADJUSTMENT_ELEVATION_NET_HPP__

#include "../../Adjustment/ElevationNet.hpp"
#include "../JSON.hpp"

AGTB_IO_BEGIN

template <>
struct JsonParser<Adjustment::ElevationNet>
{
    using Target = Adjustment::ElevationNet;

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        {
            AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, "vertices");
            AGTB_JSON_PARSER_VALIDATE_ARRAY_KEY(json, "edges");
            const auto &vert = (*PTree::ArrayView(json, "vertices").begin());
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(vert, "name", std::string);
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(vert, "elev", double);
            const auto &edge = (*PTree::ArrayView(json, "edges").begin());
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(edge, "name", std::string);
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(edge, "from", std::string);
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(edge, "to", std::string);
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(edge, "dif", double);
            AGTB_JSON_PARSER_VALIDATE_VALUE_KEY(edge, "len", double);
        }

        Target target{};
        for (const auto &vert : PTree::ArrayView(json, "vertices"))
        {
            auto name = PTree::Value<std::string>(vert, "name");
            auto elev = PTree::Value<double>(vert, "elev");
            target.AddVertex(name, {elev, true});
        }

        auto edge_adder = target.AddEdge(true);
        for (const auto &edge : PTree::ArrayView(json, "edges"))
        {
            auto name = PTree::Value<std::string>(edge, "name");
            auto from = PTree::Value<std::string>(edge, "from");
            auto to = PTree::Value<std::string>(edge, "to");
            auto dif = PTree::Value<double>(edge, "dif");
            auto len = PTree::Value<double>(edge, "len");
            edge_adder(from, to, name, {dif, len});
        }

        return target;
    }

    static std::string Expect()
    {
        return R"(
{
    "vertices": [
        {
            "name": "A",
            "elev": 237.483
        }
    ],
    "edges": [
        {
            "name": "1",
            "from": "A",
            "to": "B",
            "dif": 5.835,
            "len": 3.5
        },
        {
            "name": "2",
            "from": "B",
            "to": "C",
            "dif": 3.782,
            "len": 2.7
        },
        {
            "name": "3",
            "from": "A",
            "to": "C",
            "dif": 9.64,
            "len": 4
        },
        {
            "name": "4",
            "from": "D",
            "to": "C",
            "dif": 7.384,
            "len": 3
        },
        {
            "name": "5",
            "from": "A",
            "to": "D",
            "dif": 2.27,
            "len": 2.5
        }
    ]
}
        )";
    }
};

AGTB_IO_END

#endif