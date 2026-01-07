#include <AGTB/IO.hpp>

namespace ac = AGTB::Container;
namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    ac::PropTree json{};
    aio::ReadJson("../dat/json/elevation_net.json", json);
    using target = aa::ElevationNet;
    target net = aio::ParseJson<target>(json);
    aa::PrintElevationNet(net);
}
