
#include "config.h"
#include "types.h"

#include "api/unicode.h"
#include "api/log.h"
#include "api/memory.h"
#include "api/debug.h"

#include "../parser/api.h"

nbool assemble(char* file, unint len) {
    int32_t cps[MAX_GRAPHEME_SIZE];

    char* _file = file;
    char* end = file+len;
    unint nread = 0;

    unint indent = 0;
    unint indstack[MAXINDENT] = {0};
    unint altindstack[MAXINDENT] = {0};

    do {
        nread = parse_indent(&_file, end, (int32_t*)&cps, &indent, indstack, altindstack);
        if(nread == -1) break;
        if(nread == -2) { LOG("CodePointError\n"); return FAIL;}
        if(nread == -3) { LOG("GraphemeError\n"); return FAIL;}
        if(nread == -4) { LOG("NormalizationError\n"); return FAIL;}
        if(nread == -5) { LOG("CantStoreGrapheme\n"); return FAIL;}
        DBG("----------------------------------------------------------------------------------------------------------INDENTATION: %d\n", nread);
        nread = skip_until_next_line(&_file, end, (int32_t*)&cps);
        if(nread == -2) { LOG("CodePointError\n"); return FAIL;}
        if(nread == -3) { LOG("GraphemeError\n"); return FAIL;}
        if(nread == -4) { LOG("NormalizationError\n"); return FAIL;}
        if(nread == -5) { LOG("CantStoreGrapheme\n"); return FAIL;}
        DBG("----------------------------------------------------------------------------------------------------------SKIPING: %d\n", nread);
    } while(nread != -1);

    return SUCCESS;
}