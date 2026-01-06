#include <AGTB/IO.hpp>

namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    aio::Json json = aio::ReadJson("../dat/json/elevation_net.json");
    using target = aa::ElevationNet;
    target net = aio::ParseJson<target>(json);
    aa::PrintElevationNet(net);
}
