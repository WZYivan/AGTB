#ifndef __AGTB_APP_CONSOLE_TRAVERSE_HPP__
#define __AGTB_APP_CONSOLE_TRAVERSE_HPP__

#include "Base.hpp"

#if (AGTB_APP)

#include "../../Adjustment/Traverse.hpp"
#include "../../IO/JSON.hpp"
#include "../../IO/Adjustment/Traverse.hpp"
#include "../../Container/PropertyTree.hpp"

AGTB_DEF_CONSOLE_APP_BEGIN(Traverse)

static int Main(int argc, char **argv)
{
    po::options_description options("Options");
    options.add_options()(
        "help,H", "Usage")(
        "input,I", po::value<std::string>(), "Input files")(
        "distance-place,D", po::value<int>()->default_value(3), "Place of distance")(
        "angle-place,A", po::value<int>()->default_value(0), "Place of angle")(
        "route-type,R", po::value<std::string>(),
        "Route type of param. \'cl\' for \'ClosedLoop\', \'cc\' for \'ClosedConnecting\'.")(
        "multi-param,M", "Input file contains multi param.");

    po::positional_options_description positional;
    positional.add("input", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(options).positional(positional).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

#define AGTB_APP_CONSOLE_TRAVERSE_HANDLE_ROUTE_TYPE(__rt, __fname)             \
    PropTree json{};                                                           \
    IO::ReadJson(__fname, json);                                               \
    using Target = Adjustment::TraverseParam<__rt>;                            \
    Target param = IO::ParseJson<Target>(json);                                \
    Adjustment::TraverseAdjustResult result = Adjustment::Adjust(param, 3, 0); \
    std::println("{}", Adjustment::AdjustmentTable(param, result));

#define AGTB_APP_CONSOLE_TRAVERSE_HANDLE_ROUTE_TYPE_MULTI(__rt, __fname)           \
    PropTree json{};                                                               \
    IO::ReadJson(__fname, json);                                                   \
    using Target = Adjustment::TraverseParam<__rt>;                                \
    for (const auto &[k, sub] : PTree::MapView(json))                              \
    {                                                                              \
        Target param = IO::ParseJson<Target>(sub);                                 \
        Adjustment::TraverseAdjustResult result = Adjustment::Adjust(param, 3, 0); \
        std::println("{}:\n{}", k, Adjustment::AdjustmentTable(param, result));    \
    }

    if (vm.count("input"))
    {
        if (!vm.count("route-type"))
        {
            std::println("{} is necessary", "-R");
            return EXIT_FAILURE;
        }
        std::string fname{vm["input"].as<std::string>()};
        std::string route_type{vm["route-type"].as<std::string>()};

        if (route_type == "cl")
        {
            if (!vm.count("multi-param"))
            {
#if (AGTB_DEBUG)
                std::println("{}", "into single");
#endif
                AGTB_APP_CONSOLE_TRAVERSE_HANDLE_ROUTE_TYPE(Adjustment::RouteType::ClosedLoop, fname);
            }
            else
            {
#if (AGTB_DEBUG)
                std::println("{}", "into multi");
#endif
                AGTB_APP_CONSOLE_TRAVERSE_HANDLE_ROUTE_TYPE_MULTI(Adjustment::RouteType::ClosedLoop, fname);
            }
            return EXIT_SUCCESS;
        }
        else if (route_type == "cc")
        {
#if (false) || (true)
            if (!vm.count("multi-param"))
            {
                AGTB_APP_CONSOLE_TRAVERSE_HANDLE_ROUTE_TYPE(Adjustment::RouteType::ClosedConnecting, fname);
            }
            else
            {
                AGTB_APP_CONSOLE_TRAVERSE_HANDLE_ROUTE_TYPE_MULTI(Adjustment::RouteType::ClosedConnecting, fname);
            }
#endif
            return EXIT_SUCCESS;
        }
        else
        {
            std::println("Unknown route type `{}`", route_type);
            return EXIT_FAILURE;
        }
    }

    std::println("{}", "Get no options, you can use -H for help");
    return EXIT_SUCCESS;
}

#define AGTB_CONSOLE_APP_TRAVERSE AGTB_IMPORT_CONSOLE_APP(Traverse)

AGTB_DEF_CONSOLE_APP_END()

#else
#define AGTB_CONSOLE_APP_TRAVERSE void(0)
#endif

#endif