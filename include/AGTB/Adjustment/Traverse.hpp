#ifndef __AGTB_ADJUSTMENT_TRAVERSE_HPP__
#define __AGTB_ADJUSTMENT_TRAVERSE_HPP__

#include "../details/Macros.hpp"
#include "../Utils/Angles.hpp"
#include "Base.hpp"
#include "RefineCorrection.hpp"

#include <vector>
#include <numeric>
#include <format>

AGTB_ADJUSTMENT_BEGIN

namespace Traverse
{
    using AGTB::Utils::Angles::Angle;

    template <RouteType __rt>
    struct TraverseParam
    {
        constexpr static RouteType route_type = __rt;
        std::vector<double> distances;
        std::vector<Angle> angles;
        Angle azi_beg, azi_end;
        double x_beg, y_beg, x_end, y_end;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);
            std::format_to(sbb, "{:=^100}\n", " TraverseParam ");
            std::format_to(sbb, "{:-^50}\n", " Distances (m) ");
            for (auto &d : distances)
            {
                std::format_to(sbb, "{} ", d);
            }
            std::format_to(sbb, "\n{:-^50}\n", " Angles ");
            for (auto &a : angles)
            {
                std::format_to(sbb, "[{}] ", a.ToString());
            }
            std::format_to(sbb, "\n{:-^50}\n", " (X,Y) (m) ");
            std::format_to(sbb, "X_beg = {}, Y_beg = {}", x_beg, y_beg);
            if (route_type == RouteType::Connecting)
            {
                std::format_to(sbb, "\nX_end = {}, Y_end = {}\n", x_end, y_end);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Azimuth ");
            std::format_to(sbb, "Azimuth_beg = {}\n", azi_beg.ToString());
            if (route_type == RouteType::Connecting)
            {
                std::format_to(sbb, "Azimuth_end = {}\n", azi_end.ToString());
            }
            return sb;
        }
    };

    struct TraverseInfo
    {
        double
            f_x,
            f_y, f,
            K_inv;
        Angle f_beta;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);
            std::format_to(sbb, "{:=^100}\n", " TraverseInfo ");
            std::format_to(sbb,
                           " f_x = {} (m)\n f_y= {} (m)\n f= {} (m)\n K_inv = {}\n f_beta = {}\n",
                           f_x, f_y, f, K_inv, f_beta.ToString());
            return sb;
        }
    };

    struct TraverseVariable
    {
        Angle a_c_sum, a_sum, da_sum;
        std::vector<Angle> a_c, azi_c, delta_a;
        std::vector<double> dx, dy, dx_c, dy_c, x, y, ddx, ddy;
        double dis_sum, dx_sum, dy_sum, dx_c_sum, dy_c_sum;
        double ddx_sum, ddy_sum;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);
            std::format_to(sbb, "{:=^100}\n", " TraverseVariable ");

            std::format_to(sbb, "{:-^50}\n", " Correction of Angles ");
            for (auto &e : delta_a)
            {
                std::format_to(sbb, "[{}] ", e.ToString());
            }

            std::format_to(sbb, "\n{:-^50}\n", " Corrected Angles ");
            for (auto &e : a_c)
            {
                std::format_to(sbb, "[{}] ", e.ToString());
            }

            std::format_to(sbb, "\n{:-^50}\n", " Azimuth ");
            for (auto &e : azi_c)
            {
                std::format_to(sbb, "[{}] ", e.ToString());
            }

            std::format_to(sbb, "\n{:-^50}\n", " Delta X (m) ");
            for (auto &e : dx)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Delta Y (m) ");
            for (auto &e : dy)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Correction of Delta X (m) ");
            for (auto &e : ddx)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Correction of Delta Y (m) ");
            for (auto &e : ddy)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Corrected Delta X (m) ");
            for (auto &e : dx_c)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Corrected Delta X (m) ");
            for (auto &e : dy_c)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " X (m) ");
            for (auto &e : x)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Y (m) ");
            for (auto &e : y)
            {
                std::format_to(sbb, "{} ", e);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Sum Of ");
            std::format_to(sbb,
                           " Angle = {}\n Delta Angle = {}\n Corrected Angle = {}\n Distance = {} (m)\n Delta X = {} (m)\n Delta Y = {} (m)\n Delta X_c = {} (m)\n Delta Y_c = {} (m)\n",
                           a_sum.ToString(), da_sum.ToString(), a_c_sum.ToString(), dis_sum, dx_sum, dy_sum, dx_c_sum, dy_c_sum);

            return sb;
        };
    };

    struct TraverseAdjustResult
    {
        TraverseVariable variable;
        TraverseInfo info;
    };

    template <RouteType __rt>
    void InitTraverseVariableSizeFromParam(const TraverseParam<__rt> &param, TraverseVariable &va)
    {
        size_t dn = param.distances.size(), an = param.angles.size();
        va.dx.resize(dn);
        va.dy.resize(dn);
        va.ddx.resize(dn);
        va.ddy.resize(dn);
        va.dx_c.resize(dn);
        va.dy_c.resize(dn);
        va.x.resize(dn + 1);
        va.y.resize(dn + 1);
        va.delta_a.resize(an);
        va.a_c.resize(an);
        va.azi_c.resize(an + 1);
    }

    template <RouteType __rt>
    void __CalculateFBeta(const TraverseParam<__rt> &param, TraverseVariable &va, TraverseInfo &info)
    {
        AGTB_UNKNOWN_TEMPLATE_PARAM();
    }
    template <>
    void __CalculateFBeta<RouteType::Closed>(const TraverseParam<RouteType::Closed> &param, TraverseVariable &va, TraverseInfo &info)
    {
        info.f_beta = va.a_sum - Angle((param.angles.size() - 2) * 180, 0, 0);
    }
    template <>
    void __CalculateFBeta<RouteType::Connecting>(const TraverseParam<RouteType::Connecting> &param, TraverseVariable &va, TraverseInfo &info)
    {
        info.f_beta = va.a_sum - (param.azi_end - param.azi_beg) - Angle((param.angles.size()) * 180, 0, 0);
    }

    template <RouteType __rt>
    void CalculateAngleCorrections(const TraverseParam<__rt> &param, TraverseVariable &va, TraverseInfo &info)
    {
        auto &angles = param.angles;
        auto &corrections = va.delta_a;

        va.a_sum = std::accumulate(angles.begin(), angles.end(), Angle());
        __CalculateFBeta(param, va, info);

        double ang_seconds = TakePlace((-info.f_beta / angles.size()).Seconds(), 0);
        std::fill(corrections.begin(), corrections.end(), Angle(ang_seconds));

        va.da_sum = std::accumulate(corrections.begin(), corrections.end(), Angle());

        if (va.da_sum == -info.f_beta)
        {
            return;
        }

        Angle dif = info.f_beta + va.da_sum;
        RefineCorrections(corrections, dif, dif.Sign());
        va.da_sum = std::accumulate(corrections.begin(), corrections.end(), Angle());
    }

    template <RouteType __rt>
    void AdjustAngles(const TraverseParam<__rt> &param, TraverseVariable &va)
    {
        auto &angles = param.angles;
        auto &corrections = va.delta_a;
        auto &corrected_angles = va.a_c;
        auto &azimuth = va.azi_c;
        azimuth.at(0) = param.azi_beg;

        for (auto i = 0uz; i != angles.size(); ++i)
        {
            corrected_angles.at(i) = (angles.at(i) + corrections.at(i)).NormStd();
            azimuth.at(i + 1) = (corrected_angles.at(i) + azimuth.at(i) - Utils::Angles::A180d).NormStd();
        }

        va.a_c_sum = std::accumulate(corrected_angles.begin(), corrected_angles.end(), Angle());
    }

    template <RouteType __rt>
    inline size_t __CorrespondingAzimuthIdx(size_t dis_i)
    {
        AGTB_UNKNOWN_TEMPLATE_PARAM();
    }
    template <>
    inline size_t __CorrespondingAzimuthIdx<RouteType::Closed>(size_t dis_i)
    {
        return dis_i;
    }
    template <>
    inline size_t __CorrespondingAzimuthIdx<RouteType::Connecting>(size_t dis_i)
    {
        return dis_i + 1;
    }

    template <RouteType __rt>
    void __CalculateFxFyFK(const TraverseParam<__rt> &param, TraverseVariable &va, TraverseInfo &info, int place)
    {
        AGTB_UNKNOWN_TEMPLATE_PARAM();
    }
    template <>
    void __CalculateFxFyFK<RouteType::Closed>(const TraverseParam<RouteType::Closed> &param, TraverseVariable &va, TraverseInfo &info, int place)
    {
        info.f_x = va.dx_sum;
        info.f_y = va.dy_sum;
        info.f = TakePlace(gcem::sqrt(
                               gcem::pow(info.f_x, 2) +
                               gcem::pow(info.f_y, 2)),
                           place);
        info.K_inv = gcem::ceil(va.dis_sum / info.f);
    }
    template <>
    void __CalculateFxFyFK<RouteType::Connecting>(const TraverseParam<RouteType::Connecting> &param, TraverseVariable &va, TraverseInfo &info, int place)
    {
        info.f_x = TakePlace(va.dx_sum - (param.x_end - param.x_beg), place);
        info.f_y = TakePlace(va.dy_sum - (param.y_end - param.y_beg), place);
        info.f = TakePlace(gcem::sqrt(
                               gcem::pow(info.f_x, 2) +
                               gcem::pow(info.f_y, 2)),
                           place);
        info.K_inv = gcem::ceil(va.dis_sum / info.f);
    }

    template <RouteType __rt>
    void CalculateDeltaCoordinates(const TraverseParam<__rt> &param, TraverseVariable &va, TraverseInfo &info, int place)
    {
        auto &distances = param.distances;
        auto &azimuth = va.azi_c;
        auto &dx = va.dx;
        auto &dy = va.dy;

        for (auto i = 0uz; i != distances.size(); ++i)
        {
            auto di = distances.at(i);
            auto ai = azimuth.at(
                __CorrespondingAzimuthIdx<__rt>(i));
            dx.at(i) = TakePlace(ai.Cos() * di, place);
            dy.at(i) = TakePlace(ai.Sin() * di, place);
        }
        va.dx_sum = TakePlace(
            std::accumulate(dx.begin(), dx.end(), 0.0), place);
        va.dy_sum = TakePlace(
            std::accumulate(dy.begin(), dy.end(), 0.0), place);

        va.dis_sum = std::accumulate(distances.begin(), distances.end(), 0.0);
        __CalculateFxFyFK<__rt>(param, va, info, place);
    }

    template <RouteType __rt>
    void CalculateDeltaCoordinateCorrections(const TraverseParam<__rt> &param, TraverseVariable &va, TraverseInfo &info, int place)
    {
        auto &distances = param.distances;
        auto &ddx = va.ddx;
        auto &ddy = va.ddy;

        for (auto i = 0uz; i != distances.size(); ++i)
        {
            auto dis_i = distances.at(i);
            ddx.at(i) = TakePlace(-info.f_x * dis_i / va.dis_sum, place);
            ddy.at(i) = TakePlace(-info.f_y * dis_i / va.dis_sum, place);
        }

        va.ddx_sum = std::accumulate(ddx.begin(), ddx.end(), 0.0);
        va.ddy_sum = std::accumulate(ddy.begin(), ddy.end(), 0.0);

        if (!ApproxEq(va.ddx_sum, -info.f_x))
        {
            auto dif = va.ddx_sum + info.f_x;
            auto idx = RefineCorrections(va.ddx, dif, std::signbit(dif));
            ddx.at(idx) = TakePlace(ddx.at(idx), place);
        }

        if (!ApproxEq(va.ddy_sum, -info.f_y))
        {
            auto dif = va.ddy_sum + info.f_y;
            auto idx = RefineCorrections(va.ddy, dif, std::signbit(dif));
            ddy.at(idx) = TakePlace(ddy.at(idx), place);
        }
    }

    template <RouteType __rt>
    void AdjustCoordinates(const TraverseParam<__rt> &param, TraverseVariable &va, int place)
    {
        auto &dx = va.dx;
        auto &dy = va.dy;
        auto &ddx = va.ddx;
        auto &ddy = va.ddy;
        auto &dx_c = va.dx_c;
        auto &dy_c = va.dy_c;
        auto &x = va.x;
        auto &y = va.y;
        x.at(0) = param.x_beg;
        y.at(0) = param.y_beg;

        for (auto i = 0uz; i != dx.size(); ++i)
        {
            dx_c.at(i) = TakePlace(dx.at(i) + ddx.at(i), place);
            dy_c.at(i) = TakePlace(dy.at(i) + ddy.at(i), place);
        }
        va.dx_c_sum = std::accumulate(dx_c.begin(), dx_c.end(), 0.0);
        va.dy_c_sum = std::accumulate(dy_c.begin(), dy_c.end(), 0.0);

        for (auto i = 0uz; i != dx.size(); ++i)
        {
            x.at(i + 1) = TakePlace(x.at(i) + dx_c.at(i), place);
            y.at(i + 1) = TakePlace(y.at(i) + dy_c.at(i), place);
        }
    }

    /**
     * @brief Traverse adjustment.
     *
     * @tparam __rt
     * @param param
     * @param place precision
     * @return TraverseAdjustResult
     */
    template <RouteType __rt>
    TraverseAdjustResult Adjust(const TraverseParam<__rt> &param, int place)
    {
        TraverseVariable variable{};
        TraverseInfo info{};

        InitTraverseVariableSizeFromParam(param, variable);

        CalculateAngleCorrections(param, variable, info);
        AdjustAngles(param, variable);

        CalculateDeltaCoordinates(param, variable, info, place);
        CalculateDeltaCoordinateCorrections(param, variable, info, place);
        AdjustCoordinates(param, variable, place);

        return {variable, info};
    }

    template <RouteType __rt>
    std::string AdjustmentTable(const TraverseParam<__rt> &p, const TraverseVariable &v, const TraverseInfo &i)
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb, "{}\n{}\n{}\n", p.ToString(), v.ToString(), i.ToString());
        return sb;
    }
    template <RouteType __rt>
    std::string AdjustmentTable(const TraverseParam<__rt> &p, const TraverseAdjustResult &r)
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb, "{}\n{}\n{}\n", p.ToString(), r.variable.ToString(), r.info.ToString());
        return sb;
    }
} // namespace Traverse

using Traverse::Adjust;
using Traverse::AdjustmentTable;
using Traverse::TraverseAdjustResult;
using Traverse::TraverseInfo;
using Traverse::TraverseParam;
using Traverse::TraverseVariable;

AGTB_ADJUSTMENT_END

#endif