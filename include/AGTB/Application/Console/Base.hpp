#ifndef __AGTB_APP_CONSOLE_BASA_HPP__
#define __AGTB_APP_CONSOLE_BASA_HPP__

#include "../../details/Macros.hpp"
#if (AGTB_APP)

#include <boost/program_options.hpp>

AGTB_APP_CONSOLE_BEGIN

namespace detail
{
    namespace po = boost::program_options;
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