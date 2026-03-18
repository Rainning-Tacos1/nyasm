#ifndef CTYPE_H
#define CTYPE_H

#include "types.h"

#define CHARMASK(c) ((unsigned char)((c) & 0xff))

#define CTF_LOWER  0x01
#define CTF_UPPER  0x02
#define CTF_ALPHA  (CTF_LOWER | CTF_UPPER)
#define CTF_DIGIT  0x04
#define CTF_ALNUM  (CTF_ALPHA | CTF_DIGIT)
#define CTF_SPACE  0x08
#define CTF_XDIGIT 0x10

#define CTF_ODIGIT 0x20

#define MAX_CTF_TYPE_VAL (CTF_LOWER | CTF_UPPER | CTF_DIGIT | CTF_SPACE | CTF_XDIGIT | CTF_ODIGIT)

extern const unint __ctype_table[256];

/* Unlike their C counterparts, the following macros are not meant to
 * handle an int with any of the values [EOF, 0-UCHAR_MAX]. The argument
 * must be a signed/unsigned char. */
#define ISASCII(c) (c < 0x80)

#define ISLOWER(c)  (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_LOWER))
#define ISUPPER(c)  (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_UPPER))
#define ISALPHA(c)  (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_ALPHA))
#define ISDIGIT(c)  (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_DIGIT))
#define ISXDIGIT(c) (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_XDIGIT))
#define ISALNUM(c)  (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_ALNUM))
#define ISSPACE(c)  (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_SPACE))

#define ISODIGIT(c) (ISASCII(c) && (__ctype_table[CHARMASK(c)] & CTF_ODIGIT))

extern const unsigned char __ctype_tolower[256];
extern const unsigned char __ctype_toupper[256];

#define TOLOWER(c) (__ctype_tolower[CHARMASK(c)])
#define TOUPPER(c) (__ctype_toupper[CHARMASK(c)])

#endif