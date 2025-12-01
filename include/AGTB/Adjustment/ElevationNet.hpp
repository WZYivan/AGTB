#ifndef __AGTB_ADJUSTMENT_ELEVARION_NET_HPP__
#define __AGTB_ADJUSTMENT_ELEVARION_NET_HPP__

#include "Base.hpp"
#include "Elevation.hpp"
#include "../Linalg/NormalEquationMatrixInverse.hpp"
#include "../Linalg/CorrectionOlsSolve.hpp"
#include "../IO/Eigen.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <map>

AGTB_ADJUSTMENT_BEGIN

namespace Elevation
{
    namespace ElevationNetImpl
    {
        /**
         * @brief dif -> M, len -> KM
         *
         */
        struct RouteSection
        {
            std::string name, beg, end;
            double dif;
            double len;
            // private
            bool beg_is_control = true, end_is_control = true;
        };

        struct ElevationNetEdgeProperty
        {
            std::string name;
            double dif;
            double len;
        };

        struct ElevationNetVertexProperty
        {
            std::string name;
            double elev;
            bool is_control;
        };

        using ElevationNet = boost::adjacency_list<
            boost::vecS,
            boost::vecS,
            boost::directedS,
            ElevationNetVertexProperty,
            ElevationNetEdgeProperty>;

        using ElevationNetTraits = boost::graph_traits<ElevationNet>;
        using ElevationNetVertex = typename ElevationNetTraits::vertex_descriptor;
        using ElevationNetEdge = typename ElevationNetTraits::edge_descriptor;

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
                ElevationNetVertexProperty{
                    .name = name,
                    .elev = elev,
                    .is_control = is_control},
                net);

            name_vertex_map.insert_or_assign(name, idx);
            return idx;
        }
    }

    /**
     * @brief control_points_elevation_map.value(double) -> m
     *
     * @tparam
     */
    template <>
    struct ElevationParam<RouteType::Net>
    {
        std::vector<ElevationNetImpl::RouteSection> sections;
        std::map<std::string, double> control_points_elevation_map;
        double unit_p = 1.0;
    };

    struct ElevationNetParamParseResult
    {
        Matrix A;
        Matrix l;
        Matrix P;
        std::map<std::string, double> unknown_porperty_map;
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

    ElevationNetImpl::ElevationNet BuildNet(const ElevationNetAdjustResult &result, const ElevationParam<RouteType::Net> &param)
    {
        std::map<std::string, ElevationNetImpl::ElevationNetVertex> name_vertex_map;
        std::map<std::string, double> name_elev_map;

        const auto &sections = param.sections;
        for (const auto &sec : sections)
        {
        }

        // TODO : I have no patience, fuck stupid boost
    }
}

AGTB_ADJUSTMENT_END

#endif