#pragma once

#ifndef __AGTB_IO_EIGEN_HPP__
#define __AGTB_IO_EIGEN_HPP__

#include <concepts>
#include <type_traits>
#include <Eigen/Dense>
#include <ranges>
#include <algorithm>

#include "../details/Macros.hpp"
#include "../Utils/StreamReader.hpp"
#include "../Utils/String.hpp"

AGTB_IO_BEGIN

namespace detail::EigenIO
{

    template <typename T>
    concept EigenMatrixMetaData = requires {
        typename T::value_type;
        { T::rows } -> std::convertible_to<int>;
        { T::cols } -> std::convertible_to<int>;
    };

    template <typename T>
    concept EigenMatrix = requires {
        typename T::value_type;
        { T::RowsAtCompileTime } -> std::convertible_to<int>;
        { T::ColsAtCompileTime } -> std::convertible_to<int>;
    };

    template <EigenMatrixMetaData mmd>
    using MatrixUsing = Eigen::Matrix<typename mmd::value_type, mmd::row, mmd::col>;

    template <typename valT, int _r, int _c>
    struct MatrixMetaData
    {
        using value_type = valT;
        constexpr static int rows = _r, cols = _c;
    };

    template <EigenMatrix em>
    using MMDOf = MatrixMetaData<typename em::value_type, em::RowsAtCompileTime, em::ColsAtCompileTime>;

    namespace details
    {
        template <typename value_type, int cols>
        struct ColContainer
        {
            using container = std::array<value_type, cols>;
        };

        template <typename value_type>
        struct ColContainer<value_type, Eigen::Dynamic>
        {
            using container = std::vector<value_type>;
        };

        template <typename col_container, int row>
        struct RowContainer
        {
            using container = std::array<col_container, row>;
        };

        template <typename col_container>
        struct RowContainer<col_container, Eigen::Dynamic>
        {
            using container = std::vector<col_container>;
        };

        template <EigenMatrix em>
        struct ContainerOf
        {
            using mmd = MMDOf<em>;
            using value_type = typename mmd::value_type;
            constexpr static int rows = mmd::rows, cols = mmd::cols;
            using col_container = typename ColContainer<value_type, cols>::container;
            using container = typename RowContainer<col_container, rows>::container;
        };

        template <EigenMatrixMetaData mmd, typename container, EigenMatrix em>
        void ContainerToMatrix(em &mat, const container &c)
        {
            const int row = c.size(),
                      col = c.begin()->size();
            for (auto ri = 0; ri != row; ++ri)
            {
                for (auto ci = 0; ci != col; ++ci)
                {
                    mat(ri, ci) = c.at(ri).at(ci);
                }
            }
        }

    }

    template <EigenMatrix em>
    using ContainerOf = details::ContainerOf<em>::container;

    template <EigenMatrixMetaData mmd, typename container>
    bool IsValidContainer(const container &c)
    {
        if constexpr (mmd::rows != Eigen::Dynamic && mmd::cols != Eigen::Dynamic)
        {
            // array<array>
            return true;
        }
        else if constexpr (mmd::rows == Eigen::Dynamic && mmd::cols != Eigen::Dynamic)
        {
            // vector<array>
            return true;
        }
        else
        {
            // array<vector>
            // vector<vector>
            if (c.empty())
                return true; // 处理空容器情况

            auto it = c.begin();
            if (it == c.end())
                return true;              // 再次确认空
            const auto size = it->size(); // 获取第一个子容器的大小

            return std::all_of(c.begin(), c.end(), [&size](const auto &sub)
                               { return sub.size() == size; });
        }
    }

    template <EigenMatrixMetaData mmd, typename container, EigenMatrix em>
    void ReadEigenCustom(std::istream &is, em &mat, const std::string sep = ",")
    {
        container c{};

        auto sr = Utils::StreamReader(is);
        int line_counter = 0;
        while (sr.Ok() && line_counter++ != mmd::rows)
        {
            std::string line = sr.NextLine();
            auto vec = Utils::SplitThenConv<typename mmd::value_type>(line, sep);
            auto col = mmd::cols != Eigen::Dynamic ? mmd::cols : vec.size();

            c.push_back({});
            auto &cur = *c.rbegin();

            if constexpr (!std::is_array_v<decltype(cur)>)
            {
                c.rbegin()->resize(col);
            }

            for (auto i = 0; i != col; ++i)
            {
                cur.at(i) = vec.at(i);
            }
        }

        if (IsValidContainer<mmd, container>(c))
        {
            details::ContainerToMatrix<mmd, container>(mat, c);
        }
        else
        {
            AGTB_THROW(std::runtime_error, "Invalid container size");
        }
    }
}

/**
 * @brief Read eigen matrix from stream, each line is separated by `sep`
 *
 * @tparam em Eigen matrix type
 * @param is
 * @param mat
 * @param sep
 */
template <detail::EigenIO::EigenMatrix em>
void ReadEigen(std::istream &is, em &mat, const std::string sep = ",")
{
    using mmd = detail::EigenIO::MMDOf<em>;
    using container = detail::EigenIO::ContainerOf<em>;
    container c{};

    return detail::EigenIO::ReadEigenCustom<mmd, container, em>(is, mat, sep);
}

namespace EigenFmt
{
    const Eigen::IOFormat
        python_style(Eigen::FullPrecision, 0, ", ", ";\n", "[", "]", "[", "]"),
        csv_style(Eigen::FullPrecision, 0, ", ", "\n", "", "", "", "");
}

/**
 * @brief Print eigen matrix in specified format with message
 *
 * @tparam em
 * @param m
 * @param msg
 * @param fmt
 * @param os
 */
template <detail::EigenIO::EigenMatrix em>
void PrintEigen(
    const em &m,
    const std::string msg,
    const Eigen::IOFormat &fmt = EigenFmt::python_style,
    std::ostream &os = std::cout)
{
    os << msg << "\n"
       << m.format(fmt) << std::endl;
}

AGTB_IO_END

#endif