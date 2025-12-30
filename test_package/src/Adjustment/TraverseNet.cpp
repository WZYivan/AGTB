#include <print>

#define PRINT_VAR(var) std::println("{} = {}", #var, var)

#include <AGTB/IO/Eigen.hpp>
#include <AGTB/Adjustment/TraverseNet.hpp>

namespace aa = AGTB::Adjustment;

using AGTB::IO::PrintEigen;

int main()
{
    aa::TraverseNet net{};

    net.AddVertex()(
        "A", {{{aa::NameOfControlEdge("A"), {{86, 43, 16}, {11'768.714, 8419.242}, true}}}})(
        "B", {{{aa::NameOfControlEdge("B"), {{176, 33, 43}, {10'878.302, 8'415.114}, true}}}})(
        "C", {{{aa::NameOfControlEdge("C"), {{165, 40, 29}, {11'101.949, 8'017.572}, true}}}})(
        "D", {{{"1", {{182, 22, 43}}}}})(
        "E", {{{"2", {{188, 59, 57}}}}})(
        "G", {{{"3", {{115, 23, 37}}}, {"5", {{131, 27, 46}}}, {"7", {{113, 8, 37}}}}})(
        "F", {{{"4", {{123, 9, 5}}}}})(
        "H", {{{"6", {{165, 59, 58}}}}});

    aa::AddVirtualKnownEdge(net, "A", {274, 23, 34});
    aa::AddVirtualKnownEdge(net, "B", {8, 10, 27});
    aa::AddVirtualKnownEdge(net, "C", {107, 41, 27});

    net.AddEdge(true)(
        "A", "D", "1", {221.650})(
        "D", "E", "2", {195.843})(
        "E", "G", "3", {229.356})(
        "B", "F", "4", {189.781})(
        "F", "G", "5", {98.163})(
        "C", "H", "6", {151.480})(
        "H", "G", "7", {187.751});

    std::println("{:=^50}", "Input");
    aa::PrintTraverseNet(net);

    auto var = aa::Adjust(net, 1.0);

    PrintEigen(var.A, "A:");

    std::println("{:=^50}", "Output");
    aa::PrintTraverseNet(net);
}