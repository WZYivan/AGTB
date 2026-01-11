#define AGTB_ENABLE_DEBUG
#include <AGTB/IO.hpp>

namespace ac = AGTB::Container;
namespace aio = AGTB::IO;
namespace aa = AGTB::Adjustment;

int main()
{
    ac::PropTree json{};
    aio::ReadJson("../dat/json/elevation_net.json", json);
    using Target = aa::ElevationNet;

    aio::JsonParser<Target> parser;
    parser.DefAlias(parser.Key__vertices())("v")("verts");
    parser.DefAlias(parser.Key__edges())("es");
    parser.DefAlias(parser.Key__name())("n");
    parser.DefAlias(parser.Key__from())("beg");
    parser.DefAlias(parser.Key__to())("end");

    Target net = aio::ParseJson<Target>(json, parser);
    aa::PrintElevationNet(net);
}
