#ifndef __AGTB_APP_CONSOLE_ELEVATION_ADJUST_HPP__
#define __AGTB_APP_CONSOLE_ELEVATION_ADJUST_HPP__

#include "Base.hpp"

#if (AGTB_APP)

#include "../../Adjustment/ElevationNet.hpp"
#include "../../IO/JSON.hpp"
#include "../../IO/Adjustment/ElevationNet.hpp"
#include "../../Container/PropertyTree.hpp"

AGTB_DEF_CONSOLE_APP_BEGIN(ElevationAdjust)

static int Main(int argc, char **argv)
{
    po::options_description options("Options");
    options.add_options()(
        "help,H", "Usage")(
        "input,I", po::value<std::string>(), "Input files")(
        "elev-place,E", po::value<int>()->default_value(3), "Place of elevation")(
        "unit-p,P", po::value<double>()->default_value(1.0), "Unit P")(
        "route-type,R", po::value<std::string>(),
        "Route type of param. \'cl\' for \'ClosedLoop\', \'cc\' for \'ClosedConnecting\', \'c\' for \'Connecting\',\'n\' for \'Net\'.")(
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

    if (vm.count("input"))
    {
        if (!vm.count("route-type"))
        {
            std::println("{} is necessary", "-R or --route-type");
            return EXIT_FAILURE;
        }
        std::string fname{vm["input"].as<std::string>()};
        std::string route_type{vm["route-type"].as<std::string>()};

        if (route_type == "n")
        {
            PropTree json{};
            using Target = Adjustment::ElevationNet;
            IO::ReadJson(fname, json);
            Target net = IO::ParseJson<Target>(json);
            Adjustment::Adjust(net, vm["unit-p"].as<double>());
            Adjustment::PrintElevationNet(net);
            return EXIT_SUCCESS;
        }
#if (AGTB_EXP)
        else if (route_type == "cl")
        {
            return EXIT_SUCCESS;
        }
        else if (route_type == "cc")
        {

            return EXIT_SUCCESS;
        }
#endif
        else
        {
            std::println("Unknown route type `{}`", route_type);
            return EXIT_FAILURE;
        }
    }

    std::println("{}", "Get no options, you can use -H for help");
    return EXIT_SUCCESS;
}

AGTB_DEF_CONSOLE_APP_END()

#define AGTB_CONSOLE_APP_ELEVATION_ADJUST AGTB_IMPORT_CONSOLE_APP(ElevationAdjust)

#else
#define AGTB_CONSOLE_APP_ELEVATION_ADJUST void(0)
#endif

#endif