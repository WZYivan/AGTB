#ifndef __AGTB_ADJUSTMENT_TRAVERSE_NET_HPP__
#define __AGTB_ADJUSTMENT_TRAVERSE_NET_HPP__

#include "Base.hpp"
#include "Traverse.hpp"
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Utils/Math.hpp"
#include "../Container/NamedGraph.hpp"

#include <format>
#include <queue>

AGTB_ADJUSTMENT_BEGIN

namespace Traverse::Net
{
    struct StationProperty
    {
        Angle beta;
        PlaneCoordinate coord;
        bool is_control = false;
        bool with_init = is_control;
        bool is_virtual = false;
        bool in_queue = false;
    };

    struct VertexProperty
    {
        // Angle beta;
        // std::map<std::string, Angle> beta;
        // double x = 0.0, y = 0.0;
        std::map<std::string, StationProperty> property;
    };

    struct EdgeProperty
    {
        double len;
        Angle azimuth;
        bool is_control = false;
        bool with_init = is_control;
        bool is_virtual = false;
    };

    using TraverseNet = NamedGraph<VertexProperty, EdgeProperty>;

    struct CalculationProperty
    {
        double dx{}, dy{}, s0{}, ds{}, a{}, b{};
    };

    struct TraverseNetVariable
    {
        Matrix A, P, l, x, V;
        std::map<TraverseNet::name_type, CalculationProperty> edge_calculation_property;
        std::vector<TraverseNet::name_type> unknown;
    };

    struct BuildItem
    {
        TraverseNet::name_type cur_vert, pre_vert, nxt_vert, in, out, pre_in;

        static const TraverseNet::name_type Null;
    };

    const TraverseNet::name_type BuildItem::Null = "__NULL__";

    void PrintTraverseNet(const TraverseNet &net)
    {
        for (const auto &vert_name : net.Vertices().Names())
        {
            const auto &vert = net.Vertex(vert_name);

            std::println("{}", vert_name);
            for (const auto &kv : vert.property)
            {
                const auto &prop = kv.second;
                std::println("|- [{}] => bate = {}, x = {}, y = {}, {} control, {} init, {} virtual",
                             kv.first,
                             prop.beta.ToString(),
                             prop.coord.x,
                             prop.coord.y,
                             prop.is_control ? "is" : "not",
                             prop.with_init ? "with" : "without",
                             prop.is_virtual ? "is" : "not");
            }
        }

        for (const auto &edge_name : net.Edges().Names())
        {
            const auto &edge = net.Edge(edge_name);

            std::println("{}: {} -> {}; len = {}, azi = {}, {} control, {} init, {} virtual",
                         edge_name,
                         net.SourceName(edge_name),
                         net.TargetName(edge_name),
                         edge.len,
                         edge.azimuth.ToString(),
                         edge.is_control ? "is" : "not",
                         edge.with_init ? "with" : "without",
                         edge.is_virtual ? "is" : "not");
        }
    }

    std::string NameOfVirtualVertex(const std::string &vert_name)
    {
        return std::format("{}_prime", vert_name);
    }

    std::string NameOfVirtualEdge(const std::string &vert_name)
    {
        return std::format("S_prime_from_{}_to_{}", NameOfVirtualVertex(vert_name), vert_name);
    }

    void AddVirtualVertexOfKnownEdge(TraverseNet &net, std::string known_vert_name)
    {
        std::string virtual_name = NameOfVirtualVertex(known_vert_name);
        net.AddVertex(
            virtual_name, VertexProperty{{{NameOfVirtualEdge(known_vert_name), {.is_virtual = true}}}});
    }

    void AddVirtualKnownEdge(TraverseNet &net, std::string known_vert_name, Angle azi)
    {
        AddVirtualVertexOfKnownEdge(net, known_vert_name);
        std::string virtual_vert = NameOfVirtualVertex(known_vert_name);
        std::string virtual_edge = NameOfVirtualEdge(known_vert_name);
        net.AddEdge(
            virtual_vert,
            known_vert_name,
            virtual_edge,
            EdgeProperty{
                .azimuth = azi,
                .is_control = true,
                .is_virtual = true});
    }

