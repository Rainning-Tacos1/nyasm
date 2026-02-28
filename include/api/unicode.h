#ifndef API_UNICODE_H
#define API_UNICODE_H

// API exposed by the implementation
#include "../../src/unicode/api.h"

#define READ_GRAPHEME(uc, nread) read_grapheme(uc, nread)
#define UNICODE_INIT(uc) unicode_init(uc)


#endif