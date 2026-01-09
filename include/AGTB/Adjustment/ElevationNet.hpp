#ifndef __AGTB_ADJUSTMENT_ELEVARION_NET_HPP__
#define __AGTB_ADJUSTMENT_ELEVARION_NET_HPP__

#include "Base.hpp"
#include "Elevation.hpp"
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Linalg/CorrectionOlsSolve.hpp"
#include "../IO/Eigen.hpp"
#include "../Container/NamedGraph.hpp"

#include <queue>
#include <vector>

AGTB_ADJUSTMENT_BEGIN

namespace Elevation::Net
{
    struct VertexProperty
    {
        double elev = 0.0;
        bool is_control = false;
        bool with_init = is_control;
    };

    struct EdgeProperty
    {
        double dif = 0.0;
        double len = 0.0;
    };

    using ElevationNet = NamedGraph<VertexProperty, EdgeProperty>;

    void InitializeElevations(ElevationNet &net, std::vector<ElevationNet::name_type> &unknown)
    {
        auto edges_access = net.Edges();
        auto verts_access = net.Vertices();

        std::queue<ElevationNet::name_type> initializer_list{};

        for (const auto &v : verts_access.Names())
        {
            if (!net.Vertex(v).is_control)
            {
                unknown.emplace_back(v);
            }
            else
            {
                initializer_list.emplace(v);
            }
        }

        int r = initializer_list.size();
        int n = edges_access.Size();
        int t = unknown.size();

        if (n - t + r < 0)
        {
            AGTB_THROW(std::invalid_argument, std::format("Input data don't support adjustments: n = {}, t = {}, r = {}", n, t, r));
        }

        while (!initializer_list.empty())
        {
            const auto &seed_name = initializer_list.front();

            for (auto [it, end] = net.OutEdges(seed_name); it != end; ++it)
            {
                auto edge_idx = *it;
                const auto &edge_name = net.NameOf(edge_idx);
                auto &vert_prop = net.TargetVertex(edge_name);
                const auto &edge_prop = net.Edge(edge_name);

                if (!vert_prop.with_init)
                {
                    vert_prop.elev = net.Vertex(seed_name).elev + edge_prop.dif;
                    vert_prop.with_init = true;
                    initializer_list.emplace(net.TargetName(edge_name));
                }
            }

            for (auto [it, end] = net.InEdges(seed_name); it != end; ++it)
            {
                auto edge_idx = *it;
                const auto &edge_name = net.NameOf(edge_idx);
                auto &vert_prop = net.SourceVertex(edge_name);
                const auto &edge_prop = net.Edge(edge_name);

                if (!vert_prop.with_init)
                {
                    vert_prop.elev = edge_prop.dif - net.Vertex(seed_name).elev;
                    vert_prop.with_init = true;
                    initializer_list.emplace(net.TargetName(edge_name));
                }
            }

            initializer_list.pop();
        }
    }

    void BuildMatrix(ElevationNet &net, Matrix &A, Matrix &l, Matrix &P, std::vector<ElevationNet::name_type> &unknown, double unit_p)
    {
        auto edges_access = net.Edges();
        auto verts_access = net.Vertices();

        int n = edges_access.Size();
        int t = unknown.size();

        A = Matrix::Zero(n, t);
        l = Matrix::Zero(n, 1);
        P = Matrix::Zero(n, n);

        size_t r = 0;
        for (const auto &e : edges_access.Names())
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
    }

    void ComputeCorrections(Matrix &A, Matrix &l, Matrix &P, Matrix &x, Matrix &V)
    {
        Matrix inv{AGTB::Linalg::NormalEquationMatrixInverse<AGTB::LinalgOption::Cholesky>(A, P)};
        Matrix residual = A.transpose() * P * l;
        x = inv * residual / 1000;
        V = A * x - l / 1000;
    }

    void ApplyCorrections(ElevationNet &net, std::vector<ElevationNet::name_type> &unknown, Matrix &x, Matrix &V)
    {
        size_t r = 0;
        for (const auto &vert : unknown)
        {
            net.Vertex(vert).elev += x(r, 0);
            ++r;
        }

        r = 0;
        for (const auto &e : net.Edges().Names())
        {
            net.Edge(e).dif += V(r, 0);
            ++r;
        }
    }

    struct ElevationNetVariable
    {
        Matrix A, l, P, x, V;
        std::vector<ElevationNet::name_type> unknown;
    };
}

namespace Elevation
{
    using Net::ElevationNet;
    using Net::ElevationNetVariable;

    ElevationNetVariable Adjust(ElevationNet &net, double unit_p = 1.0)
    {
        ElevationNetVariable var{};
        Net::InitializeElevations(net, var.unknown);
        Net::BuildMatrix(net, var.A, var.l, var.P, var.unknown, unit_p);
        Net::ComputeCorrections(var.A, var.l, var.P, var.x, var.V);
        Net::ApplyCorrections(net, var.unknown, var.x, var.V);

        return var;
    }

    void PrintElevationNet(const ElevationNet &net, std::ostream &os = std::cout)
    {
        for (const auto n : net.Edges().Names())
        {
            std::println(os, "{} : {}, {} => dif = {}, len = {}",
                         n,
                         net.SourceName(n),
                         net.TargetName(n),
                         net.Edge(n).dif,
                         net.Edge(n).len);
        }

        for (const auto n : net.Vertices().Names())
        {
            std::println(os, "{} : elev = {}, {} control, {} init",
                         n,
                         net.Vertex(n).elev,
                         net.Vertex(n).is_control ? "is" : "not",
                         net.Vertex(n).with_init ? "with" : "without");
        }
    }
}

using Elevation::Adjust;
using Elevation::ElevationNet;
using Elevation::ElevationNetVariable;
using Elevation::PrintElevationNet;

AGTB_ADJUSTMENT_END

#endif