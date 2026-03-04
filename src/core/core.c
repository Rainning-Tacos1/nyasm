
#include "config.h"
#include "types.h"

#include "api/unicode.h"
#include "api/log.h"
#include "api/memory.h"
#include "api/debug.h"

#include "lexer.h"
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

        switch(_tok) {
            case NAME: {DBG(1, "[IDENTIFIER]"); break; }
            case INDENT: {DBG(1, "[INDENT]"); break; }
            case DEDENT: {DBG(1, "[DEDENT]\n"); break; }
            case NEWLINE: {DBG(1, "[\\n]\n"); break; }
            case ERRORTOKEN: {DBG(1, "[ERR TOKEN]\n"); break; }
            case 70: {DBG(1, "[STRING]"); break; }
            default: {DBG(1, "[???]"); break; }
        }
        if(_tok != NEWLINE && _tok != DEDENT && _tok != ERRORTOKEN) DBG(1, " ");

    } while(_tok != ERRORTOKEN);

    if(suc == UNICODE_ERR_CODEPOINT) { LOG("CodePointError\n"); return FAIL;}
    if(suc == UNICODE_ERR_GRAPHEME) { LOG("GraphemeError\n"); return FAIL;}
    if(suc == UNICODE_ERR_EOF) { LOG("EOF\n"); return FAIL;}
    if(suc == UNICODE_ERR_NORMALIZE) { LOG("NormalizationError\n"); return FAIL;}
    if(suc == UNICODE_ERR_TOO_SMALL) { LOG("CantStoreGrapheme\n"); return FAIL;}
    
    return SUCCESS;
}