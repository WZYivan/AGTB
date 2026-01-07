#define AGTB_ENABLE_DEBUG

#include <AGTB/IO.hpp>
#include <print>
namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    AGTB::PropTree json{};
    aio::ReadJson("../dat/json/traverse_adjust.json", json);
    using target = aa::ElevationNet;
    target net = aio::ParseJson<target>(json);
    aa::PrintElevationNet(net);
}