#include <AGTB/Utils/String.hpp>
#include <print>
#include <iostream>

void P(auto &str)
{
    std::println(std::cout, "[{}]", str);
}

int main()
{
    using namespace AGTB::Utils;

    std::string ori = "  a test str   ";

    P(ori);

    auto ls = LStrip(ori),
         rs = RStrip(ori),
         lrs = LRStrip(ori),
         sws = SkipWhiteSpace(ori);

    P(ls);
    P(rs);
    P(lrs);
    P(sws);

    std::string arr = " 2,  5  , 3, 6";
    auto vec = SplitThenConv<int>(arr, ",");
    for (auto &i : vec)
    {
        std::print("{}, ", i);
    }
    std::cout << std::endl;
}