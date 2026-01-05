#define AGTB_ENABLE_DEBUG

#include <AGTB/IO.hpp>
#include <print>
namespace aio = AGTB::IO;

int main()
{
    aio::Json json{aio::ReadJson("../dat/json/elevation_net.json")};

    if (json.HasArray("edges"))
    {
        for (const auto &v : json.ArrayView("edges"))
        {
            std::println("name: {}", v.Value<std::string>("name"));
        }
    }

    std::println("unit_p: {}", json.Value<double>("unit_p", -1));

    std::println("json: {}", json.ToString());
}