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

class DataFrameIndicesBuilder
{
public:
    using index = boost::detail::multi_array::index;
    using size_type = boost::detail::multi_array::size_type;
    using index_range = boost::detail::multi_array::index_range<index, size_type>;

private:
    size_t beg;

public:
    ~DataFrameIndicesBuilder() = default;
    DataFrameIndicesBuilder(size_t _beg) : beg(_beg) {}

    class WithEnd
    {
    private:
        const DataFrameIndicesBuilder &super;
        size_t end;

    public:
        ~WithEnd() = default;
        WithEnd(DataFrameIndicesBuilder &_ref, size_t _end) : super(_ref), end(_end)
        {
        }

        operator index_range()
        {
            return index_range(super.beg, end);
        }

        class WithStride
        {
        private:
            const WithEnd &super;
            size_t stride;

        public:
            ~WithStride() = default;
            WithStride(WithEnd &_ref, size_t _stride) : super(_ref), stride(_stride)
            {
            }

            operator index_range()
            {
                return index_range(super.super.beg, super.end, stride);
            }

            decltype(auto) operator||(index_range other)
            {
                return boost::indices[index_range(*this)][other];
            }

            decltype(auto) operator|(const DataFrameIndicesBuilder &other)
            {
                return boost::indices[*this][other.beg];
            }
        };

        WithStride operator<<(size_t _stride)
        {
            return WithStride(*this, _stride);
        }

        decltype(auto) operator||(index_range other)
        {
            return boost::indices[index_range(*this)][other];
        }

        decltype(auto) operator|(const DataFrameIndicesBuilder &other)
        {
            return boost::indices[*this][other.beg];
        }
    };

    WithEnd operator>>(size_t _end)
    {
        return WithEnd(*this, _end);
    }

    operator index_range()
    {
        return index_range(beg, beg + 1);
    }

    decltype(auto) operator||(index_range other)
    {
        return boost::indices[index_range(*this)][other];
    }

    decltype(auto) operator|(const DataFrameIndicesBuilder &other)
    {
        return boost::indices[beg][other.beg];
    }

    decltype(auto) operator|(index_range other)
    {
        return boost::indices[beg][other];
    }
};

DataFrameIndicesBuilder Idx(size_t _beg)
{
    return DataFrameIndicesBuilder(_beg);
}

template <typename __key_type>
class DataFrameKeyIndexRange
{
public:
    using key_type = __key_type;
    key_type beg, end;
    size_t stride;

    DataFrameKeyIndexRange(key_type _beg, key_type _end)
        : DataFrameKeyIndexRange(_beg, _end, 1)
    {
    }

    DataFrameKeyIndexRange(key_type _beg, key_type _end, size_t _stride)
        : beg(_beg), end(_end), stride(_stride)
    {
    }

    ~DataFrameKeyIndexRange() = default;
};

template <typename __key_type>
class DataFrameKeyIndicesBuilder
{
public:
    using key_type = __key_type;
    using index_range = DataFrameKeyIndexRange<key_type>;

private:
    key_type beg;

public:
    ~DataFrameKeyIndicesBuilder() = default;
    DataFrameKeyIndicesBuilder(key_type _beg) : beg(_beg)
    {
    }

    class WithEnd
    {
    private:
        const DataFrameKeyIndicesBuilder &super;
        key_type end;

    public:
        ~WithEnd() = default;
        WithEnd(DataFrameKeyIndicesBuilder &_ref, key_type _end) : super(_ref), end(_end)
        {
        }

        class WithStride
        {
        private:
            const WithEnd &super;
            size_t stride;

        public:
            ~WithStride() = default;
            WithStride(WithEnd &_ref, size_t _stride) : super(_ref), stride(_stride)
            {
            }

            operator index_range()
            {
                return index_range(super.super.beg, super.end, stride);
            }
        };

        WithStride operator|(size_t _stride)
        {
            return WithStride(*this, _stride);
        }

