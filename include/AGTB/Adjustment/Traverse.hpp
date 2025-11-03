#ifndef AGTB_ADJUSTMENT_TRAVERSE_HPP
#define AGTB_ADJUSTMENT_TRAVERSE_HPP

#include "../details/Macros.hpp"
#include "../Utils/Angles.hpp"
#include "Base.hpp"

#include <vector>
#include <numeric>
#include <format>
#include <concepts>

AGTB_ADJUSTMENT_BEGIN

namespace Traverse
{
    using namespace AGTB::Utils::Angles::DMS_Support;

    enum class TraverseShape
    {
        Closed,
        Attached
    };

    struct TraverseParam
    {
        TraverseShape shape;
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
            std::format_to(sbb, "X_beg = {}, Y_beg = {}\n", x_beg, y_beg);
            if (shape == TraverseShape::Attached)
            {
                std::format_to(sbb, "X_end = {}, Y_end = {}\n", x_end, y_end);
            }

            std::format_to(sbb, "\n{:-^50}\n", " Azimuth ");
            std::format_to(sbb, "Azimuth_beg = {}\n", azi_beg.ToString());
            if (shape == TraverseShape::Attached)
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
        Angle delta_a, a_c_sum;
        std::vector<Angle> a_c, azi_c;
        std::vector<double> dx, dy, dx_c, dy_c, x, y, ddx, ddy;
        double dis_sum, dx_sum, dy_sum, dx_c_sum, dy_c_sum;

        std::string ToString() const noexcept
        {
            std::string sb{};
            auto sbb = std::back_inserter(sb);
            std::format_to(sbb, "{:=^100}\n", " TraverseVariable ");

            std::format_to(sbb, "{:-^50}\n", " Correction of Angles ");
            std::format_to(sbb, "[{}]\n", delta_a.ToString());

            std::format_to(sbb, "{:-^50}\n", " Corrected Angles ");
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
                           " Corrected Azimuth = {}\n Distance = {} (m)\n Delta X = {} (m)\n Delta Y = {} (m)\n Delta X_c = {} (m)\n Delta Y_c = {} (m)\n",
                           a_c_sum.ToString(), dis_sum, dx_sum, dy_sum, dx_c_sum, dy_c_sum);

