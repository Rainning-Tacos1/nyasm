
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
    tok.uc.end = code+len-1;
    tok.source = "main";

    struct token token;
    unint _tok;

    DBG(1, "Start!\n");
    do {
        _tok = tokenize(&tok, &token);
        if(tok.uc.err != UNICODE_OK) _tok = ERRORTOKEN;

        switch(_tok) {
            case NAME: {DBG(1, "[IDENTIFIER]\n"); break; }
            case INDENT: {DBG(1, "[INDENT]\n"); break; }
            case DEDENT: {DBG(1, "[DEDENT]\n"); break; }
            case NEWLINE: {DBG(1, "[\\n]\n"); break; }
            case NUMBER: {DBG(1, "[NUMBER]\n"); break; }
            case COMMENT: {DBG(1, "[COMMENT]\n"); break;}
            case ENDMARKER: {DBG(1, "[EOF]\n"); break; }
            case EQUAL: {DBG(1, "[=]\n"); break; }
            case EQEQUAL: {DBG(1, "[==]\n"); break; }
            case ERRORTOKEN: {DBG(1, "[ERR TOKEN]\n"); break; }
            case STRING: {DBG(1, "[STRING]\n"); break; }
            default: {DBG(1, "[???](%d)", _tok); break; }
        }
        //if(_tok != NEWLINE && _tok != DEDENT && _tok != ERRORTOKEN) DBG(1, " ");

    } while(_tok != ERRORTOKEN && _tok != ENDMARKER);

    if(tok.uc.err == UNICODE_ERR_CODEPOINT) { LOG("CodePointError\n"); return FAIL;}
    
    return SUCCESS;
}