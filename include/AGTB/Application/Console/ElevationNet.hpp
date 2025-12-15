#ifndef __AGTB_APP_CONSOLE_FRAMEWORK_HPP__
#define __AGTB_APP_CONSOLE_FRAMEWORK_HPP__

#include "../../details/Macros.hpp"
#include "../../Adjustment/ElevationNet.hpp"
#include <boost/program_options.hpp>
#include <print>
#include <filesystem>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

AGTB_APP_CONSOLE_BEGIN

namespace detail::ElevationNet
{
    namespace po = boost::program_options;
    namespace fs = std::filesystem;
    namespace pt = boost::property_tree;

    struct ElevationNet
    {
    public:
        using options = po::options_description;
        using positional = po::positional_options_description;
        using parser = po::command_line_parser;
        using var_map = po::variables_map;
        using Net = Adjustment::ElevationNet;

    private:
        static inline void DeclareOptions(options &opts)
        {
            opts.add_options()(
                "help,h", "print usage")(
                "input,I", po::value<std::string>(), "input file");
        }

        static void inline DecalrePositinal(positional &posi)
        {
            posi.add("input", -1);
        }

        static void inline BuildMap(var_map &vm, int argc, char **argv, const options &opts, const positional &posi)
        {
            po::store(po::command_line_parser(argc, argv).options(opts).positional(posi).run(), vm);
            po::notify(vm);
        }

        static inline void InitOverEnterMainLoop()
        {
            std::println("> {}", "Init over, into main loop");
        }

        template <class __func>
        static inline bool HandleArgOrCheckArgExists(var_map &vm, auto &&arg, __func handle)
            requires std::invocable<__func, decltype(arg)> &&
                     std::convertible_to<std::remove_cvref_t<decltype(arg)>, var_map::key_type>
        {
            if (vm.count(arg))
            {
                handle(arg);
                return true;
            }
            return false;
        }

        static inline void ReadJsonAndAdjust(Net &net, const std::string &fname)
        {
            pt::ptree root;
            pt::read_json(fname, root);

            double unit_p = root.get<double>("unit_p");

            for (const auto &kv : root.get_child("vertices"))
            {
                const pt::ptree &child = kv.second;
                std::string name = child.get<std::string>("name");
                double elev = child.get<double>("elev");
                net.AddVertex(name, {elev, true});
            }

            for (const auto &kv : root.get_child("edges"))
            {
                const pt::ptree &child = kv.second;
                std::string
                    name = child.get<std::string>("name"),
                    from = child.get<std::string>("from"),
                    to = child.get<std::string>("to");
                double
                    dif = child.get<double>("dif"),
                    len = child.get<double>("len");
                net.AddEdge(true)(from, to, name, {dif, len});
            }

            Adjustment::Adjust(net, unit_p);
        }

    public:
        static int Main(int argc, char **argv)
        {
            bool exit = false;

            options opts("Options");
            DeclareOptions(opts);

            positional posi;
            DecalrePositinal(posi);

            var_map vm;
            BuildMap(vm, argc, argv, opts, posi);

            InitOverEnterMainLoop();

            HandleArgOrCheckArgExists(vm, "help",
                                      [&opts, &exit](const auto &arg)
                                      {
                                          std::cout << opts << std::endl;
                                          exit = true;
                                      });
            if (exit)
            {
                return EXIT_SUCCESS;
            }

            std::string fname;
            bool get_file = false;
            exit = !HandleArgOrCheckArgExists(vm, "input",
                                              [&vm, &fname, &get_file](auto arg)
                                              {
                                                  fname = vm["input"].as<std::string>();
                                                  if (!fs::exists(fname))
                                                  {
                                                      std::println("> File `{}` not exists", fname);
                                                      get_file = false;
                                                  }
                                                  else
                                                  {
                                                      std::println("> Get file `{}`", fname);
                                                      get_file = true;
                                                  }
                                              });
            if (!get_file)
            {
                return EXIT_FAILURE;
            }

            if (exit)
            {
                std::println("> {}", "You must give a .json file");
                return EXIT_FAILURE;
            }

            Net net{};
            ReadJsonAndAdjust(net, fname);
            Adjustment::PrintElevationNet(net);

            return EXIT_SUCCESS;
        }
    };
}

using detail::ElevationNet::ElevationNet;

AGTB_APP_CONSOLE_END

#endif