#ifndef __AGTB_APP_CONSOLE_BASA_HPP__
#define __AGTB_APP_CONSOLE_BASA_HPP__

#include "../../details/Macros.hpp"
#if (AGTB_APP)

#include <boost/program_options.hpp>

AGTB_APP_CONSOLE_BEGIN

namespace detail
{
    namespace po = boost::program_options;

    template <typename T>
    concept ImplConsoleAppInterface = requires(T app) {
        { app.Run() } -> std::convertible_to<int>;
        { app.Help() } -> std::convertible_to<std::string>;
        { T::ParseTerminalArgs(int(0), (char **)(nullptr)) } -> std::same_as<T>;
    };

    class ConsoleApp
    {
    private:
        po::options_description options;
        po::variables_map vm;
        po::positional_options_description positional;
        int argc;
        char **argv;

    public:
        ConsoleApp(int _argc, char **_argv) : argc(_argc), argv(_argv)
        {
            options.add_options()("help,h", "Get help");
        }
        virtual ~ConsoleApp() = default;

        std::string Help() const noexcept
        {
            std::ostringstream oss;
            oss << options;
            return oss.str();
        }

        int Run()
        {
            po::store(po::command_line_parser(argc, argv).options(options).positional(positional).run(), vm);
            po::notify(vm);
        }
    };

    template <ImplConsoleAppInterface __app>
    int RunConsoleApp(int argc, char **argv)
    {
        return __app::ParseTerminalArgs(argc, argv).Run();
    }
}

AGTB_APP_CONSOLE_END

#define AGTB_USE_CONSOLE_APP(__app)                                    \
    int main(int argc, char **argv)                                    \
    {                                                                  \
        return ::AGTB::Application::Console::__app ::Main(argc, argv); \
    }

#define AGTB_DEF_CONSOLE_APP_BEGIN(__app) \
    AGTB_APP_CONSOLE_BEGIN                \
    namespace detail::__app               \
    {                                     \
        struct __app                      \
        {
#define AGTB_DEF_CONSOLE_APP_END() \
    }                              \
    ;                              \
    }                              \
    AGTB_APP_CONSOLE_END

#define AGTB_IMPORT_CONSOLE_APP(__app) detail::__app::__app

#else

#if (AGTB_NOTE)
#warning "You include <AGTB/Application/Console/*> but not #define AGTB_ENABLE_APP, it will not work"
#endif

#include <print>
#define AGTB_USE_CONSOLE_APP(__app)                                                                                        \
    int main(int argc, char **argv)                                                                                        \
    {                                                                                                                      \
        std::println("{}", "You include <AGTB/Application/Console/*> but not #define AGTB_ENABLE_APP, it will not work."); \
        return EXIT_FAILURE;                                                                                               \
    }

#endif

#endif