#ifndef _UNICODE_H
#define _UNICODE_H

#include "types.h"

#define IS_VALID_CP(c) ((c) <= 0x10FFFF)

#define UNICODE_SUCCESS(uc)       \
    do {                          \
        (uc)->err = UNICODE_OK;   \
        return SUCCESS;           \
    } while (0)

#define UNICODE_FAIL(uc, _err) \
    do {                       \
        (uc)->nread = 0;       \
        (uc)->cp = EOF;      \
        (uc)->err = _err;      \
        return FAIL;           \
    } while (0)

#endif