#ifndef _UNICODE_H
#define _UNICODE_H


#define UNICODE_SUCCESS(stmt) \
    do {           \
        stmt;      \
        return 1;  \
    } while (0)

#define UNICODE_FAIL(err, stmt) \
    do {           \
        stmt;      \
        return err;  \
    } while (0)


#endif