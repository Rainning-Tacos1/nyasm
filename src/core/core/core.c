
#include "config.h"
#include "types.h"
#include "../unicode/api.h"
#include "api/log.h"

nbool assemble(char* file, unint len) {
    uint32_t cps[MAX_GRAPHEME_SIZE];

    char* _file = file;
    unint nread = 0;

    do {
        nread = read_grapheme(&_file, len, &cps);
        if(nread == -1) { LOG("EncodingError\n"); return FAIL;}
        if(nread == -2) { LOG("NormalizationError\n"); return FAIL;}
    } while(nread != 0);

    return SUCCESS;
}