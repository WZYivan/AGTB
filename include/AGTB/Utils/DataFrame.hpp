#ifndef __AGTB_UTILS_DATAFRAME_HPP__
#define __AGTB_UTILS_DATAFRAME_HPP__

#include "../details/Macros.hpp"

#include <string>
#include <format>

#include <Eigen/Dense>

#include <boost/multi_array.hpp>
#include <boost/algorithm/string.hpp>

AGTB_UTILS_BEGIN

template <typename __new_value_type, typename __value_type, typename __converter>
concept __DataFrameCastConverterConcept = requires(__converter convert, __value_type this_value) {
    std::invocable<__converter>;
    { convert(this_value) } -> std::convertible_to<__new_value_type>;
};

/**
 * @brief A simple implement of `pandas.DataFrame`, but only store same type
 *
 * @tparam __value_type
 * @tparam __row_key_type
 * @tparam __col_key_type
 */
template <
    typename __value_type = std::string,
    typename __row_key_type = std::string,
    typename __col_key_type = std::string>
class DataFrame
{
public:
    using value_type = __value_type;
    using row_key_type = __row_key_type;
    using col_key_type = __col_key_type;

    template <typename T>
    using frame_of = boost::multi_array<T, 2>;
    using frame_type = frame_of<value_type>;
    template <typename T>
    using series_of = boost::multi_array<T, 1>;
    using series_type = series_of<value_type>;

    using frame_view_type = typename frame_type::array_view<2>;
    using series_view_type = typename series_type::array_view<1>;

    using row_keys_type = series_of<row_key_type>;
    using col_keys_type = series_of<col_key_type>;

    template <typename T>
    using numeric_frame_of = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    template <typename T>
    using numeric_series_of = Eigen::Vector<T, Eigen::Dynamic>;

private:
    frame_type frame;
    row_keys_type row_keys;
    col_keys_type col_keys;

public:
    frame_type &inner_frame = frame;

    ~DataFrame() = default;
    DataFrame() = default;

    DataFrame(size_t rows, size_t cols)
    {
        frame.resize(boost::extents[rows][cols]);
        row_keys.resize(boost::extents[rows]);
        col_keys.resize(boost::extents[cols]);
    }

    DataFrame(frame_type &&frame_arg, row_keys_type &&row_keys_arg, col_keys_type &&col_keys_arg)
        : frame(frame_arg), row_keys(row_keys_arg), col_keys(col_keys_arg)
    {
    }

    /**
     * @brief Build dataframe from another with differenet type using converter
     *
     * @tparam __other_value_type
     * @param other_frame
     * @param other_row_keys
     * @param other_col_keys
     * @param converter
     * @return DataFrame
     */
    template <typename __other_value_type>
    static DataFrame BuildFrom(
        const typename DataFrame<__other_value_type, row_key_type, col_key_type>::frame_type &other_frame,
        const series_of<row_key_type> &other_row_keys,
        const series_of<col_key_type> &other_col_keys,
        auto converter)
    {
        using target = DataFrame;

        size_t rows = other_frame.shape()[0];
        size_t cols = other_frame.shape()[1];

        typename target::frame_type frame{};
        frame.resize(boost::extents[rows][cols]);

        typename target::row_keys_type row_keys{};
        row_keys.resize(boost::extents[rows]);

        typename target::col_keys_type col_keys{};
        col_keys.resize(boost::extents[cols]);

        std::copy(other_row_keys.origin(), other_row_keys.origin() + other_row_keys.num_elements(), row_keys.origin());
        std::copy(other_col_keys.origin(), other_col_keys.origin() + other_col_keys.num_elements(), col_keys.origin());

        // row_keys_map = other_row_keys_map;
        // col_keys_map = other_col_keys_map;

        std::transform(
            other_frame.data(),
            other_frame.data() + other_frame.num_elements(),
            frame.data(),
            converter);
        return target(
            std::move(frame),
            std::move(row_keys),
            std::move(col_keys));
    }

    std::string ToString() const
    {
        std::string str{};

        str.append(std::format("\t{}\n", boost::join(col_keys, " \t")));
        for (size_t i = 0; i < row_keys.size(); i++)
        {
            str.append(std::format("{}: ", row_keys[i]));
            for (size_t c = 0; c != col_keys.size(); ++c)
            {
                str.append(
                    c != col_keys.size() - 1 ? std::format("{}, ", frame[i][c]) : std::format("{}", frame[i][c]));
            }
            str.append("\n");
        }

        return str;
    }

    /**
     * @brief Access by keys
     *
     * @tparam __self
     * @param self
     * @param col_idx
     * @return decltype(auto)
     */
    template <typename __self>
    decltype(auto) IdxLocate(this __self &&self, size_t col_idx)
    {
        if (col_idx >= self.frame.shape()[1])
        {
            AGTB_THROW(std::out_of_range, "Column index out of bounds");
        }
        return self.frame[boost::indices[boost::multi_array_types::index_range(
            0,
            self.frame.shape()[0])]
                                        [static_cast<frame_type::index>(col_idx)]];
    }

    /**
     * @brief Access by index
     *
     * @tparam __self
     * @param self
     * @param row_idx
     * @param col_idx
     * @return decltype(auto)
     */
    template <typename __self>
    decltype(auto) IdxLocate(this __self &&self, size_t row_idx, size_t col_idx)
    {
        if (col_idx >= self.frame.shape()[1] || row_idx >= self.frame.shape()[0])
        {
            AGTB_THROW(std::out_of_range, "Column index out of bounds");
        }
        return self.frame[row_idx][col_idx];
    }

