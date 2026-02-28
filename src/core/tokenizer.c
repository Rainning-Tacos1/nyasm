#include "types.h"
#include "tokenizer.h"

#include "api/unicode.h"
#include "api/debug.h"

#ifndef EOF
    #define EOF -1
#endif

void tok_state_init(struct tok_state* tok) {
    for(unint i=0; i<MAX_INDENT; ++i) tok->altindstack[i] = tok->indstack[i] = 0;
    tok->pendin = tok->indent = tok->err = tok->nread = 0;
    tok->atbol = 1;
    UNICODE_INIT(&tok->uc);
}

/*
  Tries to read the next grapheme
  Updates tok fields:
    nread, err, cps[0]

  You can perform "normal" character checks as it never trigger them
*/
void next_grapheme(struct tok_state* tok) {
    unint err;
    if((err = READ_GRAPHEME(&tok->uc, &tok->nread)) == SUCCESS) {
        tok->err = 0;
        return;
    }
    // Set error params so that we can do "normal" checks and fail every time
    tok->nread = -1;
    tok->err = err;
    *tok->uc.cps = EOF;
}

nint get_indentation(struct tok_state* tok) {
    unint col = 0;
    unint altcol = 0;
}

nint tokenize(struct tok_state* tok, struct token* token) {
    unint col = 0;
    unint altcol = 0;
    unint blankline = 0;

    int32_t* cps = tok->uc.cps;

    if(tok->atbol) {
        tok->atbol = 0;
        for(;;) {
            next_grapheme(tok);
            if(tok->nread != 1) break;

            if(*cps == (int32_t)' ') { ++col; ++altcol; }
            else if(*cps == (int32_t)'\t') {
                col = (col / TAB_SIZE + 1) * TAB_SIZE;
                altcol = (altcol / ALT_TAB_SIZE + 1) * ALT_TAB_SIZE;
            }
            // No formfeed
            // Handle multiline
            else break;
        }
        // Blank lines
        if(tok->nread == 1 && *cps == (int32_t)'\n') blankline = 1;
        // col = cont_line_col ? cont_line_col : col;
        // altcol = cont_line_col ? cont_line_col : altcol;
        if(!blankline) {
            col = col;
            altcol = altcol;

            if(col == tok->indstack[tok->indent]) {
                if (altcol != tok->altindstack[tok->indent]) return -10;
            } else if(col > tok->indstack[tok->indent]) {
                if (tok->indent+1 >= MAX_INDENT) return -20;
                if (altcol <= tok->altindstack[tok->indent]) return -30;
                tok->pendin++;
                tok->indstack[++tok->indent] = col;
                tok->altindstack[tok->indent] = altcol;
            } else {
                while (tok->indent > 0 &&
                    col < tok->indstack[tok->indent]) {
                    tok->pendin--;
                    tok->indent--;
                }
                if (col != tok->indstack[tok->indent]) return -40;
                if (altcol != tok->altindstack[tok->indent]) return -50;
            }
        }
    }

    /* Return pending indents/dedents */
    if (tok->pendin != 0) {
        if (tok->pendin < 0) {
            tok->pendin++;
            return 20;
        }
        else {
            tok->pendin--;
            return 10;
        }
    }


    tok->atbol = (tok->nread == 1 && *cps == (int32_t)'\n'); 
    next_grapheme(tok);

    return (tok->err != 0) ? tok->err : 30;
}