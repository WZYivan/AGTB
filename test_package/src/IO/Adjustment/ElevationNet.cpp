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
    parser.DefAlias("vertices")("v")("verts");
    parser.DefAlias("edges")("es");
    parser.DefAlias("name")("n");
    parser.DefAlias("from")("beg");
    parser.DefAlias("to")("end");
    Target net = aio::ParseJson<Target>(json, parser);
    aa::PrintElevationNet(net);
}