    void InitTraverseNet(TraverseNet &net, TraverseNetVariable &var)
    {
        auto &unknown = var.unknown;
        auto edges_access = net.Edges();
        auto verts_access = net.Vertices();

        std::queue<TraverseNet::name_type> initializer_list{};

        for (const auto &vert_name : verts_access.Names())
        {
            const auto &vert = net.Vertex(vert_name);

            for (auto [it, end] = net.InEdges(vert_name); it != end; ++it)
            {
                const auto &ename = net.NameOf(*it);
                const auto &prop = vert.property.at(ename);

                if (!prop.is_control && !prop.is_virtual)
                {
                    if (std::find(unknown.begin(), unknown.end(), vert_name) != unknown.end())
                    {
                        continue;
                    }
                    unknown.push_back(vert_name);
                }
            }
        }

        std::vector<BuildItem> cal_list{};

        for (const auto &vert_name : verts_access.Names())
        {
            auto &vert = net.Vertex(vert_name);
            for (const auto &ename : net.InEdgesOf(vert_name))
            {
                auto &prop = vert.property.at(ename);
                if (!prop.is_control)
                {
                    continue;
                }

                prop.in_queue = true;
                for (const auto &out_name : net.OutEdgesOf(vert_name))
                {
                    BuildItem item{
                        .cur_vert = vert_name,
                        .pre_vert = net.SourceName(ename),
                        .nxt_vert = net.TargetName(out_name),
                        .in = ename,
                        .out = out_name,
                        .pre_in = ename};

                    cal_list.push_back(item);
                }
            }
        }
        // TODO: HERE
        for (const auto &item : cal_list)
        {
            const auto &cur_vert = item.cur_vert;

            std::println("cur: {}", cur_vert);

            auto prop = net.Vertex(cur_vert);

            for (const auto &[k, v] : prop.property)
            {
                std::println("|-{}, {}", k, v.beta.ToString());
            }

            for (const auto &ename : net.EdgesOf(cur_vert))
            {
                std::println("in: ", ename);
            }

            std::println("{}", "over loop");
        }

        for (const auto &edge_name : edges_access.Names())
        {
            if (net.Edge(edge_name).is_control)
            {
                initializer_list.push(edge_name);
            }
        }

        int t = unknown.size();
        int n = initializer_list.size() + edges_access.Size() * 2;

        while (!initializer_list.empty())
        {
            const auto &seed_edge_name = initializer_list.front();
            const auto &known_edge = net.Edge(seed_edge_name);
            const auto &known_vert = net.TargetVertex(seed_edge_name);
            const auto &known_vert_name = net.TargetName(seed_edge_name);

            // std::println("known edge: {}, known vert: {}", seed_edge_name, known_vert_name);

            for (auto [it, end] = net.OutEdges(known_vert_name); it != end; ++it)
            {
                auto eidx = *it;
                const auto &ename = net.NameOf(eidx);
                auto &unknown_edge = net.Edge(eidx);
                auto &unknown_vert = net.TargetVertex(ename);
                auto &unknown_prop = unknown_vert.property.at(ename);
                auto &seed_prop = known_vert.property.at(seed_edge_name);

                // std::println("unknown edge: {}, unknwon vert: {}", ename, net.TargetName(ename));

                unknown_edge.azimuth = (known_edge.azimuth + seed_prop.beta - Utils::Angles::ang_180d).NormStd();
                unknown_edge.with_init = true;
                initializer_list.push(ename);

                if (unknown_prop.with_init)
                {
                    continue;
                }

                double
                    s = unknown_edge.len,
                    cos = unknown_edge.azimuth.Cos(),
                    sin = unknown_edge.azimuth.Sin(),
                    dx = s * cos,
                    dy = s * sin,
                    s0 = gcem::sqrt(dx * dx + dy * dy),
                    ds = (s - s0) * 1000,
                    a = rho2 * sin / (s0 * 1000),
                    b = -rho2 * cos / (s0 * 1000);

                unknown_prop.coord.x = seed_prop.coord.x + dx;
                unknown_prop.coord.y = seed_prop.coord.y + dy;
                unknown_prop.with_init = true;

                CalculationProperty cal_prop{.dx = dx, .dy = dy, .s0 = s0, .ds = ds, .a = a, .b = b};
                var.edge_calculation_property.insert_or_assign(ename, cal_prop);
            }

            initializer_list.pop();
        }
    }

    auto PropOfItem(TraverseNet &net, TraverseNetVariable &var, const BuildItem &item)
    {
        const auto
            &in = item.in,
            &out = item.out;
        CalculationProperty in_prop{}, out_prop{};

        if (var.edge_calculation_property.contains(in))
        {
            in_prop = var.edge_calculation_property.at(in);
        }

        if (var.edge_calculation_property.contains(out))
        {
            out_prop = var.edge_calculation_property.at(out);
        }

        return std::make_tuple(in_prop, out_prop);
    }

    auto AngleCoeff(const CalculationProperty &in, const CalculationProperty &out)
    {
        double
            ai = in.a,
            bi = in.b,
            ao = out.a,
            bo = out.b,
            x1 = ao - ai,
            y1 = bo - bi,
            x2 = -ao,
            y2 = -bo,
            x3 = ai,
            y3 = bi;
        return std::make_tuple(x1, y1, x2, y2, x3, y3);
    }

    auto DistanceCoeff(const CalculationProperty &in)
    {
        double
            dx = in.dx,
            dy = in.dy,
            s0 = in.s0,
            c1 = dx / s0,
            c2 = dy / s0;
        return std::make_tuple(-c1, -c2, c1, c2);
    }

    bool __IsUnknown(const StationProperty &prop)
    {
        return ((!prop.is_control) && (!prop.is_virtual));
    }

