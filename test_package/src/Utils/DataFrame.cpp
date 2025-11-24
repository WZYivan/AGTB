#include <AGTB/Utils/DataFrame.hpp>
#include <AGTB/IO/Eigen.hpp>
#include <AGTB/IO/CSV.hpp>
#include <print>

int main()
{
    using AGTB::IO::PrintEigen;
    using AGTB::IO::ReadCSV;
    using AGTB::Utils::DataFrame;

    DataFrame<double> df(3, 4);
    auto nf = df.NumericFrame<double>();
    nf.col(0).fill(1.0);
    nf.col(1).fill(2.0);
    nf.col(2) = nf.col(0) + nf.col(1);
    PrintEigen(nf, "numeric frame:");

    std::println("{}", df.ToString());
    auto str_df =
        df.Cast<std::string>([](const double &v)
                             { return std::format("val: {}", v); });
    std::println("{}", str_df.ToString());

    auto csv = ReadCSV<double>("../dat/csv/Draft.csv", ",", true);
    csv.ColKeys(1) = "123";
    std::println("{}", csv.ToString());
}