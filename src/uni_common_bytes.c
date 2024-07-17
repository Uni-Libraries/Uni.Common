//
// Includes
//

// stdlib
#include <stdlib.h>
#include <string.h>

// compiler
#if defined(_MSC_VER)
#include <intrin.h>
#endif

// unimcu common
#include "unimcu_common_bytes.h"


//
// Functions
//

size_t unimcu_bytes_search(const void* big, size_t big_len, const void* little, size_t little_len) {
    size_t result = SIZE_MAX;
    if (big != NULL && little != NULL && big_len >= little_len) {
        for (size_t big_off = 0U; big_off <= big_len - little_len; big_off++) {
            if (memcmp((const uint8_t *)big + big_off, little, little_len) == 0) {
                result = big_off;
                break;
            }
        }
    }
    return result;
}


bool unimcu_bytes_unpack64(const uint64_t *in, uint32_t *out_high, uint32_t *out_low) {
    bool result = false;

    if (in != NULL) {
        if (out_high != NULL) {
            *out_high = (uint32_t)((*in & 0xFFFFFFFF00000000ULL) >> 32);
        }

        if (out_low != NULL) {
            *out_low = (uint32_t)(*in & 0xFFFFFFFFU);
        }

        result = true;
    }

    return result;
}
