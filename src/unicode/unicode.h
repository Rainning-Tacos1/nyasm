#ifndef _UNICODE_H
#define _UNICODE_H


#define UNICODE_RETURN(uc, stat) \
    do {                         \
        (uc)->err = stat;        \
        return (stat != UNICODE_OK); \
    } while (0)

#endif