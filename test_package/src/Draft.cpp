#include <AGTB/Container/NamedGraph.hpp>
#include <AGTB/Linalg/NormalEquationMatrixInverse.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <vector>
#include <print>

using AGTB::NamedGraph;
using AGTB::IO::PrintEigen;
using AGTB::Linalg::Matrix;

using std::println;

struct vertex_property
{
    double elev;
    bool is_control;
    bool with_init = is_control;
};

struct edge_property
{
    double dif;
    double len;
};

void print_net(auto &net)
{
    for (const auto n : net.Edges().Names())
    {
        println("{} : {}, {} => dif = {}, len = {}",
                n,
                net.SourceName(n),
                net.TargetName(n),
                net.Edge(n).dif,
                net.Edge(n).len);
    }

    for (const auto n : net.Vertices().Names())
    {
        println("{} : elev = {}, {} control, {} init",
                n,
                net.Vertex(n).elev,
                net.Vertex(n).is_control ? "is" : "not",
                net.Vertex(n).with_init ? "with" : "without");
    }
}

int main()
{
    NamedGraph<
        vertex_property,
        edge_property>
        net;

    net.AddVertex()(
        "A", {237.483, true})(
        "B", {.is_control = false})(
        "C", {.is_control = false})(
        "D", {.is_control = false});

    net.AddEdge()(
        "A", "B", "1", {5.835, 3.5})(
        "B", "C", "2", {3.782, 2.7})(
        "A", "C", "3", {9.640, 4.0})(
        "D", "C", "4", {7.384, 3.0})(
        "A", "D", "5", {2.270, 2.5});

    print_net(net);

    auto edges = net.Edges();
    auto verts = net.Vertices();
    std::vector<std::string> unknown{};

    for (const auto &v : verts.Names())
    {
        if (!net.Vertex(v).is_control)
        {
            unknown.push_back(v);
        }
    }

    for (const auto &e : edges.Names())
    {
        auto
            &beg = net.SourceVertex(e),
            &end = net.TargetVertex(e);
        auto
            beg_name = net.SourceName(e),
            end_name = net.TargetName(e);
        const auto &edge = net.Edge(e);

        if (beg.is_control && !end.with_init)
        {
            end.elev = edge.dif + beg.elev;
            end.with_init = true;
        }
        else if (!beg.with_init && end.is_control)
        {
            beg.elev = end.elev - edge.dif;
            beg.with_init = true;
        }
        else
        {
            ;
        }
    }

    print_net(net);

    int n = net.Edges().Size();
    int t = unknown.size();

    Matrix A{Matrix::Zero(n, t)};
    Matrix l{Matrix::Zero(n, 1)};
    Matrix P{Matrix::Identity(n, n)};

    double unit_p = 10.0;

    size_t r = 0;
    for (const auto &e : edges.Names())
    {
        auto
            subA = A.row(r),
            subl = l.row(r);

        auto &edge = net.Edge(e);
        auto
            &beg = net.SourceVertex(e),
            &end = net.TargetVertex(e);
        auto
            beg_name = net.SourceName(e),
            end_name = net.TargetName(e);

        subl(0) += edge.dif;

        if (!beg.is_control)
        {
            auto it = std::ranges::find(unknown, beg_name);
            size_t c = std::distance(unknown.begin(), it);
            subA(c) = -1;
            subl(0) += beg.elev; // value(double)
        }
        else
        {
            subl(0) += beg.elev;
        }

        if (!end.is_control)
        {
            auto it = std::ranges::find(unknown, end_name);
            size_t c = std::distance(unknown.begin(), it);
            subA(c) = 1;
            subl(0) -= end.elev; // val(double)
        }
        else
        {
            subl(0) -= end.elev;
        }

        subl(0) *= 1000;

        P(r, r) = unit_p / edge.len;

        ++r;
    }

    Matrix inv{AGTB::Linalg::NormalEquationMatrixInverse<AGTB::LinalgOption::Cholesky>(A, P)};
    Matrix residual = A.transpose() * P * l;
    Matrix x = inv * residual / 1000;
    Matrix V = A * x - l / 1000;

    PrintEigen(A, "A:");
    PrintEigen(l, "l:");
    PrintEigen(P, "P:");
    PrintEigen(x, "x:");
    PrintEigen(V, "V:");

    r = 0;
    for (const auto &vert : unknown)
    {
        net.Vertex(vert).elev += x(r, 0);
        ++r;
    }

    r = 0;
    for (const auto &e : edges.Names())
    {
        net.Edge(e).dif += V(r, 0);
        ++r;
    }

    print_net(net);
}