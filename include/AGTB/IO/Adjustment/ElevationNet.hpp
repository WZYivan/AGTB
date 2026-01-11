#ifndef __AGTB_IO_ADJUSTMENT_ELEVATION_NET_HPP__
#define __AGTB_IO_ADJUSTMENT_ELEVATION_NET_HPP__

#include "../../Adjustment/ElevationNet.hpp"
#include "../JSON.hpp"

AGTB_IO_BEGIN

template <>
struct JsonParser<Adjustment::ElevationNet> : public PTreeExt::EnableDefAliasBase
{
    using Target = Adjustment::ElevationNet;

    AGTB_JSON_PARSER_DEF_KEY(vertices);
    AGTB_JSON_PARSER_DEF_KEY(edges);
    AGTB_JSON_PARSER_DEF_KEY(elev);
    AGTB_JSON_PARSER_DEF_KEY(len);
    AGTB_JSON_PARSER_DEF_KEY(dif);
    AGTB_JSON_PARSER_DEF_KEY(name);
    AGTB_JSON_PARSER_DEF_KEY(from);
    AGTB_JSON_PARSER_DEF_KEY(to);

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

    template <typename __ptree>
    static Target Parse(const __ptree &json)
    {
        {
            PTree::ValidateArray(json, Key__vertices());
            PTree::ValidateArray(json, Key__edges());
            const auto &vert = (*PTree::ArrayView(json, Key__vertices()).begin());
            PTree::ValidateValue<std::string>(vert, Key__name());
            PTree::ValidateValue<double>(vert, Key__elev());
            const auto &edge = (*PTree::ArrayView(json, Key__edges()).begin());
            PTree::ValidateValue<std::string>(edge, Key__name());
            PTree::ValidateValue<std::string>(edge, Key__from());
            PTree::ValidateValue<std::string>(edge, Key__to());
            PTree::ValidateValue<double>(edge, Key__dif());
            PTree::ValidateValue<double>(edge, Key__len());
        }

        Target target{};
        for (const auto &vert : PTree::ArrayView(json, Key__vertices()))
        {
            auto name = PTree::Value<std::string>(vert, Key__name());
            auto elev = PTree::Value<double>(vert, Key__elev());
            target.AddVertex(name, {elev, true});
        }

        auto edge_adder = target.AddEdge(true);
        for (const auto &edge : PTree::ArrayView(json, Key__edges()))
        {
            auto name = PTree::Value<std::string>(edge, Key__name());
            auto from = PTree::Value<std::string>(edge, Key__from());
            auto to = PTree::Value<std::string>(edge, Key__to());
            auto dif = PTree::Value<double>(edge, Key__dif());
            auto len = PTree::Value<double>(edge, Key__len());
            edge_adder(from, to, name, {dif, len});
        }

        return target;
    }

    template <typename __ptree>
    Target ParseConfig(const __ptree &json) const
    {
        const auto &map = AliasMap();

        {
            PTree::ValidateArray(json, Key__vertices(), map);
            PTree::ValidateArray(json, Key__edges(), map);
            const auto &vert = (*PTree::ArrayView(json, Key__vertices(), map).begin());
            PTree::ValidateValue<std::string>(vert, Key__name(), map);
            PTree::ValidateValue<double>(vert, Key__elev(), map);
            const auto &edge = (*PTree::ArrayView(json, Key__edges(), map).begin());
            PTree::ValidateValue<std::string>(edge, Key__name(), map);
            PTree::ValidateValue<std::string>(edge, Key__from(), map);
            PTree::ValidateValue<std::string>(edge, Key__to(), map);
            PTree::ValidateValue<double>(edge, Key__dif(), map);
            PTree::ValidateValue<double>(edge, Key__len(), map);
        }

        Target target{};
        for (const auto &vert : PTree::ArrayView(json, Key__vertices(), map))
        {
            auto name = PTree::Value<std::string>(vert, Key__name(), map);
            auto elev = PTree::Value<double>(vert, Key__elev(), map);
            target.AddVertex(name, {elev, true});
        }

        auto edge_adder = target.AddEdge(true);
        for (const auto &edge : PTree::ArrayView(json, Key__edges(), map))
        {
            auto name = PTree::Value<std::string>(edge, Key__name(), map);
            auto from = PTree::Value<std::string>(edge, Key__from(), map);
            auto to = PTree::Value<std::string>(edge, Key__to(), map);
            auto dif = PTree::Value<double>(edge, Key__dif(), map);
            auto len = PTree::Value<double>(edge, Key__len(), map);
            edge_adder(from, to, name, {dif, len});
        }

        return target;
    }
};

AGTB_IO_END

#endif