    auto __AIdxOf(size_t idx)
    {
        return std::make_tuple(idx * 2, idx * 2 + 1);
    }

    void HandleOneVertex(TraverseNetVariable &var, const StationProperty &prop, size_t r, size_t idx, double x, double y)
    {
        if (__IsUnknown(prop))
        {
            auto [xi, yi] = __AIdxOf(idx);
            var.A(r, xi) = x;
            var.A(r, yi) = y;
        }
    }

    void HandleItem(TraverseNet &net, TraverseNetVariable &var, size_t &arow, size_t &srow, const BuildItem &item)
    {
        const auto &cur = net.Vertex(item.cur_vert);
        const auto &cur_prop = cur.property.at(item.in);

        if (cur_prop.is_virtual)
        {
            return;
        }

        const auto &pre_prop = net.Vertex(item.pre_vert).property.at(item.pre_in);
        const auto &nxt_prop = net.Vertex(item.nxt_vert).property.at(item.out);

        const auto &unknown = var.unknown;
        auto [in_prop, out_prop] = PropOfItem(net, var, item);

        size_t cur_idx = std::distance(unknown.begin(), std::find(unknown.begin(), unknown.end(), item.cur_vert));
        size_t pre_idx = std::distance(unknown.begin(), std::find(unknown.begin(), unknown.end(), item.pre_vert));
        size_t nxt_idx = std::distance(unknown.begin(), std::find(unknown.begin(), unknown.end(), item.nxt_vert));

        auto [ax1, ay1, ax2, ay2, ax3, ay3] = AngleCoeff(in_prop, out_prop);
        auto [dx1, dy1, dx2, dy2] = DistanceCoeff(in_prop);

        HandleOneVertex(var, cur_prop, arow, cur_idx, ax1, ay1);
        HandleOneVertex(var, nxt_prop, arow, nxt_idx, ax2, ay2);
        HandleOneVertex(var, pre_prop, arow, pre_idx, ax3, ay3);

        ++arow;

        if (cur_prop.is_control)
        {
            return;
        }

        HandleOneVertex(var, cur_prop, srow, cur_idx, dx1, dy1);
        HandleOneVertex(var, nxt_prop, srow, nxt_idx, dx2, dy2);

        ++srow;
    }

    void BuildMatrix(TraverseNet &net, double unit_p, TraverseNetVariable &var)
    {
        int n, r, t, na, ns{0};

        t = var.unknown.size();
        na = net.Edges().Size();
        for (const auto &v : net.Vertices().Names())
        {
            ns += net.Vertex(v).property.size();
        }
        n = na + ns;

        var.A = Matrix(n, 2 * t);
        var.l = Matrix(n, 1);
        var.P = Matrix::Identity(n, n);

        auto edges_access = net.Edges();
        auto verts_access = net.Vertices();

        std::queue<BuildItem> cal_list{};

        for (const auto &vert_name : verts_access.Names())
        {
            const auto &vert = net.Vertex(vert_name);
            for (const auto &ename : net.InEdgesOf(vert_name))
            {
                const auto &prop = vert.property.at(ename);
                if (!prop.is_control)
                {
                    continue;
                }
                for (const auto &out_name : net.OutEdgesOf(vert_name))
                {
                    BuildItem item{
                        .cur_vert = vert_name,
                        .pre_vert = net.SourceName(ename),
                        .nxt_vert = net.TargetName(out_name),
                        .in = ename,
                        .out = out_name,
                        .pre_in = ename};

                    cal_list.push(item);
                }
            }
        }

        size_t arow{0}, srow{0};

        while (!cal_list.empty())
        {
            const auto &item = cal_list.front();

            HandleItem(net, var, arow, srow, item);

            // std::println("item: {} -> {} -> {} | {} => {} => [{}] => {}",
            //              item.pre_vert,
            //              item.cur_vert,
            //              item.nxt_vert,
            //              item.pre_in,
            //              item.in,
            //              item.cur_vert,
            //              item.out);

            cal_list.pop();
        }
    }
}

namespace Traverse
{
    using Net::AddVirtualKnownEdge;
    using Net::PrintTraverseNet;
    using Net::TraverseNet;
    using Net::TraverseNetVariable;

    /**
     * @brief Assume only no more than one out_edge
     *
     * @param net
     * @param unit_p
     * @return TraverseNetVariable
     */
    TraverseNetVariable Adjust(TraverseNet &net, double unit_p = 1.0)
    {
        TraverseNetVariable var{};

        Net::InitTraverseNet(net, var);
        Net::BuildMatrix(net, unit_p, var);

        return var;
    }

    TraverseNet::name_type NameOfControlEdge(const TraverseNet::name_type &control)
    {
        return Net::NameOfVirtualEdge(control);
    }
}

using Traverse::AddVirtualKnownEdge;
using Traverse::Adjust;
using Traverse::NameOfControlEdge;
using Traverse::PrintTraverseNet;
using Traverse::TraverseNet;

AGTB_ADJUSTMENT_END

#endif