#include "AGTB/Adjustment/Base.hpp"
#include "AGTB/Adjustment/Elevation.hpp"
#include "AGTB/Linalg/NormalEquationMatrixInverse.hpp"
#include "AGTB/Linalg/CorrectionOlsSolve.hpp"
#include "AGTB/IO/Eigen.hpp"

#include <boost/graph/adjacency_list.hpp>

#include <print>
#include <map>

AGTB_ADJUSTMENT_BEGIN

namespace Elevation
{

    struct ElevationRouteSectionProperty
    {
        std::string name;
        double dif;
        double len;
    };

    struct ElevationStationProperty
    {
        std::string name;
        double elev;
        bool is_control;
    };

    /**
     * @brief dif -> M, len -> KM
     *
     */
    struct ElevationNetMixedPropertyInParam
    {
        std::string name, beg, end;
        double dif;
        double len;
        // private
        mutable bool beg_is_control = true, end_is_control = true;
    };

    using ElevationNet = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        ElevationStationProperty,
        ElevationRouteSectionProperty>;

    using ElevationNetTraits = boost::graph_traits<ElevationNet>;
    using ElevationNetVertex = typename ElevationNetTraits::vertex_descriptor;
    using ElevationNetEdge = typename ElevationNetTraits::edge_descriptor;

    /**
     * @brief control_points_elevation_map.value(double) -> m
     *
     * @tparam
     */
    template <>
    struct ElevationParam<RouteType::Net>
    {
        std::vector<ElevationNetMixedPropertyInParam> sections;
        std::map<std::string, double> control_points_elevation_map;
        double unit_p = 1.0;
    };

    struct ElevationNetParamParseResult
    {
        Matrix A;
        Matrix l;
        Matrix P;
        std::map<std::string, double> unknown_porperty_map;
        ElevationNet net;
    };

    ElevationNetParamParseResult ParseElevationNetParam(ElevationParam<RouteType::Net> &param)
    {
        auto &control_map = param.control_points_elevation_map;
        auto &sections = param.sections;

        int n = sections.size();
        int r = control_map.size();

        std::map<std::string, double> unknown_aprox_map;

        for (size_t r = 0; r != sections.size(); ++r)
        {
            auto &sec = sections.at(r);

            const auto
                &beg = sec.beg,
                &end = sec.end;
            sec.beg_is_control = control_map.contains(beg);
            sec.end_is_control = control_map.contains(end);

            if (sec.beg_is_control && !sec.end_is_control && !unknown_aprox_map.contains(end))
            {
                unknown_aprox_map.insert_or_assign(end, sec.dif + control_map.at(beg));
            }

            else if (!sec.beg_is_control && sec.end_is_control && !unknown_aprox_map.contains(beg))
            {
                unknown_aprox_map.insert_or_assign(beg, control_map.at(end) - sec.dif);
            }

            else
            {
                ;
            }
        }

        int t = unknown_aprox_map.size();

        if (n - t + r < 0)
        {
            AGTB_THROW(std::runtime_error,
                       std::format(
                           "While parsing elevation net parameters, it's not enough for approxmiately calculatin of unkonwn points( n = {}, t = {}, r = {})\n", n, t, r));
        }

        Matrix A{Matrix::Zero(n, t)};
        Matrix l{Matrix::Zero(n, 1)};
        Matrix P{Matrix::Identity(n, n)};

        for (size_t r = 0; r != sections.size(); ++r)
        {
            const auto &sec = sections.at(r);
            auto subA = A.row(r), subL = l.row(r);

            bool beg_is_control = sec.beg_is_control,
                 end_is_control = sec.end_is_control;

            subL(0) += sec.dif;

            if (!beg_is_control)
            {
                auto it = unknown_aprox_map.find(sec.beg);
                size_t c = std::distance(unknown_aprox_map.begin(), it);
                subA(c) = -1;
                subL(0) += it->second; // value(double)
            }
            else
            {
                subL(0) += control_map.at(sec.beg);
            }

            if (!end_is_control)
            {
                auto it = unknown_aprox_map.find(sec.end);
                size_t c = std::distance(unknown_aprox_map.begin(), it);
                subA(c) = 1;
                subL(0) -= it->second; // val(double)
            }
            else
            {
                subL(0) += -control_map.at(sec.end);
            }

            subL(0) *= 1000;

            P(r, r) = param.unit_p / sec.len;
        }

        return {
            A, l, P, unknown_aprox_map};
    }

    Matrix SolveParsedParam(ElevationNetParamParseResult &parsed)
    {
        const auto
            &A = parsed.A,
            &l = parsed.l,
            &P = parsed.P;
        Matrix inv{Linalg::NormalEquationMatrixInverse<LinalgOption::Cholesky>(A, P)};
        Matrix residual = A.transpose() * P * l;
        Matrix x = inv * residual / 1000;
        Matrix V = A * x - l / 1000;

        auto &map = parsed.unknown_porperty_map;
        size_t i = 0;
        for (auto &[key, value] : map)
        {
            value += x(i++, 0);
        }

        return V;
    }

