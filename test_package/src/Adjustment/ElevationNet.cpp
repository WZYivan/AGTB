#include <AGTB/Adjustment/ElevationNet.hpp>

namespace aa = AGTB::Adjustment;

int main()
{
    aa::ElevationNet net{};

    net.AddVertex()(
        "A", {237.483, true});

    net.AddEdge(true)(
        "A", "B", "1", {5.835, 3.5})(
        "B", "C", "2", {3.782, 2.7})(
        "A", "C", "3", {9.640, 4.0})(
        "D", "C", "4", {7.384, 3.0})(
        "A", "D", "5", {2.270, 2.5});

    aa::Adjust(net, 10.0);
    aa::PrintElevationNet(net);
}