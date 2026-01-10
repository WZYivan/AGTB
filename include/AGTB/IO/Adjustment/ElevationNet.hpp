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
            PTree::ValidateArray(json, "vertices");
            PTree::ValidateArray(json, "edges");
            const auto &vert = (*PTree::ArrayView(json, "vertices").begin());
            PTree::ValidateValue<std::string>(vert, "name");
            PTree::ValidateValue<double>(vert, "elev");
            const auto &edge = (*PTree::ArrayView(json, "edges").begin());
            PTree::ValidateValue<std::string>(edge, "name");
            PTree::ValidateValue<std::string>(edge, "from");
            PTree::ValidateValue<std::string>(edge, "to");
            PTree::ValidateValue<double>(edge, "dif");
            PTree::ValidateValue<double>(edge, "len");
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

    JsonParser() = default;
    ~JsonParser() = default;

    auto DefAlias(const std::string &key)
    {
        return PTree::DefAlias(map, key);
    }

private:
    PropPathAliasMap map;

public:
    template <typename __ptree>
    Target ParseConfig(const __ptree &json) const
    {
#if (AGTB_DEBUG)
        for (const auto &[k, v] : map)
        {
            std::println("key = {}", k);
            for (const auto &alias : v)
            {
                std::println("\talias = {}", alias);
            }
        }
#endif
        {
            PTree::ValidateArray(json, "vertices", map);
            PTree::ValidateArray(json, "edges", map);
            const auto &vert = (*PTree::ArrayView(json, "vertices", map).begin());
            PTree::ValidateValue<std::string>(vert, "name", map);
            PTree::ValidateValue<double>(vert, "elev", map);
            const auto &edge = (*PTree::ArrayView(json, "edges", map).begin());
            PTree::ValidateValue<std::string>(edge, "name", map);
            PTree::ValidateValue<std::string>(edge, "from", map);
            PTree::ValidateValue<std::string>(edge, "to", map);
            PTree::ValidateValue<double>(edge, "dif", map);
            PTree::ValidateValue<double>(edge, "len", map);
        }

        Target target{};
        for (const auto &vert : PTree::ArrayView(json, "vertices", map))
        {
            auto name = PTree::Value<std::string>(vert, "name", map);
            auto elev = PTree::Value<double>(vert, "elev", map);
            target.AddVertex(name, {elev, true});
        }

        auto edge_adder = target.AddEdge(true);
        for (const auto &edge : PTree::ArrayView(json, "edges", map))
        {
            auto name = PTree::Value<std::string>(edge, "name", map);
            auto from = PTree::Value<std::string>(edge, "from", map);
            auto to = PTree::Value<std::string>(edge, "to", map);
            auto dif = PTree::Value<double>(edge, "dif", map);
            auto len = PTree::Value<double>(edge, "len", map);
            edge_adder(from, to, name, {dif, len});
        }

        return target;
    }
};

AGTB_IO_END

#endif