#ifndef __AGTB_ADJUSTMENT_ELEVATION_HPP__
#define __AGTB_ADJUSTMENT_ELEVATION_HPP__

#include "Base.hpp"
#include "RefineCorrection.hpp"

#include <vector>
#include <format>

AGTB_ADJUSTMENT_BEGIN

namespace Elevation
{
    template <RouteType __rt>
    struct ElevationParam
    {
        constexpr static RouteType route_type = __rt;
        std::vector<double> distances /*km*/, h;
        double H_beg, H_end;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);

            std::format_to(sbb, "{:=^100}\n", " ElevationParam ");
            std::format_to(sbb, "{:-^50}\n", " distances (km) ");
            for (auto &e : distances)
            {
                std::format_to(sbb, "{} ", e);
            }
            std::format_to(sbb, "\n{:-^50}\n", " Delta Height (m) ");
            for (auto &e : h)
            {
                std::format_to(sbb, "{} ", e);
            }
            std::format_to(sbb, "\n H_beg = {}", H_beg);
            if constexpr (__rt == RouteType::Connecting)
            {
                std::format_to(sbb, "\n H_end = {}", H_end);
            }
            return sb;
        }
    };

    struct ElevationVariable
    {
        std::vector<double> dh, h_c, H;
        double dis_sum, h_sum, dh_sum, h_c_sum;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);

            std::format_to(sbb, "{:=^100}\n", " ElevationVariable ");
            std::format_to(sbb, "{:-^50}\n", " Delta Height Correction (m) ");
            for (auto &e : dh)
            {
                std::format_to(sbb, "{} ", e);
            }
            std::format_to(sbb, "\n{:-^50}\n", " Corrected Delta Height (m) ");
            for (auto &e : h_c)
            {
                std::format_to(sbb, "{} ", e);
            }
            std::format_to(sbb, "\n{:-^50}\n", " Height (m) ");
            for (auto &e : H)
            {
                std::format_to(sbb, "{} ", e);
            }
            std::format_to(sbb, "\n{:-^50}\n", " Sum of ");
            std::format_to(sbb, " distances (km) = {}\n Delta Height (m) = {}\n Corrections (mm) = {}\n Corrected Delta Height (m) = {}",
                           dis_sum, h_sum, dh_sum, h_c_sum);

            return sb;
        }
    };

    struct ElevationInfo
    {
        double f_h, f_h_tolerance, v_km;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);

            std::format_to(sbb, "{:=^100}\n", " ElevationInfo ");
            std::format_to(sbb, " f_h (m) = {}\n f_h_tolerance (mm) = {}\n v_km (mm) = {}\n",
                           f_h, f_h_tolerance, v_km);
            return sb;
        }
    };

    struct ElevationAdjustResult
    {
        ElevationVariable variable;
        ElevationInfo info;

        int N() const
        {
            return variable.dh.size();
        }
    };

    template <RouteType __rt>
    std::string AdjustmentTable(const ElevationParam<__rt> &p, const ElevationVariable &v, const ElevationInfo &i)
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb, "{}\n{}\n{}\n", p.ToString(), v.ToString(), i.ToString());
        return sb;
    }
    template <RouteType __rt>
    std::string AdjustmentTable(const ElevationParam<__rt> &p, const ElevationAdjustResult &r)
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb, "{}\n{}\n{}\n", p.ToString(), r.variable.ToString(), r.info.ToString());
        return sb;
    }

    template <RouteType __rt>
    void InitElevationVariableSizeFromParam(const ElevationParam<__rt> &param, ElevationVariable &va)
    {
        auto s = param.distances.size();
        va.dh.resize(s);
        va.h_c.resize(s);
        va.H.resize(s + 1);
    }

    template <RouteType __rt>
    void __CalculateFh(const ElevationParam<__rt> &param, ElevationVariable &va, ElevationInfo &info, int place)
    {
        AGTB_TEMPLATE_NOT_SPECIFIED();
    }
    template <>
    void __CalculateFh<RouteType::ClosedLoop>(const ElevationParam<RouteType::ClosedLoop> &param, ElevationVariable &va, ElevationInfo &info, int place)
    {
        info.f_h = TakePlace(
            std::accumulate(param.h.begin(), param.h.end(), 0.0), place);
        info.f_h_tolerance = TakePlace(
            40 * gcem::sqrt(va.dis_sum), place);
        info.v_km = TakePlace(-info.f_h / va.dis_sum, place) * 1000;
    }
    template <>
    void __CalculateFh<RouteType::Connecting>(const ElevationParam<RouteType::Connecting> &param, ElevationVariable &va, ElevationInfo &info, int place)
    {
        info.f_h = TakePlace(
            (std::accumulate(param.h.begin(), param.h.end(), 0.0) -
             (param.H_end - param.H_beg)),
            place);
        info.f_h_tolerance = TakePlace(
            40 * gcem::sqrt(va.dis_sum), place);
        info.v_km = TakePlace(-info.f_h / va.dis_sum, place) * 1000;
    }

    template <RouteType __rt>
    void CalculateDeltaHeightCorrections(const ElevationParam<__rt> &param, ElevationVariable &va, ElevationInfo &info, int place)
    {
        auto &distance = param.distances;
        auto &h = param.h;
        auto &dh = va.dh;

        va.dis_sum = TakePlace(
            std::accumulate(distance.begin(), distance.end(), 0.0), place);
        va.h_sum = TakePlace(
            std::accumulate(h.begin(), h.end(), 0.0), place);

        __CalculateFh(param, va, info, place);

        for (auto i = 0uz; i != distance.size(); ++i)
        {
            dh.at(i) = TakePlace(info.v_km / 1000 * distance.at(i), place);
        }

        va.dh_sum = TakePlace(
            std::accumulate(dh.begin(), dh.end(), 0.0), place);
        if (ApproxEq(va.dh_sum, -info.f_h))
        {
            return;
        }

        double dif = info.f_h + va.dh_sum;
        RefineCorrections(dh, dif, std::signbit(dif), place);
        va.dh_sum = TakePlace(
            std::accumulate(dh.begin(), dh.end(), 0.0), place);
    }

    template <RouteType __rt>
    void AdjustHeight(const ElevationParam<__rt> &param, ElevationVariable &va, int place)
    {
        auto &h = param.h;
        auto &dh = va.dh;
        auto &h_c = va.h_c;
        auto &H = va.H;
        va.H.at(0) = param.H_beg;

        for (auto i = 0uz; i != dh.size(); ++i)
        {
            h_c.at(i) = TakePlace(h.at(i) + dh.at(i), place);
            H.at(i + 1) = TakePlace(H.at(i) + h_c.at(i), place);
        }

        va.h_c_sum = TakePlace(
            std::accumulate(h_c.begin(), h_c.end(), 0.0), place);
    }

    /**
     * @brief Elevation adjustment.
     *
     * @tparam __rt
     * @param param
     * @param place
     * @return ElevationAdjustResult
     */
    template <RouteType __rt>
    ElevationAdjustResult Adjust(const ElevationParam<__rt> &param, int place)
    {
        ElevationVariable variable{};
        ElevationInfo info{};

        InitElevationVariableSizeFromParam(param, variable);
        CalculateDeltaHeightCorrections(param, variable, info, place);
        AdjustHeight(param, variable, place);

        return {variable, info};
    }
}

using Elevation::Adjust;
using Elevation::AdjustmentTable;
using Elevation::ElevationAdjustResult;
using Elevation::ElevationInfo;
using Elevation::ElevationParam;
using Elevation::ElevationVariable;

AGTB_ADJUSTMENT_END

#endif