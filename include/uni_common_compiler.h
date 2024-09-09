#pragma once

//
// UNI_COMMON_COMPILER_WEAK
//

#if defined(__GNUC__)
    #define UNI_COMMON_COMPILER_WEAK __attribute__((weak))
#else
    #define UNI_COMMON_COMPILER_WEAK
#endif



//
// UNI_COMMON_COMPILER_UNUSED_VAR
//

#if defined(__GNUC__)
    #define UNI_COMMON_COMPILER_UNUSED_VAR(x) x __attribute__((__unused__))
#else
    #define UNI_COMMON_COMPILER_UNUSED_VAR(x) x
#endif



//
// UNI_COMMON_COMPILER_UNUSED_FUNC
//

#if defined(__GNUC__)
    #define UNI_COMMON_COMPILER_UNUSED_FUNC(x) __attribute__((__unused__)) UNI_COMMON_COMPILER_UNUSED_FUNC_##x
#else
    #define UNI_COMMON_COMPILER_UNUSED_FUNC(x) UNI_COMMON_COMPILER_UNUSED_FUNC_##x
#endif



//
// UNI_COMMON_COMPILER_OPTIMIZATION
//

#if defined(__clang__)
    #define UNI_COMMON_COMPILER_OPTIMIZATION(x)
#elif defined(__GNUC__)
    #define UNI_COMMON_COMPILER_OPTIMIZATION(x) __attribute__((optimize(x)))
#else
    #define UNI_COMMON_COMPILER_OPTIMIZATION(x)
#endif



//
// UNI_COMMON_COMPILER_STATIC_ASSERT
//

#include <assert.h>
#define UNI_COMMON_COMPILER_STATIC_ASSERT(a, b) static_assert(a, b)



//
// UNI_COMMON_COMPILER_ALIGN
//

#if defined(__GNUC__)
#define UNI_COMMON_COMPILER_ALIGN(x) __attribute__((aligned(x)))
#else
#define UNI_COMMON_COMPILER_ALIGN(x)
#endif



//
// UNI_COMMON_COMPILER_ALIGN_PACK
//

#if defined(__GNUC__)
    #define UNI_COMMON_COMPILER_ALIGN_PACK(x) __attribute__((packed, aligned(x)))
#else
    #define UNI_COMMON_COMPILER_ALIGN_PACK(x)
#endif


//
// UNI_COMMON_COMPILER_INLINE_ALWAYS
//

#if defined(__GNUC__)
#define UNI_COMMON_COMPILER_INLINE_ALWAYS __attribute__((always_inline)) static inline
#else
#define UNI_COMMON_COMPILER_INLINE_ALWAYS static inline
#endif