        operator index_range()
        {
            return index_range(super.beg, end);
        }
    };

    WithEnd operator>>(key_type _end)
    {
        return WithEnd(*this, _end);
    }

    operator index_range()
    {
        return index_range(beg, beg + 1);
    }
};

template <typename __key_type>
DataFrameKeyIndicesBuilder<__key_type> Key(__key_type _beg)
{
    return DataFrameKeyIndicesBuilder<__key_type>(_beg);
}

template <
    typename __value_type = std::string,
    typename __row_key_type = std::string,
    typename __col_key_type = std::string>
class DataFrameView;

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

    using data_frame_view_type = DataFrameView<value_type, row_key_type, col_key_type>;

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
    using row_keys_view_type = typename row_keys_type::array_view<1>;
    using col_keys_view_type = typename col_keys_type::array_view<1>;

    template <typename T>
    using numeric_frame_of = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    template <typename T>
    using numeric_series_of = Eigen::Vector<T, Eigen::Dynamic>;

    using index = boost::detail::multi_array::index;
    using size_type = boost::detail::multi_array::size_type;
    using index_range = boost::detail::multi_array::index_range<index, size_type>;

    using row_key_index_range = DataFrameKeyIndexRange<row_key_type>;
    using col_key_index_range = DataFrameKeyIndexRange<col_key_type>;

private:
    frame_type frame;
    row_keys_type row_keys;
    col_keys_type col_keys;

