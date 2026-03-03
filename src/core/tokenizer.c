#include "types.h"
#include "tokenizer.h"

#include "api/unicode.h"
#include "api/debug.h"

#include "err.h"
#include "token.h"

void tok_state_init(struct tok_state* tok) {
    for(unint i=0; i<MAX_INDENT; ++i) tok->altindstack[i] = tok->indstack[i] = 0;
    tok->lineno = tok->pendin = tok->indent = tok->nread = 0;
    tok->err = E_OK;
    tok->atbol = 1;
    UNICODE_INIT(&tok->uc);
}

unint make_token(struct tok_state* tok, unint token_type, struct token* token) {
    // For now
    token->lineno = tok->lineno;
    return token_type;
}

/*
  Tries to read the next grapheme
  Updates tok fields:
    nread, err, cps

  In case of error:
    you can perform "normal" character checks as they will never trigger
*/
void next_grapheme(struct tok_state* tok) {
    nbool fail;
    if((fail = READ_GRAPHEME(&tok->uc, &tok->nread)) == UNICODE_OK) return;

    // Set error params so that we can do "normal" checks and fail every time
    // Except for EOF, pls check if its bc of an error.

    tok->nread = -1;
    tok->err = E_UNICODE;
    *tok->uc.cps = EOF;
}

/*
  Parse indentation and update tokenizer state
  Returns:
    0 : Success
    1 : Indentation error
*/
nbool get_indentation(struct tok_state* tok) {
    unint col = 0;
    unint altcol = 0;
    unint blankline = 0;

    int32_t* cps = tok->uc.cps;

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
            if (altcol != tok->altindstack[tok->indent]) DO_FAIL(tok->err = E_TABSPACE);
        } else if(col > tok->indstack[tok->indent]) {
            if (tok->indent+1 >= MAX_INDENT) DO_FAIL(tok->err = E_TOODEEP);
            if (altcol <= tok->altindstack[tok->indent]) DO_FAIL(tok->err = E_TABSPACE);
            tok->pendin++;
            tok->indstack[++tok->indent] = col;
            tok->altindstack[tok->indent] = altcol;
        } else {
            while (tok->indent > 0 &&
                col < tok->indstack[tok->indent]) {
                tok->pendin--;
                tok->indent--;
            }
            if (col != tok->indstack[tok->indent]) DO_FAIL(tok->err = E_DEDENT);
            if (altcol != tok->altindstack[tok->indent]) DO_FAIL(tok->err = E_TABSPACE);
        }
    }
    
    return (tok->uc.err != UNICODE_OK);
}

/*
  Tells if a indent or a dedent needs to be generated
  Pair this function with a check for tok->pendin == 0
  Returns:
    indent or dedent
*/
unint generate_indent_dedent_token(struct tok_state* tok, struct token* token) {
    if (tok->pendin < 0) {
        tok->pendin++;
        return make_token(tok, DEDENT, token);
    }
    else {
        tok->pendin--;
        return make_token(tok, INDENT, token);
    }
}

nint tokenize(struct tok_state* tok, struct token* token) {
    int32_t* cps = tok->uc.cps;

    // If: At Begining Of Line
    if(tok->atbol) {
        tok->atbol = 0;
        // Updates tok->pendin and errorrs
        if(get_indentation(tok) == FAIL) return make_token(tok, ERRORTOKEN, token);

    }

    // Need to generate indent/dedent tokens?
    while (tok->pendin != 0) return generate_indent_dedent_token(tok, token); // Updates tok->pendin
    for(;;) {
        switch(*tok->uc.cps) {
            // Use python lexing for new lines
            case '\r':
                next_grapheme(tok);
            case '\n':
                if (*tok->uc.cps != '\n') break; // Check because of \r fallthrough. The same is done in python but with ifs

                ++tok->lineno;
                tok->atbol = 1;

                return 40;
            case ' ': case '\f': case '\t': case '\v': {  /* spaces */
                next_grapheme(tok);
                break;
            }
            default:
                DBG(1, "Char: %c ", (char)*cps);
                next_grapheme(tok);
                return 30;
        }
    }
}