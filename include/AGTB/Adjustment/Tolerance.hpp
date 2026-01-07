#ifndef __AGTB_ADJUSTMENT_TOLERANCE_HPP__
#define __AGTB_ADJUSTMENT_TOLERANCE_HPP__

#include "Base.hpp"
#include "Traverse.hpp"
#include "Elevation.hpp"

AGTB_ADJUSTMENT_BEGIN

namespace Tolerance
{

    enum class ToleranceCatagory
    {
        Grade,
        Level
    };

    enum class EvaluateTarget
    {
        /**
         * @brief Electromagnetic distance measure traverse
         *
         */
        EDT,
        /**
         * @brief Figure root distance measure traverse
         *
         */
        FDT,
        Elevation
    };

    template <EvaluateTarget __target>
    struct EvaluateParamForward
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    };

    template <>
    struct EvaluateParamForward<EvaluateTarget::EDT>
    {
        using ParamType = TraverseAdjustResult;
    };
    template <>
    struct EvaluateParamForward<EvaluateTarget::FDT>
    {
        using ParamType = TraverseAdjustResult;
    };
    template <>
    struct EvaluateParamForward<EvaluateTarget::Elevation>
    {
        using ParamType = ElevationAdjustResult;
    };

    template <typename T>
    concept EvaluateParamForwardConcept = requires {
        typename T::ParamType;
    } && requires(T::ParamType p) {
        { p.N() } -> std::convertible_to<int>;
    };

    template <EvaluateTarget __target>
        requires EvaluateParamForwardConcept<EvaluateParamForward<__target>>
    using EvaluateParam = EvaluateParamForward<__target>::ParamType;

    /**
     * @brief Boolean represents if each target meets tolerence
     *
     * @tparam __target
     */
    template <EvaluateTarget __target>
    struct EvaluateResult
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    };

    template <>
    struct EvaluateResult<EvaluateTarget::EDT>
    {
        bool K, f_bate;
    };
    template <>
    struct EvaluateResult<EvaluateTarget::FDT>
    {
        bool K, f_bate;
    };
    template <>
    struct EvaluateResult<EvaluateTarget::Elevation>
    {
        bool f_h;
    };

    /**
     * @brief Values of tolerence
     *
     * @tparam __target
     * @tparam __cat
     * @tparam __num
     */
    template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
    struct ToleranceValue
    {
        AGTB_TEMPLATE_NOT_SPECIALIZED();
    };

    /**
     * @brief EDT
     *
     * @tparam
     */
    template <>
    struct ToleranceValue<EvaluateTarget::EDT, ToleranceCatagory::Grade, 3>
    {
        constexpr static int K_inv = 60'000;
        static double FBeta(const EvaluateParam<EvaluateTarget::EDT> &p)
        {
            return 3 * gcem::sqrt(p.N());
        }
    };

    template <>
    struct ToleranceValue<EvaluateTarget::EDT, ToleranceCatagory::Grade, 4>
    {
        constexpr static int K_inv = 40'000;
        static double FBeta(const EvaluateParam<EvaluateTarget::EDT> &p)
        {
            return 5 * gcem::sqrt(p.N());
        }
    };

    template <>
    struct ToleranceValue<EvaluateTarget::EDT, ToleranceCatagory::Level, 1>
    {
        constexpr static int K_inv = 14'000;
        static double FBeta(const EvaluateParam<EvaluateTarget::EDT> &p)
        {
            return 10 * gcem::sqrt(p.N());
        }
    };
    template <>
    struct ToleranceValue<EvaluateTarget::EDT, ToleranceCatagory::Level, 2>
    {
        constexpr static int K_inv = 10'000;
        static double FBeta(const EvaluateParam<EvaluateTarget::EDT> &p)
        {
            return 16 * gcem::sqrt(p.N());
        }
    };
    template <>
    struct ToleranceValue<EvaluateTarget::EDT, ToleranceCatagory::Level, 3>
    {
        constexpr static int K_inv = 6'000;
        static double FBeta(const EvaluateParam<EvaluateTarget::EDT> &p)
        {
            return 24 * gcem::sqrt(p.N());
        }
    };

    // TODO: ToleranceValue<EvaluateTarget::FDT, ... >
    // TODO: ToleranceValue<EvaluateTarget::Elevation, ... >

    namespace ToleranceEvaluatorImpl
    {
        template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
        struct Impl
        {
            AGTB_TEMPLATE_NOT_SPECIALIZED();
        };

        template <ToleranceCatagory __cat, size_t __num>
        struct Impl<EvaluateTarget::EDT, __cat, __num>
        {
            constexpr static EvaluateTarget target = EvaluateTarget::EDT;
            using __Value = ToleranceValue<target, __cat, __num>;

            static EvaluateResult<target> Evaluate(const EvaluateParam<target> &p)
            {
                return {
                    .K = p.info.K_inv > __Value::K_inv,
                    .f_bate = p.info.f_beta < __Value::FBeta(p)};
            }
        };

        // template <ToleranceCatagory __cat, size_t __num>
        // struct Impl<EvaluateTarget::FDT, __cat, __num>
        // {
        //     constexpr static EvaluateTarget target = EvaluateTarget::FDT;
        //     using __Value = ToleranceValue<target, __cat, __num>;

        //     static EvaluateResult<target> Evaluate(const EvaluateParam<target> &p)
        //     {
        //         return
        //         {
        //             .K = p.info.K_inv > __Value::K_inv,
        //             .f_bate = p.info.f_beta < __Value::FBeta(p);
        //         }
        //     }
        // };

        // template <ToleranceCatagory __cat, size_t __num>
        // struct Impl<EvaluateTarget::Elevation, __cat, __num>
        // {
        //     constexpr static EvaluateTarget target = EvaluateTarget::Elevation;
        //     using __Value = ToleranceValue<target, __cat, __num>;

        //     static EvaluateResult<target> Evaluate(const EvaluateParam<target> &p)
        //     {
        //         return
        //         {
        //             .f_h = p.info.f_h < __Value::FH(p);
        //         };
        //     }
        // };

        template <typename T>
        concept ImplConcept = requires {
            { T::target } -> std::convertible_to<EvaluateTarget>;
        } && requires(const EvaluateParam<T::target> &p) {
            { T::Evaluate(p) } -> std::convertible_to<EvaluateResult<T::target>>;
        };

        template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
        struct CheckImpl
        {
            using __Impl = Impl<__target, __cat, __num>;

            template <ImplConcept T>
            struct DoCheck
            {
            };

            using __Check = DoCheck<__Impl>;
        };

        template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
        using CheckedImpl = CheckImpl<__target, __cat, __num>::__Impl;
    }

    template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
    using ToleranceEvaluator = ToleranceEvaluatorImpl::CheckedImpl<__target, __cat, __num>;

    template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
    EvaluateResult<__target> EvaluateTolerance(const EvaluateParam<__target> &p)
    {
        return ToleranceEvaluator<__target, __cat, __num>::Evaluate(p);
    }

    template <EvaluateTarget __target, ToleranceCatagory __cat, size_t __num>
    struct EvaluateToleranceTParam
    {
        constexpr static EvaluateTarget target = __target;
        constexpr static ToleranceCatagory catgory = __cat;
        constexpr static size_t num = __num;

        using Result = EvaluateResult<target>;
        using Param = EvaluateParam<target>;
        using Value = ToleranceValue<target, catgory, num>;
        using Evaluator = ToleranceEvaluator<target, catgory, num>;
    };

    template <typename T>
    concept EvaluateToleranceTParamConcept = requires {
        { T::target } -> std::convertible_to<EvaluateTarget>;
        { T::catgory } -> std::convertible_to<ToleranceCatagory>;
        { T::num } -> std::convertible_to<size_t>;
        typename T::Result;
        typename T::Param;
        typename T::Value;
        typename T::Evaluator;
    };

    template <EvaluateToleranceTParamConcept Tp>
    Tp::Result EvaluateTolerance(const typename Tp::Param &p)
    {
        return Tp::Evaluator::Evaluate(p);
    }
}

using Tolerance::EvaluateResult;
using Tolerance::EvaluateTarget;
using Tolerance::EvaluateTolerance;
using Tolerance::EvaluateToleranceTParam;
using Tolerance::ToleranceCatagory;

AGTB_ADJUSTMENT_END

#endif