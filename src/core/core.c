
#include "config.h"
#include "types.h"

#include "api/unicode.h"
#include "api/log.h"
#include "api/memory.h"
#include "api/debug.h"

#include "lexer.h"
#include "token.h"
#include "err.h"

nbool assemble(char* code, unint len) {
    struct tok_state tok;
    tok_state_init(&tok);

    tok.uc.curr = tok.uc.buf = code;
    tok.inp = tok.uc.curr; // Trigger an underflow/verification
    tok.uc.end = code+len; // There is still space for an implicite newline
    tok.source = "main";

    struct token token;
    unint _tok;

    DBG(1, "Start!\n");
    do {
        _tok = tokenize(&tok, &token);
        
    } while(_tok != ERRORTOKEN && _tok != ENDMARKER);

    (_tok == ERRORTOKEN) ?
        DBG(1, "Error: %d\n", tok.done) :
        DBG(1, "End\n");
    
    return SUCCESS;
}