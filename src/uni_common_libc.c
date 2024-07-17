//
// Includes
//

#if defined(NEWLIB)

// stdlib
#include <errno.h>
#include <unistd.h>
#if !defined(__clang__)
#include <reent.h>
#endif

// unimcu
#include "unimcu_common_libc.h"



//
// Implementation
//

int *__errno(void) {
#if defined(__clang__)
    return NULL;
#else
    return &_REENT->_errno;
#endif
}

int _close(int) {
    errno = ENOSYS;
    return -1;
}

int _kill(int, int) {
    errno = ENOSYS;
    return -1;
}

off_t _lseek(int, off_t, int) {
    errno = ENOSYS;
    return -1;
}

int _getpid() {
    errno = ENOSYS;
    return -1;
}

#endif
