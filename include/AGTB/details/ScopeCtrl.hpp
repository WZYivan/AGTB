#define AGTB_BEGIN \
    namespace AGTB \
    {
#define AGTB_END }

#define AGTB_PHOTOGRAMMETRY_BEGIN \
    AGTB_BEGIN                    \
    namespace Photogrammetry      \
    {

#define AGTB_PHOTOGRAMMETRY_END \
    }                           \
    AGTB_END

#define AGTB_GEODESY_BEGIN \
    AGTB_BEGIN             \
    namespace Geodesy      \
    {

#define AGTB_GEODESY_END \
    }                    \
    AGTB_END

#define AGTB_APP_BEGIN    \
    AGTB_BEGIN            \
    namespace Application \
    {

#define AGTB_APP_END \
    }                \
    AGTB_END

#define AGTB_APP_CONSOLE_BEGIN \
    AGTB_APP_BEGIN             \
    namespace Console          \
    {

#define AGTB_APP_CONSOLE_END \
    }                        \
    AGTB_APP_END

#define AGTB_UTILS_BEGIN \
    AGTB_BEGIN           \
    namespace Utils      \
    {

#define AGTB_UTILS_END \
    }                  \
    AGTB_END

#define AGTB_ADJUSTMENT_BEGIN \
    AGTB_BEGIN                \
    namespace Adjustment      \
    {

#define AGTB_ADJUSTMENT_END \
    }                       \
    AGTB_END

#define AGTB_LINALG_BEGIN \
    AGTB_BEGIN            \
    namespace Linalg      \
    {

#define AGTB_LINALG_END \
    }                   \
    AGTB_END

#define AGTB_IO_BEGIN \
    AGTB_BEGIN        \
    namespace IO      \
    {

#define AGTB_IO_END \
    }               \
    AGTB_END

#define AGTB_CONTAINER_BEGIN \
    AGTB_BEGIN               \
    namespace Container      \
    {

#define AGTB_CONTAINER_END \
    }                      \
    AGTB_END