            return sb;
        };
    };

    template <TraverseShape shape>
    class AdjustorBase
    {
    protected:
        TraverseParam param;
        TraverseVariable va;
        TraverseInfo info;
        bool is_solved;

    public:
        AdjustorBase(TraverseParam _p)
            : param(std::move(_p)), is_solved(false)
        {
            if (param.shape != shape)
            {
                AGTB_THROW(Utils::constructor_error, "Param.shape not correct");
            }

            auto a_s = param.angles.size(), d_s = param.distances.size();
            if (shape == TraverseShape::Closed && a_s != d_s)
            {
                AGTB_THROW(Utils::constructor_error, "[TraverseShape::Closed] Angles and distances must have same size");
            }
            else if (shape == TraverseShape::Attached && a_s != d_s + 1)
            {
                AGTB_THROW(Utils::constructor_error, "[TraverseShape::Attached] Angles.size must equals to distances.size + 1");
            }
        }
        virtual ~AdjustorBase() = default;

        const TraverseInfo &Info() const
        {
            if (is_solved)
            {
                return info;
            }
            else
            {
                AGTB_THROW(std::runtime_error, "ClosedLoopSolver haven't solve it.");
            }
        }

        const TraverseVariable &Variable() const
        {
            if (is_solved)
            {
                return va;
            }
            else
            {
                AGTB_THROW(std::runtime_error, "ClosedLoopSolver haven't solve it.");
            }
        }

        const TraverseParam &Param() const noexcept
        {
            return param;
        }

        virtual bool Solve(int precision)
        {
            AGTB_NOT_IMPLEMENT();
        }
    };

    class ClosedAdjustor : public AdjustorBase<TraverseShape::Closed>
    {

    public:
        ClosedAdjustor(TraverseParam _p)
            : AdjustorBase<TraverseShape::Closed>(std::move(_p))
        {
        }

        bool Solve(int take_precision = 2) override
        {
            auto &a = param.angles;
            auto n = a.size();

            Angle a_sum = std::accumulate(a.begin(), a.end(), Angle());
            Angle a_sum_expect = Angle((n - 2) * 180, 0, 0);
            info.f_beta = a_sum - a_sum_expect;
            va.delta_a = -info.f_beta / n;

            va.a_c.resize(n);
            va.azi_c.resize(n + 1);
            va.azi_c.at(0) = param.azi_beg;
            for (auto i = 0uz; i != n; ++i)
            {
                va.a_c.at(i) = (a.at(i) + va.delta_a).NormStd();
                va.azi_c.at(i + 1) = (va.a_c.at(i) + va.azi_c.at(i) - A180d).NormStd();
            }
            va.a_c_sum = std::accumulate(va.a_c.begin(), va.a_c.end(), Angle());

            va.dx.resize(n);
            va.ddx.resize(n);
            va.dy.resize(n);
            va.ddy.resize(n);
            va.dx_c.resize(n);
            va.dy_c.resize(n);
            va.x.resize(n + 1);
            va.y.resize(n + 1);
            va.x.at(0) = param.x_beg;
            va.y.at(0) = param.y_beg;

            auto &dis = param.distances;
            va.dis_sum = std::accumulate(dis.begin(), dis.end(), 0);

            for (auto i = 0; i != n; ++i)
            {
                auto dis_i = dis.at(i);
                auto azi_i = va.azi_c.at(i);
                va.dx.at(i) = TakePrecision(azi_i.Cos() * dis_i, take_precision);
                va.dy.at(i) = TakePrecision(azi_i.Sin() * dis_i, take_precision);
            }

            va.dx_sum = TakePrecision(std::accumulate(va.dx.begin(), va.dx.end(), 0.0), take_precision);
            va.dy_sum = TakePrecision(std::accumulate(va.dy.begin(), va.dy.end(), 0.0), take_precision);

            info.f_x = va.dx_sum;
            info.f_y = va.dy_sum;
            info.f = TakePrecision(
                gcem::sqrt(
                    gcem::pow(info.f_x, 2) +
                    gcem::pow(info.f_y, 2)),
                take_precision);
            info.K_inv = gcem::ceil(va.dis_sum / info.f);

            for (auto i = 0; i != n; ++i)
            {
                auto &dis_i = param.distances.at(i);
                va.ddx.at(i) = -info.f_x * dis_i / va.dis_sum;
                va.ddy.at(i) = -info.f_y * dis_i / va.dis_sum;

                va.dx_c.at(i) = TakePrecision(va.dx.at(i) + va.ddx.at(i), take_precision);
                va.dy_c.at(i) = TakePrecision(va.dy.at(i) + va.ddy.at(i), take_precision);
            }

            va.dx_c_sum = std::accumulate(va.dx_c.begin(), va.dx_c.end(), 0.0);
            va.dy_c_sum = std::accumulate(va.dy_c.begin(), va.dy_c.end(), 0.0);

            for (auto i = 0; i != n; ++i)
            {
                va.x.at(i + 1) = TakePrecision(va.x.at(i) + va.dx_c.at(i), take_precision);
                va.y.at(i + 1) = TakePrecision(va.y.at(i) + va.dy_c.at(i), take_precision);
            }

            return is_solved = true;
        }
    };

    class AttachedAdjustor : public AdjustorBase<TraverseShape::Attached>
    {
    public:
        AttachedAdjustor(TraverseParam _p)
            : AdjustorBase<TraverseShape::Attached>(std::move(_p))
        {
        }

        bool Solve(int take_precision = 2) override
        {
            auto &a = param.angles;
            auto n_a = a.size(), n_d = param.distances.size();

            Angle a_sum = std::accumulate(a.begin(), a.end(), Angle());
            info.f_beta = a_sum - (param.azi_end - param.azi_beg) - Angle(n_a * 180, 0, 0);
            va.delta_a = -info.f_beta / n_a;

            va.a_c.resize(n_a);
            va.azi_c.resize(n_a + 1);
            va.azi_c.at(0) = param.azi_beg;
            for (auto i = 0uz; i != n_a; ++i)
            {
                va.a_c.at(i) = (a.at(i) + va.delta_a).NormStd();
                va.azi_c.at(i + 1) = (va.a_c.at(i) + va.azi_c.at(i) - A180d).NormStd();
            }
            va.a_c_sum = std::accumulate(va.a_c.begin(), va.a_c.end(), Angle());

            va.dx.resize(n_d);
            va.ddx.resize(n_d);
            va.dy.resize(n_d);
            va.ddy.resize(n_d);
            va.dx_c.resize(n_d);
            va.dy_c.resize(n_d);
            va.x.resize(n_a);
            va.y.resize(n_a);
            va.x.at(0) = param.x_beg;
            va.y.at(0) = param.y_beg;

            auto &dis = param.distances;
            va.dis_sum = std::accumulate(dis.begin(), dis.end(), 0);

            for (auto i = 0; i != n_d; ++i)
            {
                auto dis_i = dis.at(i);
                auto azi_i = va.azi_c.at(i + 1);
                va.dx.at(i) = TakePrecision(azi_i.Cos() * dis_i, take_precision);
                va.dy.at(i) = TakePrecision(azi_i.Sin() * dis_i, take_precision);
            }

            va.dx_sum = TakePrecision(std::accumulate(va.dx.begin(), va.dx.end(), 0.0), take_precision);
            va.dy_sum = TakePrecision(std::accumulate(va.dy.begin(), va.dy.end(), 0.0), take_precision);

            info.f_x = va.dx_sum - (param.x_end - param.x_beg);
            info.f_y = va.dy_sum - (param.y_end - param.y_beg);
            info.f = TakePrecision(
                gcem::sqrt(
                    gcem::pow(info.f_x, 2) +
                    gcem::pow(info.f_y, 2)),
                take_precision);
            info.K_inv = gcem::ceil(va.dis_sum / info.f);

            for (auto i = 0; i != n_d; ++i)
            {
                auto &dis_i = param.distances.at(i);
                va.ddx.at(i) = -info.f_x * dis_i / va.dis_sum;
                va.ddy.at(i) = -info.f_y * dis_i / va.dis_sum;

                va.dx_c.at(i) = TakePrecision(va.dx.at(i) + va.ddx.at(i), take_precision);
                va.dy_c.at(i) = TakePrecision(va.dy.at(i) + va.ddy.at(i), take_precision);
            }

            va.dx_c_sum = std::accumulate(va.dx_c.begin(), va.dx_c.end(), 0.0);
            va.dy_c_sum = std::accumulate(va.dy_c.begin(), va.dy_c.end(), 0.0);

            for (auto i = 0; i != n_d; ++i)
            {
                va.x.at(i + 1) = TakePrecision(va.x.at(i) + va.dx_c.at(i), take_precision);
                va.y.at(i + 1) = TakePrecision(va.y.at(i) + va.dy_c.at(i), take_precision);
            }

            return is_solved = true;
        }
    };

    std::string SolveResult(const TraverseParam &p, const TraverseVariable &v, const TraverseInfo &i)
    {
        std::string sb{};
        auto sbb = std::back_inserter(sb);
        std::format_to(sbb, "{}\n{}\n{}\n", p.ToString(), v.ToString(), i.ToString());
        return sb;
    }

    template <typename T>
    concept TraverseSolverConcept = requires(T t) {
        { t.Info() } -> std::convertible_to<TraverseInfo>;
        { t.Variable() } -> std::convertible_to<TraverseVariable>;
        { t.Param() } -> std::convertible_to<TraverseParam>;
    };

    template <TraverseSolverConcept solver>
    std::string SolveResultOf(const solver &s)
    {
        return SolveResult(s.Param(), s.Variable(), s.Info());
    }
} // namespace Traverse

AGTB_ADJUSTMENT_END

#endif