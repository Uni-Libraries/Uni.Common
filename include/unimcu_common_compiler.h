#pragma once

//
// UNIMCU_COMPILER_WEAK
//

#if defined(__GNUC__)
    #define UNIMCU_COMPILER_WEAK __attribute__((weak))
#else
    #define UNIMCU_COMPILER_WEAK
#endif



//
// UNIMCU_COMPILER_UNUSED_VAR
//

#if defined(__GNUC__)
    #define UNIMCU_COMPILER_UNUSED_VAR(x) x __attribute__((__unused__))
#else
    #define UNIMCU_COMPILER_UNUSED_VAR(x) x
#endif



//
// UNIMCU_COMPILER_UNUSED_FUNC
//

#if defined(__GNUC__)
    #define UNIMCU_COMPILER_UNUSED_FUNC(x) __attribute__((__unused__)) UNIMCU_COMPILER_UNUSED_FUNC_##x
#else
    #define UNIMCU_COMPILER_UNUSED_FUNC(x) UNIMCU_COMPILER_UNUSED_FUNC_##x
#endif



//
// UNIMCU_COMPILER_OPTIMIZATION
//

#if defined(__clang__)
    #define UNIMCU_COMPILER_OPTIMIZATION(x)
#elif defined(__GNUC__)
    #define UNIMCU_COMPILER_OPTIMIZATION(x) __attribute__((optimize(x)))
#else
    #define UNIMCU_COMPILER_OPTIMIZATION(x)
#endif



//
// UNIMCU_COMPILER_STATIC_ASSERT
//

#include <assert.h>
#define UNIMCU_COMPILER_STATIC_ASSERT(a, b) static_assert(a, b)



//
// UNIMCU_COMPILER_ALIGN
//

#if defined(__GNUC__)
#define UNIMCU_COMPILER_ALIGN(x) __attribute__((aligned(x)))
#else
#define UNIMCU_COMPILER_ALIGN(x)
#endif



//
// UNIMCU_COMPILER_ALIGN_PACK
//

#if defined(__GNUC__)
    #define UNIMCU_COMPILER_ALIGN_PACK(x) __attribute__((packed, aligned(x)))
#else
    #define UNIMCU_COMPILER_ALIGN_PACK(x)
#endif


//
// UNIMCU_COMPILER_INLINE_ALWAYS
//

#if defined(__GNUC__)
#define UNIMCU_COMPILER_INLINE_ALWAYS __attribute__((always_inline)) static inline
#else
#define UNIMCU_COMPILER_INLINE_ALWAYS static inline
#endif
