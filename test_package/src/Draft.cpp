#include <AGTB/Utils/DataFrame.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <AGTB/IO/CSV.hpp>
#include <AGTB/Utils/Timer.hpp>
#include <print>

int main()
{
    using AGTB::DataFrame;
    using AGTB::Idx;
    using AGTB::IO::PrintEigen;
    using AGTB::IO::ReadCSV;

    DataFrame<double> df(3, 4);
    auto nf = df.NumericFrame<double>();
    nf.col(0).fill(1.0);
    nf.col(1).fill(2.0);
    nf.col(2) = nf.col(0) + nf.col(1);
    PrintEigen(nf, "numeric frame:");

    AGTB::timer.Tik();
    std::println("{}", df.ToString());
    auto str_df =
        df.Cast<std::string>([](const double &v)
                             { return std::format("val: {}", v); });
    std::println("{}", str_df.ToString());
    AGTB::timer.Tok();

    auto csv = ReadCSV<double>("../dat/csv/Draft.csv", ",", true);
    csv.ColKeys(1) = "123";
    std::println("{}", csv.ToString());

    auto block = df.ILoc(Idx(0) >> 3, Idx(1) >> 4);

    // bool is_block_empty = (block.shape()[0] == 0) || (block.shape()[1] == 0);

    // if (!is_block_empty)
    // {
    //     // 1. 创建一个新的 multi_array 并从视图复制数据
    //     typename decltype(df)::frame_type copied_block_data(boost::extents[block.shape()[0]][block.shape()[1]]);
    //     copied_block_data = block; // 这会触发数据复制

    //     // 2. 使用复制的数据调用 BuildFrom
    //     auto block_df = decltype(df)::BuildFrom(std::move(copied_block_data));
    //     std::println("{}", block_df.ToString());
    // }
    // else
    // {
    //     std::println("Block is empty, cannot create DataFrame from it.");
    // }

    auto block_df = decltype(df)::BuildFrom(block);

    std::println("{}", block_df.ToString());
    std::println("{}", df.ToString());
}