#include <AGTB/Container/NamedGraph.hpp>
#include <vector>
#include <print>

using AGTB::NamedGraph;

using std::println;

struct vertex_property
{
    double vertex_val;
};

struct edge_property
{
    double edge_val;
};

int main()
{
    NamedGraph<
        vertex_property,
        edge_property>
        ng;

    ng.AddVertex()("A", {1.0})("B", {2.0})("C", {3.0});

    auto vertices = ng.Vertices();

    for (auto [it, end] = vertices.Iterators(); it != end; ++it)
    {

        println("{} {}", it->first, it->second);
    }

    for (const auto &n : vertices.Names())
    {
        println("{} -> {}", n, ng.Vertex(n).vertex_val);
    }

    ng.AddEdge("A", "B", "ab", {1145.0});
    for (const auto n : ng.Edges().Names())
    {
        println("{} : {}, {} => {}",
                n,
                ng.SourceName(n),
                ng.TargetName(n),
                ng.Edge(n).edge_val);
    }
}