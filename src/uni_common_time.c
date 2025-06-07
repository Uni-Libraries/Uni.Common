//
// Includes
//

#include "uni_common_bytes.h"
#include "uni_common_time.h"


//
// Public
//

bool uni_common_time_tm_to_unix(struct tm *tm, time_t *unixtime) {
    bool result = false;

    if (tm != NULL && unixtime != NULL) {
        #if defined(_MSC_VER) || defined(__MINGW32__)
        *unixtime = _mkgmtime(tm);
        #elif defined(__NEWLIB__)
        *unixtime = mktime(tm);
        #else
        *unixtime = timegm(tm);
        #endif
        result = true;
    }

    return result;
}


bool uni_common_time_unix_to_tm(const time_t *unixtime, struct tm *tm) {
    bool result = false;

    if (unixtime != NULL && tm != NULL) {
        #if defined(_MSC_VER) || defined(__MINGW32__)
        result = gmtime_s(tm, unixtime) == 0;
        #else
        result = gmtime_r(unixtime, tm) != NULL;
        #endif
    }

    return result;
}


bool uni_common_time_unpack(time_t *in, uint32_t *out_high, uint32_t *out_low) {
    return uni_common_bytes_unpack64((uint64_t *)in, out_high, out_low);
}

