#include <AGTB/IO/JSON.hpp>
#include <AGTB/IO/Adjustment/Traverse.hpp>
#include <print>

namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    aio::Json json = aio::ReadJson("../dat/json/traverse_adjust.json");
    aa::TraverseParam<aa::RouteType::ClosedLoop> param;
    using target = decltype(param);
    param = aio::ParseJson<target>(json);
    std::println("{}", param.ToString());
}