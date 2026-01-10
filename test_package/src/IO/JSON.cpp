#define AGTB_ENABLE_DEBUG
// #define AGTB_ENABLE_EXP

#include <AGTB/AGTB.hpp>
#include <print>

namespace aio = AGTB::IO;
namespace ac = AGTB::Container;

int main()
{
    ac::PropTree ptree;
    aio::ReadJson("../dat/json/traverse_adjust.json", ptree);

    ac::PTree::ValidateValue<double>(ptree, "x_beg");
    std::println("x_beg = {}", ac::PTree::Value<double>(ptree, "x_beg"));

    assert(ac::PTree::HasArray(ptree, "distances"));

    for (const auto &v : ac::PTree::ArrayView(ptree, "distances"))
    {
        std::println("v = {}", ac::PTree::Value<double>(v));
    }

    for (const auto &v : ac::PTree::ArrayTo<std::vector<double>>(ptree, "distances"))
    {
        std::println("v = {}", v);
    }

    for (const auto &[k, v] : ac::PTree::MapView(ptree))
    {
        std::println("key = {}", k);
    }
}