    struct ElevationNetAdjustResult
    {
        std::map<std::string, double> section;
        std::map<std::string, double> station;
    };

    ElevationNetAdjustResult Adjust(ElevationParam<RouteType::Net> &param)
    {
        auto parsed = ParseElevationNetParam(param);
        Matrix V = SolveParsedParam(parsed);

        ElevationNetAdjustResult result{};
        result.station = std::move(parsed.unknown_porperty_map);

        auto &section = result.section;
        size_t i = 0;
        for (auto &sec : param.sections)
        {
            section.insert_or_assign(sec.name, sec.dif + V(i++, 0));
        }

        return result;
    }

    ElevationNetVertex AddVertexByNameFromElevAndNameMapOrOnlyReturnExistingVertexIndex(
        std::string name,
        std::map<std::string, ElevationNetVertex> &name_vertex_map,
        const std::map<std::string, double> &name_elev_map,
        const std::map<std::string, double> &name_approx_map,
        ElevationNet &net)
    {
        if (name_vertex_map.contains(name))
        {
            return name_vertex_map.at(name);
        }

        ElevationNetVertex idx{};
        double elev;
        bool is_control;
        if (name_elev_map.contains(name))
        {
            elev = name_elev_map.at(name);
            is_control = true;
        }
        else if (name_approx_map.contains(name))
        {
            elev = name_approx_map.at(name);
            is_control = false;
        }
        else
        {
            AGTB_THROW(std::invalid_argument, std::format("vertex `{}` is not a control or approx", name));
        }

        idx = boost::add_vertex(
            ElevationStationProperty{
                .name = name,
                .elev = elev,
                .is_control = is_control},
            net);

        name_vertex_map.insert_or_assign(name, idx);
        return idx;
    }

    struct BuildNetResult
    {
        ElevationNet net;
        std::map<std::string, ElevationNetVertex> name_vertex_map;
        std::map<std::string, ElevationNetEdge> name_edge_map;
        int n, t, r;
    };

    BuildNetResult BuildNet(const ElevationParam<RouteType::Net> &param)
    {
        std::map<std::string, ElevationNetVertex> name_vertex_map;
        std::map<std::string, ElevationNetEdge> name_edge_map;

        std::map<std::string, double> name_approx_map;

        auto &control_map = param.control_points_elevation_map;
        auto &sections = param.sections;

        int n = sections.size();
        int r = control_map.size();

        for (size_t r = 0; r != sections.size(); ++r)
        {
            auto &sec = sections.at(r);

            const auto
                &beg = sec.beg,
                &end = sec.end;
            auto beg_is_control = control_map.contains(beg);
            auto end_is_control = control_map.contains(end);

            if (beg_is_control && !end_is_control && !name_approx_map.contains(end))
            {
                name_approx_map.insert_or_assign(end, sec.dif + control_map.at(beg));
            }

            else if (!beg_is_control && end_is_control && !name_approx_map.contains(end))
            {
                name_approx_map.insert_or_assign(end, control_map.at(end) - sec.dif);
            }

            else
            {
            }
        }

        int t = name_approx_map.size();

        if (n - t + r < 0)
        {
            AGTB_THROW(std::runtime_error,
                       std::format(
                           "While parsing elevation net parameters, it's not enough for approxmiately calculatin of unkonwn points( n = {}, t = {}, r = {})\n", n, t, r));
        }

        ElevationNet net{};

        for (size_t r = 0; r != sections.size(); ++r)
        {
            auto sec = sections.at(r);
            auto beg = sec.beg;
            auto end = sec.end;

            auto beg_idx =
                AddVertexByNameFromElevAndNameMapOrOnlyReturnExistingVertexIndex(
                    beg,
                    name_vertex_map,
                    control_map,
                    name_approx_map,
                    net);
            auto end_idx =
                AddVertexByNameFromElevAndNameMapOrOnlyReturnExistingVertexIndex(
                    end,
                    name_vertex_map,
                    control_map,
                    name_approx_map,
                    net);
            auto add_edge_result = boost::add_edge(
                beg_idx,
                end_idx,
                ElevationRouteSectionProperty{
                    .name = sec.name,
                    .dif = sec.dif,
                    .len = sec.len},
                net);

            if (add_edge_result.second)
            {
                name_edge_map.insert_or_assign(sec.name, add_edge_result.first);
            }
        }

        return {
            net, name_vertex_map, name_edge_map, n, t, r};
    }

    struct ParseNetResult
    {
        Matrix A, l, P;
    };

