
#include "config.h"
#include "types.h"
#include "../unicode/api.h"
#include "api/log.h"

nbool assemble(char* file, unint len) {
    int32_t cps[MAX_GRAPHEME_SIZE];

    char* _file = file;
    char* end = file+len;
    unint nread = 0;

    do {
        nread = read_grapheme(&_file, end, (int32_t*)&cps);
        if(nread == -1) { LOG("CodePointError\n"); return FAIL;}
        if(nread == -2) { LOG("GraphemeError\n"); return FAIL;}
        if(nread == -3) { LOG("NormalizationError\n"); return FAIL;}
        if(nread == -4) { LOG("CantStoreGrapheme\n"); return FAIL;}
    } while(nread != 0);

    return SUCCESS;
}