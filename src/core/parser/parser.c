#include "types.h"
#include "api.h"
#include "config.h"

#include "api/unicode.h"
#include "api/debug.h"
/*
  Returns the levels of indentation.
  Returns:
    0 .. n : Number of indentation
*/
nint parse_indent(char** file, char* end, int32_t* cps, unint* indent, unint* indstack, unint* altindstack) {
    
    unint col = 0;
    unint altcol = 0;
    unint nread = 0;
    unint pendin = 0;

    // Thanks python ;)
    for(;;) {
        nread = READ_GRAPHEME(file, end, cps);
        if(nread < 0) return nread; // parsing error;
        if(nread != 1) break; // '\t' ' ' are not 1 cp wide

        if (*cps == (int32_t)' ') col++, altcol++;
        else if(*cps == (int32_t)'\t') {
            col = (col / TABSIZE + 1) * TABSIZE;
            altcol = (altcol / ALTTABSIZE + 1) * ALTTABSIZE;
        // Ignore '\f'
        // Ignore '\\'
        } else break;
    }
    DBG("COL: %d | ALTCOL: %d\n", col, altcol);
    // Ignore comments
    // Ignore '\\'

    // col = cont_line_col ? cont_line_col : col;
    // altcol = cont_line_col ? cont_line_col : altcol;
    col = col;
    altcol = altcol;

    if(col == indstack[*indent]) {
        if (altcol != altindstack[*indent]) return -10;
    } else if(col > indstack[*indent]) {
        if (*indent+1 >= MAXINDENT) return -20;
        if (altcol <= altindstack[*indent]) return -30;
        pendin++;
        indstack[++*indent] = col;
        altindstack[*indent] = altcol;
    } else {
        while (*indent > 0 &&
            col < indstack[*indent]) {
            pendin--;
            (*indent)--;
        }
        if (col != indstack[*indent]) return -40;
        if (altcol != altindstack[*indent]) return -50;
    }

    return pendin < 0 ? -pendin : pendin; // for now
}

unint skip_until_next_line(char** file, char* end, int32_t* cps) {
    unint nread = 0;
    unint c = 0;
    while(true) {
        nread = READ_GRAPHEME(file, end, cps);
        if(nread < 0) return nread;
        
        if((nread == 1 && *cps == (int32_t)'\n')) return c;
        ++c;
    }
}