    ParseNetResult ParseNet(const ElevationParam<RouteType::Net> &param, const BuildNetResult &result)
    {
        auto &net = result.net;
        auto &name_vertex_map = result.name_vertex_map;
        auto &name_edge_map = result.name_edge_map;
        auto &sections = param.sections;

        Matrix A{Matrix::Zero(result.n, result.t)};
        Matrix l{Matrix::Zero(result.n, 1)};
        Matrix P{Matrix::Identity(result.n, result.n)};

        for (size_t r = 0; r != sections.size(); ++r)
        {
            const auto &sec = sections.at(r);
            auto subA = A.row(r), subL = l.row(r);

            auto beg = sec.beg,
                 end = sec.end;
            auto beg_vert = net[name_vertex_map.at(beg)],
                 end_vert = net[name_vertex_map.at(end)];

            subL(0) += sec.dif;

            if (!beg_vert.is_control)
            {
                auto it = name_vertex_map.find(sec.beg);
                size_t c = std::distance(name_vertex_map.begin(), it);
                subA(c - 1) = -1;
                subL(0) += beg_vert.elev; // value(double)
            }
            else
            {
                subL(0) += beg_vert.elev;
            }

            if (!end_vert.is_control)
            {
                auto it = name_vertex_map.find(sec.end);
                size_t c = std::distance(name_vertex_map.begin(), it);
                subA(c - 1) = 1;
                subL(0) -= end_vert.elev; // val(double)
            }
            else
            {
                subL(0) -= end_vert.elev;
            }

            subL(0) *= 1000;

            P(r, r) = param.unit_p / sec.len;
        }

        return {A, l, P};
    }

    void AdjustNet(BuildNetResult &built, const ParseNetResult &parsed)
    {
        const auto
            &A = parsed.A,
            &l = parsed.l,
            &P = parsed.P;
        auto &net = built.net;
        const auto &name_vertex_map = built.name_vertex_map;
        const auto &name_edge_map = built.name_edge_map;

        Matrix inv{Linalg::NormalEquationMatrixInverse<LinalgOption::Cholesky>(A, P)};
        Matrix residual = A.transpose() * P * l;
        Matrix x = inv * residual / 1000;
        Matrix V = A * x - l / 1000;

        size_t i = 0;
        for (const auto &[k, v] : name_vertex_map)
        {
            auto vert = net[v];
            if (vert.is_control)
            {
                continue;
            }
            net[v].elev += x(i++, 0);
        }

        i = 0;
        for (const auto &[k, v] : name_edge_map)
        {
            auto edge = net[v];
            edge.dif += V(i++, 0);
        }
    }

    ElevationNetAdjustResult AdjustAndBuildNet(const ElevationParam<RouteType::Net> &param, ElevationNet &net_out)
    {
        auto build_net_result = BuildNet(param);
        auto parse_net_result = ParseNet(param, build_net_result);
        AdjustNet(build_net_result, parse_net_result);

        ElevationNetAdjustResult result{};

        const auto &net = build_net_result.net;
        for (auto [beg, end] = boost::vertices(net); beg != end; ++beg)
        {
        }

        net_out = std::move(build_net_result.net);
        return result;
    }
}

AGTB_ADJUSTMENT_END

using namespace AGTB::Adjustment;
using namespace AGTB::Adjustment::Elevation;
using AGTB::IO::PrintEigen;

int main()
{
    ElevationParam<RouteType::Net> param1{
        {{"h1", "A", "P1", 1.003, 1},
         {"h2", "P1", "P2", 0.501, 2},
         {"h3", "C", "P2", 0.503, 2},
         {"h4", "B", "P1", 0.505, 1}},
        {{"A", 11.000},
         {"B", 11.500},
         {"C", 12.008}}};

    ElevationParam<RouteType::Net> param2{
        {{"1", "A", "B", 5.835, 3.5},
         {"2", "B", "C", 3.782, 2.7},
         {"3", "A", "C", 9.640, 4.0},
         {"4", "D", "C", 7.384, 3.0},
         {"5", "A", "D", 2.270, 2.5}},
        {{"A", 237.483}},
        10.0};

    auto res = Adjust(param2);

    for (auto &[k, v] : res.section)
    {
        std::println("{}, {}", k, v);
    }

    for (auto &[k, v] : res.station)
    {
        std::println("{}, {}", k, v);
    }

    auto build_net_result = BuildNet(param2);
    auto net = build_net_result.net;

    for (auto [beg, end] = boost::vertices(net); beg != end; ++beg)
    {
        auto vertex = net[*beg];
        std::println("{}, {}, {}", vertex.name, vertex.elev, vertex.is_control ? "Is Control" : "Not");
    }

    for (auto [beg, end] = boost::edges(net); beg != end; ++beg)
    {
        auto edge = net[*beg];
        auto src = net[boost::source(*beg, net)];
        auto tar = net[boost::target(*beg, net)];
        std::println("{}, {}, {} <=> {} -> {}", edge.name, edge.dif, edge.len, src.name, tar.name);
    }

    auto parse_net_result = ParseNet(param2, build_net_result);
    PrintEigen(parse_net_result.A, "Net A");
    PrintEigen(parse_net_result.l, "Net l");
    PrintEigen(parse_net_result.P, "Net P");

    auto parse = ParseElevationNetParam(param2);
    PrintEigen(parse.A, "Net A");
    PrintEigen(parse.l, "Net l");
    PrintEigen(parse.P, "Net P");
}