    /**
     * @brief Access by keys
     *
     * @tparam __self
     * @tparam __col_like
     * @param self
     * @param ck
     * @return requires
     */
    template <typename __self, typename __col_like>
        requires std::convertible_to<__col_like, col_key_type>
    decltype(auto) Locate(this __self &&self, __col_like &&ck)
    {
        size_t col_idx = __FindIdxOf(self.col_keys, ck);
        return self.IdxLocate(col_idx);
    }

    /**
     * @brief Access by keys
     *
     * @tparam __self
     * @tparam __row_like
     * @tparam __col_like
     * @param self
     * @param rk
     * @param ck
     * @return requires&&
     */
    template <typename __self, typename __row_like, typename __col_like>
        requires std::convertible_to<__row_like, row_key_type> && std::convertible_to<__col_like, col_key_type>
    decltype(auto) Locate(this __self &&self, __row_like &&rk, __col_like &&ck)
    {
        size_t col_idx = __FindIdxOf(self.col_keys, ck);
        size_t row_idx = __FindIdxOf(self.row_keys, rk);

        return self.IdxLocate(row_idx, col_idx);
    }

    /**
     * @brief Access keys of column
     *
     * @tparam __self
     * @param self
     * @return auto&&
     */
    template <typename __self>
    auto &&ColKeys(this __self &&self)
    {
        return self.col_keys;
    }

    /**
     * @brief Access specified key of column
     *
     * @tparam __self
     * @param self
     * @param idx
     * @return auto&&
     */
    template <typename __self>
    auto &&ColKeys(this __self &&self, size_t idx)
    {
        if (idx > self.col_keys.size())
        {
            AGTB_THROW(std::out_of_range, std::format("Index {} out of range", idx));
        }
        return self.col_keys[idx];
    }

    /**
     * @brief Access keys of row
     *
     * @tparam __self
     * @param self
     * @return auto&&
     */
    template <typename __self>
    auto &&RowKeys(this __self &&self)
    {
        return self.row_keys;
    }

    /**
     * @brief Access specified key of row
     *
     * @tparam __self
     * @param self
     * @param idx
     * @return auto&&
     */
    template <typename __self>
    auto &&RowKeys(this __self &&self, size_t idx)
    {
        if (idx > self.row_keys.size())
        {
            AGTB_THROW(std::out_of_range, std::format("Index {} out of range", idx));
        }
        return self.row_keys[idx];
    }

    /**
     * @brief Convert to `Eigen::Matrix<>`. If `value_type` is same as target and is numeric type, it returns `Eigen::Map`. All of them are `Eigen::RowMajor`
     *
     * @tparam __cast_type
     * @return decltype(auto)
     */
    template <typename __cast_type>
    decltype(auto) NumericFrame()
    {
        using pure_cast = std::remove_cvref_t<__cast_type>;
        using numeric_frame = numeric_frame_of<pure_cast>;

        if constexpr (
            std::same_as<
                pure_cast,
                std::remove_cvref_t<value_type>> &&
            (std::floating_point<pure_cast> ||
             std::integral<pure_cast>))
        {
            return Eigen::Map<numeric_frame_of<pure_cast>>(frame.data(), frame.shape()[0], frame.shape()[1]);
        }
        else if constexpr (std::convertible_to<value_type, std::string>)
        {
            size_t rows = row_keys.size(), cols = col_keys.size();
            numeric_frame result_matrix(rows, cols);

            for (size_t r = 0; r < rows; ++r)
            {
                for (size_t c = 0; c < cols; ++c)
                {
                    if (!FromString<pure_cast>(this->IdxLocate(r, c), result_matrix(r, c)))
                    {
                        AGTB_THROW(std::invalid_argument, std::format("Convert value {} fail", this->IdxLocate(r, c)));
                    }
                }
            }
            return result_matrix;
        }
        else
        {
            AGTB_STATIC_THROW("Unsupported cast");
        }
    }

    size_t Rows() const noexcept
    {
        return frame.shape()[0];
    }

    size_t Cols() const noexcept
    {
        return frame.shape()[1];
    }

    decltype(auto) Shape() const noexcept
    {
        return frame.shape();
    }

    /**
     * @brief Cast all elements to another type and return a new dataframe
     *
     * @tparam __new_value_type
     * @param convert
     * @return DataFrame<__new_value_type, row_key_type, col_key_type>
     */
    template <typename __new_value_type>
    DataFrame<__new_value_type, row_key_type, col_key_type> Cast(auto &&convert) const noexcept
        requires __DataFrameCastConverterConcept<__new_value_type, value_type, decltype(convert)>
    {
        return DataFrame<__new_value_type, row_key_type, col_key_type>::template BuildFrom<value_type>(
            this->frame,
            this->row_keys,
            this->col_keys,
            std::forward<decltype(convert)>(convert));
    }

private:
    decltype(auto) __GetColSeriesIndicies(size_t ci) const noexcept
    {
        return boost::indices[boost::multi_array_types::index_range(
            0,
            frame.shape()[0])]
                             [ci];
    }

    template <typename __keys, typename __key_type_like>
        requires std::convertible_to<__key_type_like, typename __keys::value_type>
    static size_t __FindIdxOf(const __keys &keys, __key_type_like &&key)
    {
        typename __keys::value_type typed_key = static_cast<typename __keys::value_type>(key);
        auto it = std::find(keys.origin(),
                            keys.origin() + keys.num_elements(),
                            typed_key);
        if (it == keys.origin() + keys.num_elements())
        {
            AGTB_THROW(std::out_of_range, std::format("{}{}", "Column key not found: ", typed_key));
        }
        size_t idx = std::distance(keys.origin(), it);
        return idx;
    }
};

AGTB_UTILS_END

AGTB_BEGIN

using Utils::DataFrame;

AGTB_END

#endif