#ifndef UNICODE_H
#define UNICODE_H
#include "types.h"
#include "config.h"

unint read_grapheme(char* buf, unint len, uint32_t out[MAX_GRAPHEME_SIZE]);

#endif