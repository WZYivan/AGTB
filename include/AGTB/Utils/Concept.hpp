#ifndef __AGTB_UTILS_CONCEPT_HPP__
#define __AGTB_UTILS_CONCEPT_HPP__

#include "../details/Macros.hpp"

#include <type_traits>

AGTB_BEGIN

template <typename T, typename... Tp>
concept IsOneOf = (std::is_same_v<T, Tp> || ...);

template <typename T>
struct Unqualified
{
    using Type = std::remove_cvref_t<T>;
};

template <typename T>
using UnqualifiedType = Unqualified<T>::Type;

template <typename __to_be_qualified, typename __like_this>
struct ConstLike
{
    using Type = std::conditional_t<
        std::is_const_v<__like_this>,
        std::add_const_t<__to_be_qualified>,
        __to_be_qualified>;
};

template <typename __to_be_qualified, typename __like_this>
using ConstLikeType = ConstLike<__to_be_qualified, __like_this>::Type;

template <typename __container>
concept StdContainerLike = requires {
    typename __container::value_type;
};

template <typename T>
concept HasEqual = requires(T l, T r) {
    { l == r } -> std::same_as<bool>;
};

template <typename T>
concept HasGtAndLt = requires(T l, T r) {
    { l > r } -> std::same_as<bool>;
    { l < r } -> std::same_as<bool>;
};

template <typename __base, typename __derived>
concept IsBaseOf = std::is_base_of_v<__base, __derived>;

template <typename __derived, typename __base>
concept IsDerivedFrom = IsBaseOf<__base, __derived>;

//
// Pascal Casing Begin
//

#define AGTB_DEF_REQUIRE_TYPE_NAME(__type_name) \
    template <typename T>                       \
    concept __type_name = requires {            \
        typename T ::__type_name;               \
    };