public:
    template <typename __self>
    auto &&Unwrap(this __self &&self)
    {
        return self.frame;
    }

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

    data_frame_view_type View()
    {
        return data_frame_view_type(*this);
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

    static DataFrame BuildFrom(frame_type &&data)
    {
        using target = DataFrame;

        size_t rows = data.shape()[0];
        size_t cols = data.shape()[1];

        target result(rows, cols);
        result.frame = std::move(data);

        return result;
    }

    std::string ToString() const
    {
        std::string str{};

        str.append(std::format("\t{}\n", boost::join(col_keys, " \t")));
        for (size_t i = 0; i != Rows(); i++)
        {
            str.append(std::format("{}: ", row_keys[i]));
            for (size_t c = 0; c != Cols(); ++c)
            {
                str.append(
                    c != Cols() - 1 ? std::format("{}, ", frame[i][c]) : std::format("{}", frame[i][c]));
            }
            str.append("\n");
        }

        return str;
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, size_t col_idx)
    {
        if (col_idx >= self.Cols())
        {
            AGTB_THROW(std::out_of_range, "Column index out of bounds");
        }
        auto col_indices =
            boost::multi_array_types::index_range(col_idx, col_idx + 1);
        auto row_indices =
            boost::multi_array_types::index_range(0, self.Rows());
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, size_t row_idx, size_t col_idx)
    {
        if (col_idx >= self.Cols() || row_idx >= self.Rows())
        {
            AGTB_THROW(std::out_of_range, "Index out of bounds");
        }
        return self.frame[row_idx][col_idx];
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, const index_range &row_indices, size_t col_idx)
    {
        if (col_idx >= self.Cols())
        {
            AGTB_THROW(std::out_of_range, "Column index out of bounds");
        }
        auto col_indices =
            boost::multi_array_types::index_range(col_idx, col_idx + 1);
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, size_t row_idx, const index_range &col_indices)
    {
        if (row_idx >= self.Rows())
        {
            AGTB_THROW(std::out_of_range, "Row index out of bounds");
        }
        auto row_indices =
            boost::multi_array_types::index_range(row_idx, row_idx + 1);
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, const index_range &row_indices, const index_range &col_indices)
    {
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self, typename __col_like>
        requires std::convertible_to<__col_like, col_key_type>
    decltype(auto) Loc(this __self &&self, __col_like &&ck)
    {
        size_t col_idx = __FindIdxOf(self.col_keys, ck);
        return self.ILoc(col_idx);
    }

    template <typename __self, typename __row_like, typename __col_like>
        requires std::convertible_to<__row_like, row_key_type> && std::convertible_to<__col_like, col_key_type>
    decltype(auto) Loc(this __self &&self, __row_like &&rk, __col_like &&ck)
    {
        size_t col_idx = __FindIdxOf(self.col_keys, ck);
        size_t row_idx = __FindIdxOf(self.row_keys, rk);

        return self.ILoc(row_idx, col_idx);
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
            return Eigen::Map<numeric_frame_of<pure_cast>>(frame.data(), Rows(), Cols());
        }
        else if constexpr (std::convertible_to<value_type, std::string>)
        {
            size_t rows = Rows(), cols = Cols();
            numeric_frame result_matrix(rows, cols);

            for (size_t r = 0; r < rows; ++r)
            {
                for (size_t c = 0; c < cols; ++c)
                {
                    if (!FromString<pure_cast>(this->ILoc(r, c), result_matrix(r, c)))
                    {
                        AGTB_THROW(std::invalid_argument, std::format("Convert value {} fail", this->ILoc(r, c)));
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

    template <typename __self>
    data_frame_view_type __ViewOf(this __self &&self, const index_range &row_indices, const index_range &col_indices)
    {
        return data_frame_view_type(
            self.frame[boost::indices[row_indices][col_indices]],
            self.col_keys[boost::indices[col_indices]],
            self.row_keys[boost::indices[row_indices]]);
    }

    template <typename __self>
    index_range __ColKeyRangeToIndexRange(this __self &&self, const col_key_index_range &ckir)
    {
        col_key_type beg = ckir.beg, end = ckir.end;
        size_type stride = ckir.stride;
        size_type i_beg = __FindIdxOf(self.col_keys, beg);
        size_type i_end = __FindIdxOf(self.col_keys, end);
        i_end = (i_beg == i_end) ? i_beg + 1 : i_end + 1;
        return index_range(i_beg, i_end, stride);
    }
};

template <
    typename __value_type,
    typename __row_key_type,
    typename __col_key_type>
class DataFrameView
{
public:
    using value_type = __value_type;
    using row_key_type = __row_key_type;
    using col_key_type = __col_key_type;

    template <typename T>
    using frame_view_of = typename boost::multi_array<T, 2>::array_view<2>::type;
    using frame_view_type = frame_view_of<value_type>;

    template <typename T>
    using series_view_of = typename boost::multi_array<T, 1>::array_view<1>::type;
    using col_keys_view_type = series_view_of<col_key_type>;
    using row_keys_view_type = series_view_of<row_key_type>;

    using data_frame_type = DataFrame<value_type, row_key_type, col_key_type>;

    using index = boost::detail::multi_array::index;
    using size_type = boost::detail::multi_array::size_type;
    using index_range = boost::detail::multi_array::index_range<index, size_type>;

    template <typename T>
    using numeric_frame_of = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

private:
    frame_view_type frame_view;
    col_keys_view_type col_keys_view;
    row_keys_view_type row_keys_view;

public:
    ~DataFrameView() = default;
    DataFrameView(frame_view_type _frame_view, col_keys_view_type _col_keys_view, row_keys_view_type _row_keys_view)
        : frame_view(_frame_view), col_keys_view(_col_keys_view), row_keys_view(_row_keys_view) {};
    DataFrameView(data_frame_type &_df)
        : DataFrameView(
              _df.ILoc(Idx(0) >> _df.Rows(), Idx(0) >> _df.Cols()),
              _df.ColKeys()[boost::indices[boost::multi_array_types::index_range(0, _df.Cols())]],
              _df.RowKeys()[boost::indices[boost::multi_array_types::index_range(0, _df.Rows())]])
    {
    }

    std::string ToString() const
    {
        std::string str{};

        str.append(std::format("\t{}\n", boost::join(col_keys_view, " \t")));
        for (size_t i = 0; i < row_keys_view.size(); i++)
        {
            str.append(std::format("{}: ", row_keys_view[i]));
            for (size_t c = 0; c != col_keys_view.size(); ++c)
            {
                str.append(
                    c != col_keys_view.size() - 1 ? std::format("{}, ", frame_view[i][c]) : std::format("{}", frame_view[i][c]));
            }
            str.append("\n");
        }

        return str;
    }

    size_t Rows() const
    {
        return Shape()[0];
    }

    size_t Cols() const
    {
        return Shape()[1];
    }

    decltype(auto) Shape() const
    {
        return frame_view.shape();
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, size_t col_idx)
    {
        if (col_idx >= self.Cols())
        {
            AGTB_THROW(std::out_of_range, "Column index out of bounds");
        }
        auto col_indices =
            boost::multi_array_types::index_range(col_idx, col_idx + 1);
        auto row_indices =
            boost::multi_array_types::index_range(0, Rows());
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, size_t row_idx, size_t col_idx)
    {
        if (col_idx >= self.Cols() || row_idx >= self.Rows())
        {
            AGTB_THROW(std::out_of_range, "Index out of bounds");
        }
        return self.frame[row_idx][col_idx];
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, const index_range &row_indices, size_t col_idx)
    {
        if (col_idx >= self.Cols())
        {
            AGTB_THROW(std::out_of_range, "Column index out of bounds");
        }
        auto col_indices =
            boost::multi_array_types::index_range(col_idx, col_idx + 1);
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, size_t row_idx, const index_range &col_indices)
    {
        if (row_idx >= self.Rows())
        {
            AGTB_THROW(std::out_of_range, "Row index out of bounds");
        }
        auto row_indices =
            boost::multi_array_types::index_range(row_idx, row_idx + 1);
        return self.__ViewOf(row_indices, col_indices);
    }

    template <typename __self>
    decltype(auto) ILoc(this __self &&self, const index_range &row_indices, const index_range &col_indices)
    {
        return self.__ViewOf(row_indices, col_indices);
    }

    /**
     * @brief To `DataFrameView`, its memory layout is non-continous, and `Eigen::Map` can't handle that. So, this always returns a copy.
     *
     * @tparam __cast_type
     * @return decltype(auto)
     */
    template <typename __cast_type>
    decltype(auto) NumericFrame()
    {
        using pure_cast = std::remove_cvref_t<__cast_type>;
        using numeric_frame = numeric_frame_of<pure_cast>;

        size_t rows = this->Rows();
        size_t cols = this->Cols();
        numeric_frame result_matrix(rows, cols);

        for (size_t r = 0; r < rows; ++r)
        {
            for (size_t c = 0; c < cols; ++c)
            {
                if constexpr (std::convertible_to<value_type, std::string>)
                {
                    if (!FromString<pure_cast>(frame_view[r][c], result_matrix(r, c)))
                    {
                        AGTB_THROW(std::invalid_argument, std::format("Convert value {} fail", frame_view[r][c]));
                    }
                }
                else
                {
                    result_matrix(r, c) = static_cast<pure_cast>(frame_view[r][c]);
                }
            }
        }
        return result_matrix;
    }

    template <typename __self>
    decltype(auto) UnWrap(this __self &&self)
    {
        return self.frame_view;
    }

    data_frame_type Eval() const
    {
        return data_frame_type(
            frame_view,
            row_keys_view,
            col_keys_view);
    }

private:
    template <typename __self>
    DataFrameView __ViewOf(this __self &&self, const index_range &row_indices, const index_range &col_indices)
    {
        return DataFrameView(
            self.frame_view[boost::indices[row_indices][col_indices]],
            self.col_keys_view[boost::indices[col_indices]],
            self.row_keys_view[boost::indices[row_indices]]);
    }
};

AGTB_UTILS_END

AGTB_BEGIN

using Utils::DataFrame;
using Utils::Idx;
using Utils::Key;

AGTB_END

#endif