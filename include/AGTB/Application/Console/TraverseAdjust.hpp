#ifndef __AGTB_APP_CONSOLE_TRAVERSE_ADJUST_HPP__
#define __AGTB_APP_CONSOLE_TRAVERSE_ADJUST_HPP__

#include "../../details/Macros.hpp"
#include "../../Adjustment/Traverse.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>

#include <string>
#include <filesystem>

AGTB_APP_CONSOLE_BEGIN

namespace detail::TraverseAdjust
{

    namespace po = boost::program_options;
    namespace pt = boost::property_tree;
    namespace fs = std::filesystem;

    struct TraverseAdjust
    {
    public:
        static int Main(int argc, char **argv)
        {
            po::options_description options("Options");
            options.add_options()(
                "help,h", "Usage")(
                "input,I", po::value<std::string>(), "Input file")(
                "distance,D", po::value<int>()->default_value(3), "Place of distance")(
                "angle,A", po::value<int>()->default_value(0), "Place of angle")(
                "example,E", "Give a example input file");

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

            if (vm.count("example"))
            {
                std::string example{
                    R"(
{
    "distances" : [105.22, 80.18, 129.34, 78.16],
    "angles" : [
        [107, 48, 32], 
        [73, 0, 24], 
        [89, 33, 48], 
        [89, 36, 30]
    ],
    "azi_beg" : [125, 30, 0],
    "x_beg" : 506.32,
    "y_beg" : 215.65
}
)"};
                std::println("{}", example);
                return EXIT_SUCCESS;
            }

            if (vm.count("input"))
            {
                std::string fname{vm["input"].as<std::string>()};

                if (!fs::exists(fname))
                {
                    std::println("File `{}` not exists", fname);
                    return EXIT_FAILURE;
                }

                pt::ptree root;
                pt::read_json(fname, root);

                auto distances = root.get_child("distances");
                auto angles = root.get_child("angles");
                auto azi_beg = root.get_child("azi_beg");
                double x_beg = root.get<double>("x_beg"), y_beg = root.get<double>("y_beg");

                Adjustment::TraverseParam<Adjustment::RouteType::ClosedLoop> param{};

                for (const auto &kv : distances)
                {
                    const pt::ptree &child = kv.second;
                    param.distances.push_back(child.get_value<double>());
                }

                for (const auto &kv : angles)
                {
                    const pt::ptree &child = kv.second;
                    auto it = child.begin();

                    double d, m, s;
                    d = it->second.get_value<double>();
                    ++it;
                    m = it->second.get_value<double>();
                    ++it;
                    s = it->second.get_value<double>();

                    param.angles.push_back(Angle(d, m, s));
                }

                const pt::ptree &azi_beg_array = root.get_child("azi_beg");
                auto it = azi_beg_array.begin();

                double azi_degrees = it->second.get_value<double>();
                ++it;
                double azi_minutes = it->second.get_value<double>();
                ++it;
                double azi_seconds = it->second.get_value<double>();

                param.azi_beg = Angle(azi_degrees, azi_minutes, azi_seconds);
                param.x_beg = x_beg;
                param.y_beg = y_beg;

                int pd = vm["distance"].as<int>(), pa = vm["angle"].as<int>();

                auto result = Adjustment::Adjust(param, pd, pa);

                std::println("{}", Adjustment::AdjustmentTable(param, result));

                return EXIT_SUCCESS;
            }

            std::println("{}", "No options get");
            return EXIT_FAILURE;
        }
    };
}

using detail::TraverseAdjust::TraverseAdjust;

AGTB_APP_CONSOLE_END

#endif