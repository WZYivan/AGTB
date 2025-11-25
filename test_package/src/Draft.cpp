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

    auto block = df.ILoc(Idx(0) >> 2, 0);
    // std::println("{}", block.ToString());
}