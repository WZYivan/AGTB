#ifdef AGTB_ENABLE_DEBUG
#include <print>
#include <iostream>
#include <cassert>
#include "../IO.hpp"
#define AGTB_DEBUG true
#else
#define AGTB_DEBUG false
#endif

#ifdef AGTB_DEBUG_INFO_VERBOSE_ALL
#define AGTB_DEBUG_INFO_LEVEL 100
#else
#define AGTB_DEBUG_INFO_LEVEL 0
#endif

#define AGTB_DEBUG_INTERNAL_LEVEL 1

#ifdef AGTB_DISABLE_NOTE
#define AGTB_NOTE false
#else
#define AGTB_NOTE true
#endif

#ifdef AGTB_ENABLE_EXP
#define AGTB_EXP true
#else
#define AGTB_EXP false
#endif