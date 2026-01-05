#include <AGTB/IO/JSON.hpp>
#include <AGTB/IO/Adjustment/Traverse.hpp>
#include <print>

namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    aio::Json json = aio::ReadJson("../dat/json/traverse_closed_loop_alias.json");
    aa::TraverseParam<aa::RouteType::ClosedLoop> param;
    using target = decltype(param);
    aio::JsonParser<target> parser("d", "a", "ab", "x", "y");
    param = aio::ParseJson<target>(json, parser);
    std::println("{}", param.ToString());
}