#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

//
// Includes
//

// stdlib
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// uni_common
#include "uni_common_compiler.h"



//
// Functions
//

/**
 * Get count if 1 bits in variable
 * @param val variable to check
 * @return count of 1 bits in val
 */
UNI_COMMON_COMPILER_INLINE_ALWAYS uint32_t uni_common_bytes_popcount32(uint32_t val) {
#if defined(_MSC_VER)
    return __popcnt(val);
#else
    return __builtin_popcount(val);
#endif
}


/**
 * Find subarray in array
 * @param big big array pointer
 * @param big_len big array size in bytes
 * @param little little array pointer
 * @param little_len little array size in bytes
 * @return offset from big pointer in case little was found, SIZE_MAX if little was not found
 */
size_t uni_common_bytes_search(const void* big, size_t big_len, const void* little, size_t little_len);


/**
 * Swap bytes in uint16_t variable
 * @param val uint16_t value to swap bytes
 * @return result with swapped bytes
 */
UNI_COMMON_COMPILER_INLINE_ALWAYS uint16_t uni_common_bytes_swap16(uint16_t val) {
#if defined(_MSC_VER)
    return _byteswap_ushort(val);
#else
    return __builtin_bswap16(val);
#endif
}

/**
 * Reverse bytes in uint32_t values
 * @param val uint32_t vlaue to reverse
 * @return result with reversed bytes
 */
UNI_COMMON_COMPILER_INLINE_ALWAYS uint32_t uni_common_bytes_swap32(uint32_t val) {
#if defined(_MSC_VER)
    return _byteswap_ulong(val);
#else
    return __builtin_bswap32(val);
#endif
}


/**
 * Unpack uint64_t to two uint32_t values
 * @param in pointer to the uint64_t value to unpack
 * @param out_high pointer to the most significant bytes output
 * @param out_low pointer to the least significant bytes output
 * @return true on success
 */
bool uni_common_bytes_unpack64(const uint64_t *in, uint32_t *out_high, uint32_t *out_low);


#if defined(__cplusplus)
}
#endif
