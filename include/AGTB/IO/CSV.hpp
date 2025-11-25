#ifndef __AGTB_IO_CSV_HPP__
#define __AGTB_IO_CSV_HPP__

#include "../details/Macros.hpp"
#include "../Utils/DataFrame.hpp"
#include "../Utils/CharConv.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/algorithm/string.hpp> // For split, trim

AGTB_IO_BEGIN

/**
 * @brief Read Csv file to a `DataFrame`
 *
 * @tparam __value_type
 * @tparam __row_key_type
 * @tparam __col_key_type
 * @param fname
 * @param separator
 * @param has_header
 * @return DataFrame<__value_type, __row_key_type, __col_key_type>
 */
template <typename __value_type, typename __row_key_type = std::string, typename __col_key_type = std::string>
DataFrame<__value_type, __row_key_type, __col_key_type> ReadCSV(
    const std::string &fname,
    std::string separator = ",",
    bool has_header = true)
{
    using DataFrameType = DataFrame<__value_type, __row_key_type, __col_key_type>;
    using frame_type = typename DataFrameType::frame_type;
    using row_keys_type = typename DataFrameType::row_keys_type;
    using col_keys_type = typename DataFrameType::col_keys_type;

    boost::iostreams::stream<boost::iostreams::file_source> file_stream(fname, std::ios_base::in | std::ios_base::binary);
    if (!file_stream)
    {
        AGTB_THROW(std::runtime_error, std::format("Cannot open file: {}", fname));
    }

    std::string line;
    std::vector<std::string> headers;
    std::vector<std::vector<__value_type>> data_rows;
    std::vector<__row_key_type> row_names;

    size_t line_number = 0;
    while (std::getline(file_stream, line))
    {
        line_number++;
        std::vector<std::string> fields;
        boost::split(fields, line, boost::is_any_of(separator), boost::token_compress_on);

        if (has_header && headers.empty())
        {
            headers.reserve(fields.size());
            for (auto &field : fields)
            {
                boost::trim(field);
                headers.push_back(field);
            }
            continue;
        }

        if constexpr (std::same_as<__value_type, std::string>)
        {
            data_rows.push_back(std::move(fields));
            row_names.emplace_back(std::to_string(data_rows.size() - 1));
        }
        else if (fields.size() > 0)
        {
            std::vector<__value_type> row_data;
            row_data.reserve(fields.size());
            for (auto &field : fields)
            {
                boost::trim(field);
                __value_type converted_value;
                if (!FromString<__value_type>(field, converted_value))
                {
                    AGTB_THROW(std::invalid_argument,
                               std::format("Cannot convert field '{}' at line {} to type", field, line_number));
                }
                row_data.push_back(converted_value);
            }
            data_rows.push_back(std::move(row_data));
            row_names.emplace_back(std::to_string(data_rows.size() - 1));
        }
    }

    if (data_rows.empty())
    {
        return DataFrameType();
    }

    size_t num_rows = data_rows.size();
    size_t num_cols = has_header ? headers.size() : data_rows[0].size();

    DataFrameType df(num_rows, num_cols);

    if (has_header)
    {
        if (headers.size() != num_cols)
        {
            AGTB_THROW(std::runtime_error,
                       std::format("Number of header fields ({}) does not match number of data columns ({})",
                                   headers.size(), num_cols));
        }
        for (size_t i = 0; i < num_cols; ++i)
        {
            df.ColKeys(i) = static_cast<__col_key_type>(headers[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < num_cols; ++i)
        {
            df.ColKeys(i) = "Col" + std::to_string(i);
        }
    }

    for (size_t i = 0; i < num_rows; ++i)
    {
        df.RowKeys(i) = row_names[i];
    }

    for (size_t r = 0; r < num_rows; ++r)
    {
        const auto &data_row = data_rows[r];
        size_t cols_in_row = data_row.size();
        for (size_t c = 0; c < num_cols; ++c)
        {
            if (c < cols_in_row)
            {
                df.ILoc(r, c) = data_row[c];
            }
            else
            {
                df.ILoc(r, c) = __value_type{};
            }
        }
    }

    return df;
}

AGTB_IO_END

#endif