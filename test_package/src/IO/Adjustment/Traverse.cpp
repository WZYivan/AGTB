#include <AGTB/IO.hpp>
#include <AGTB/Adjustment.hpp>
#include <print>
#include <map>

namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;
namespace ac = AGTB::Container;

int main()
{
    AGTB::PropTree json{};
    aio::ReadJson("../dat/json/traverse_closed_loop_alias.json", json);
    using target = aa::TraverseParam<aa::RouteType::ClosedLoop>;
    std::map<std::string, target> params;
    aio::JsonParser<target> parser{};
    parser.DefAlias(parser.Key__distances())("d");
    parser.DefAlias(parser.Key__angles())("a");
    parser.DefAlias(parser.Key__azi_beg())("ab");
    parser.DefAlias(parser.Key__x_beg())("x");
    parser.DefAlias(parser.Key__y_beg())("y");

    for (const auto &[key, sub_json] : AGTB::PTree::MapView(json))
    {
        params.insert_or_assign(key, aio::ParseJson(sub_json, parser));
    }

    std::ofstream ofs{"RESULT.md"};

    for (const auto &[k, p] : params)
    {
        std::println(ofs, "# {}\n{}\n", k, p.ToString());
    }
}