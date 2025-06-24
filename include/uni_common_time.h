#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

    //
    // Includes
    //

    #include <stdbool.h>
    #include <stdint.h>
    #include <time.h>

    #include "uni_common_compiler.h"


    //
    // Checks
    //
    UNI_COMMON_COMPILER_STATIC_ASSERT(sizeof(time_t) == 8U, "time is not 8-bytes");


    //
    // Functiosn
    //

    /**
     * Converts tm struct to unix epoch time
     * @param tm pointer to the tm structure
     * @param unixtime pointer to unixtime upto 64-bit
     * @return true on success
     */
    bool uni_common_time_tm_to_unix(struct tm *tm, time_t *unixtime);


    /**
     * Converts unix epoch time to tm struct
     * @param unixtime pointer to unixtime upto 64-bit
     * @param tm pointer to the allocated tm structure
     * @return true on success
     *
     * @warning GCC bug: gcc corrupts R2 content with the 64-bit unixtime, so use pointer to the 64-bit value
     */
    bool uni_common_time_unix_to_tm(const time_t *unixtime, struct tm *tm);


    /**
     * Unpack time_t to two uint32_t values
     * @param in pointer to the time_t value to unpack
     * @param out_high pointer to the most significant bytes output
     * @param out_low pointer to the least significant bytes output
     * @return true on success
     */
    bool uni_common_time_unpack(time_t *in, uint32_t *out_high, uint32_t *out_low);


    #if defined(__cplusplus)
}
#endif
