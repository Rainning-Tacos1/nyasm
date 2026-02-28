#ifndef UNICODE_H
#define UNICODE_H

#include "types.h"
#include "config.h"

// Errors provided by read_grapheme
#define UNICODE_ERR_CODEPOINT -1
#define UNICODE_ERR_GRAPHEME -2
#define UNICODE_ERR_EOF -3
#define UNICODE_ERR_NORMALIZE -4
#define UNICODE_ERR_TOO_SMALL -5

struct unicode {
    char* buf;
    char* curr;
    char* end;

    int32_t cps[MAX_GRAPHEME_SIZE];
};

unint read_grapheme(struct unicode* uc, unint* nread);

void unicode_init(struct unicode* uc);

#endif