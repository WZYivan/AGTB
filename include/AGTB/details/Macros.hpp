#pragma once

#ifndef __AGTB_DETAILS_MACROS_HPP__
#define __AGTB_DETAILS_MACROS_HPP__

#ifndef __GNUC__
#warning "This is a GNU C++ 23 lib"
#endif

#include "ScopeCtrl.hpp"
#include "Throw.hpp"
#include "Options.hpp"

#ifdef AGTB_ENABLE_STUPID_MODULE
#define AGTB_LOCALE_PRIVATE_NS detail
#define AGTB_PRIVATE namespace AGTB_LOCALE_PRIVATE_NS ::
#define AGTB_FROM_IMPORT(__src, __what) using __src ::__what
#define AGTB_FROM_IMPORT_AS(__src, __what, __alias) using __alias = __src ::__what
#define AGTB_FROM_PRIVATE_IMPORT(__src, __what) AGTB_FROM_IMPORT(AGTB_LOCALE_PRIVATE_NS ::__src, __what)
#define AGTB_FROM_PRIVATE_IMPORT_AS(__src, __what, __alias) AGTB_FROM_IMPORT_AS(AGTB_LOCALE_PRIVATE_NS ::__src, __what, __alias)
#define AGTB_IMPORT_AS(__src, __alias) namespace __alias = __src;
#define AGTB_IMPORT_PRIVATE_AS(__src, __alias) AGTB_IMPORT_AS(AGTB_LOCALE_PRIVATE_NS ::__src, __alias)
#endif

#define AGTB_WEAK_REQUIRE_MEMBER_TYPE(__Tp, __M, __RTp) {__Tp::__M}->std::convertible_to<__RTp>
#define AGTB_T_HAS_TYPED_MEMBER(__M, __RTp) AGTB_WEAK_REQUIRE_MEMBER_TYPE(T, __M, __RTp)
#define AGTB_WEAK_REQUIRE_MEMBER_RETURN_TYPE(__Tp, __M, __MCp, __RTp) {__Tp::__M(__MCp)}->std::convertible_to<__RTp>
#define AGTB_T_HAS_MEMBER_RETURN(__M, __MCp, __RTp) AGTB_WEAK_REQUIRE_MEMBER_RETURN_TYPE(T, __M, __MCp, __RTp)

#endif