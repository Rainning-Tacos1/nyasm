
#include "config.h"
#include "types.h"

#include "api/unicode.h"
#include "api/log.h"
#include "api/memory.h"
#include "api/debug.h"

#include "tokenizer.h"
#include "token.h"

nbool assemble(char* code, unint len) {
    struct tok_state tok;
    tok_state_init(&tok);

    tok.uc.curr = tok.uc.buf = code;
    tok.uc.end = code+len;

    unint nread = 0;
    unint suc;

    struct token token;
    nint _tok;
    DBG(1, "Start!\n");
    do {
        _tok = tokenize(&tok, &token);
        DBG(1, "_tok: %d\n", _tok);
    } while(_tok != ERRORTOKEN);

    if(suc == UNICODE_ERR_CODEPOINT) { LOG("CodePointError\n"); return FAIL;}
    if(suc == UNICODE_ERR_GRAPHEME) { LOG("GraphemeError\n"); return FAIL;}
    if(suc == UNICODE_ERR_EOF) { LOG("EOF\n"); return FAIL;}
    if(suc == UNICODE_ERR_NORMALIZE) { LOG("NormalizationError\n"); return FAIL;}
    if(suc == UNICODE_ERR_TOO_SMALL) { LOG("CantStoreGrapheme\n"); return FAIL;}
    
    return SUCCESS;
}