#define AGTB_HAS_PASCAL_CASING_TYPE_NAME_BEGIN \
    namespace HasPascalCasingTypeName          \
    {
#define AGTB_HAS_PASCAL_CASING_TYPE_NAME_END }
#define AGTB_DEF_REQUIRE_PASCAL_CASING_TYPE_NAME(__type_name) \
    AGTB_HAS_PASCAL_CASING_TYPE_NAME_BEGIN                    \
    AGTB_DEF_REQUIRE_TYPE_NAME(__type_name)                   \
    AGTB_HAS_PASCAL_CASING_TYPE_NAME_END
#define AGTB_HAS_PASCAL_CASING_TYPE_NAME(__T, __type_name) \
    HasPascalCasingTypeName::__type_name<__T>

//
// Snake Casing Begin
//

#define AGTB_HAS_SNAKE_CASING_TYPE_NAME_BEGIN \
    namespace has_snake_casing_type_name      \
    {
#define AGTB_HAS_SNAKE_CASING_TYPE_NAME_END }
#define AGTB_DEF_REQUIRE_SNAKE_CASING_TYPE_NAME(__type_name) \
    AGTB_HAS_SNAKE_CASING_TYPE_NAME_BEGIN                    \
    AGTB_DEF_REQUIRE_TYPE_NAME(__type_name)                  \
    AGTB_HAS_SNAKE_CASING_TYPE_NAME_END
#define AGTB_HAS_SNAKE_CASING_TYPE_NAME(__T, __type_name) \
    has_snake_casing_type_name::__type_name<__T>

//
// Has Type Name Begin
//

#define AGTB_HAS_TYPE_NAME_BEGIN \
    namespace HasTypeName        \
    {
#define AGTB_HAS_TYPE_NAME_END }
#define AGTB_DEF_REQUIRE_HAS_TYPE_NAME(__pascal_name, __snake_name) \
    AGTB_HAS_TYPE_NAME_BEGIN                                        \
    template <typename T>                                           \
    concept __pascal_name =                                         \
        AGTB_HAS_PASCAL_CASING_TYPE_NAME(T, __pascal_name) ||       \
        AGTB_HAS_SNAKE_CASING_TYPE_NAME(T, __snake_name);           \
    AGTB_HAS_TYPE_NAME_END

//
// Extract Type Name Begin
//

#define AGTB_EXTRACT_BEGIN \
    namespace Extract      \
    {
#define AGTB_EXTRACT_END }

#define AGTB_EXTRACT_TYPE_NAME(__T, __name) \
    using TypeName = typename __T ::__name

#define AGTB_DEF_EXTRACT_DECLARE(__pascal_name, __snake_name)                                          \
    template <typename T>                                                                              \
    struct __pascal_name                                                                               \
    {                                                                                                  \
        AGTB_STATIC_THROW(std::format("T has no type named {} or {}", #__pascal_name, #__snake_name)); \
    };
// template <HasTypeName::__pascal_name T>     \


#define AGTB_DEF_EXTRACT_PASCAL(__pascal_name, __snake_name)   \
    template <HasPascalCasingTypeName::__pascal_name T>        \
        requires(!has_snake_casing_type_name::__snake_name<T>) \
    struct __pascal_name<T>                                    \
    {                                                          \
        AGTB_EXTRACT_TYPE_NAME(T, __pascal_name);              \
    }
#define AGTB_DEF_EXTRACT_SNAKE(__pascal_name, __snake_name)  \
    template <has_snake_casing_type_name::__snake_name T>    \
        requires(!HasPascalCasingTypeName::__pascal_name<T>) \
    struct __pascal_name<T>                                  \
    {                                                        \
        AGTB_EXTRACT_TYPE_NAME(T, __snake_name);             \
    };

#define AGTB_DEF_EXTRACT_ALL_BUT_PASCAL(__pascal_name, __snake_name) \
    template <HasPascalCasingTypeName::__pascal_name T>              \
        requires has_snake_casing_type_name::__snake_name<T>         \
    struct __pascal_name<T>                                          \
    {                                                                \
        AGTB_EXTRACT_TYPE_NAME(T, __pascal_name);                    \
    };

#define AGTB_DEF_EXTARCT(__pascal_name, __snake_name)             \
    AGTB_EXTRACT_BEGIN                                            \
    AGTB_DEF_EXTRACT_DECLARE(__pascal_name, __snake_name);        \
    AGTB_DEF_EXTRACT_PASCAL(__pascal_name, __snake_name);         \
    AGTB_DEF_EXTRACT_SNAKE(__pascal_name, __snake_name);          \
    AGTB_DEF_EXTRACT_ALL_BUT_PASCAL(__pascal_name, __snake_name); \
    AGTB_EXTRACT_END
//
// Extract Alias Begin
//

#define AGTB_EXTRACT_TYPE_NAME_BEGIN \
    namespace ExtractTypeName        \
    {
#define AGTB_EXTRACT_TYPE_NAME_END }
#define AGTB_DEF_EXTRACT_TYPE_NAME(__pascal_name)              \
    AGTB_EXTRACT_TYPE_NAME_BEGIN                               \
    template <HasTypeName::__pascal_name T>                    \
    using __pascal_name = Extract::__pascal_name<T>::TypeName; \
    AGTB_EXTRACT_TYPE_NAME_END

//
// Tool Chain Begin
//

/**
 * @brief `PSCTM` means `Pascal and Snake Casing Typename Management` which is designed to manage those typenames
 * with same meaning but different casing style. For example, `ValueType` and `value_type` are in same
 * meaning but different casing style. Such marcro define a toolchain consists of struct , concept and
 * alias in namespace `has_snake_casing_type_name`, `HasPascalCasingTypeName`, `HasTypeName`, `Extract`
 * and `ExtractTypeName`. To use it, give it `PascalName` and `snake_name`, and use `HasTypeName::PascalName<T>`
 * to constraints that `T` must have `PascalName` or `snake_name` and use `ExtractTypeName::PascalName<T>`
 * to extract `PascalName` or `snake_name` (default to be `PascalName`) of `T`.
 *
 */
#define AGTB_DEF_PSCTM_TOOLCHAIN(__pascal_name, __snake_name)    \
    AGTB_DEF_REQUIRE_PASCAL_CASING_TYPE_NAME(__pascal_name);     \
    AGTB_DEF_REQUIRE_SNAKE_CASING_TYPE_NAME(__snake_name);       \
    AGTB_DEF_REQUIRE_HAS_TYPE_NAME(__pascal_name, __snake_name); \
    AGTB_DEF_EXTARCT(__pascal_name, __snake_name);               \
    AGTB_DEF_EXTRACT_TYPE_NAME(__pascal_name);

//
// Macro Define End
//

AGTB_DEF_PSCTM_TOOLCHAIN(ValueType, value_type);
AGTB_DEF_PSCTM_TOOLCHAIN(KeyType, key_type);
AGTB_DEF_PSCTM_TOOLCHAIN(SizeType, size_type);

AGTB_END

#endif