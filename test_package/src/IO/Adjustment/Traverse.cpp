#include <AGTB/IO/JSON.hpp>
#include <AGTB/IO/Adjustment/Traverse.hpp>
#include <print>
#include <map>

namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    aio::Json json = aio::ReadJson("../dat/json/traverse_closed_loop_alias.json");
    using target = aa::TraverseParam<aa::RouteType::ClosedLoop>;
    std::map<std::string, target> params;
    aio::JsonParser<target> parser("d", "a", "ab", "x", "y");

    for (const auto &[key, sub_json] : json.ToMapView())
    {
        params.insert_or_assign(key, aio::ParseJson(sub_json, parser));
    }

    for (const auto &[k, p] : params)
    {
        std::println("[{}]\n{}\n", k, p.ToString());
    }
}