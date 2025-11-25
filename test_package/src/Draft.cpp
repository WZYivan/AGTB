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
    std::println("Raw Df:\n{}", df.ToString());
    auto str_df =
        df.Cast<std::string>([](const double &v)
                             { return std::format("val: {}", v); });
    std::println("Casted Df:\n{}", str_df.ToString());
    AGTB::timer.Tok();

    auto csv = ReadCSV<std::string>("../dat/csv/Draft.csv", ",", true);
    csv.ColKeys(1) = "123";

    std::println("Readed Csv:\n{}", csv.ToString());
    auto name = csv.Loc("Name");
    std::println("Name:\n{}", name.ToString());

    // csv.RowKeys() = name.UnWrap()[boost::indices[Idx(0) >> 2][0]];
    csv.RowKeys() = name.UnWrap()[Idx(0) >> 2 | Idx(0)];
    std::println("Named Readed Csv:\n{}", csv.ToString());

    auto data_block =
        csv.ILoc(
               Idx(0) >> csv.Rows(),
               Idx(1) >> csv.Cols())
            .Eval()
            .Cast<double>(
                [](const std::string &str) -> double
                {
                    double v;
                    std::string val{str};
                    boost::trim(val);
                    AGTB::Utils::FromString<double>(val, v);
                    return v;
                });
    std::println("Data Block of csv:\n{}", data_block.ToString());

    auto block_df =
        df.ILoc(
            Idx(0) >> 3 << 2,
            Idx(1) >> 4);

    std::println("Block of Df:\n{}", block_df.ToString());

    auto blk2 =
        block_df.ILoc(
            Idx(0) >> 2,
            Idx(0) >> 2);
    std::println("Block of Block:\n{}", blk2.ToString());

    auto nf_blk2 = blk2.NumericFrame<double>();
    PrintEigen(nf_blk2, "Numeric Frame of a block of block");
}