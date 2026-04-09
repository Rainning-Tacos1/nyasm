#include <stdarg.h>
#include "types.h"
#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "err.h"

#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#define IF_IDENTIDIER ((int32_t[]){'i', 'f', -1})
#define INCLUDE_IDENTIFIER ((int32_t[]){'i', 'n', 'c', 'l', 'u', 'd', 'e', -1})

void _error_no_err_line(struct Parser* p, const char* errmsg, ...) {
    va_list va;
    va_start(va, errmsg);
    _format_syntax_error(ERROR_TYPE_MESSAGE, errmsg, p->tok->source, &p->tok->lineno, NULL, &p->tok->lineno, NULL, NULL, NULL, va);
    va_end(va);
}

void _error_known_location(struct Parser* p, nint lineno, nint col_offset, nint end_lineno, nint end_col_offset, const char* errmsg, ...) {
    va_list va;
    va_start(va, errmsg);
    _syntaxerror_range(p->tok, errmsg, lineno, end_lineno, col_offset, end_col_offset, va);
    va_end(va);
}

void memory_error() {
    LOG("out of memory");
}

unint _fill_token(struct Parser* p) {

    // Allocate
    struct token* new_token = (struct token*)MEM_ALLOC(sizeof(struct token), "token struct");
    if(new_token == NULL) {
        memory_error(); return FAIL;
    }

    unint type = tokenize(p->tok, new_token);

    // Skip comments
    //while (type == COMMENT || type == NEWLINE) {
    //    type = tokenize(p->tok, new_token);
    //    if(type == ERRORTOKEN && p->tok->done == E_NOMEM) { memory_error(); return -1; }
    //}

    // Link
    if(p->last_token != NULL) p->last_token->next = new_token;
    p->last_token = new_token;

    // Check for errors
    if (type == ERRORTOKEN && p->tok->done == E_DECODE) return -1;
    const char *msg = NULL;
    if(type == ERRORTOKEN) {
        switch(p->tok->done) {
            case E_EOF:
                if(p->tok->level) {
                    nint error_lineno = p->tok->parenlinenostack[p->tok->level-1];
                    nint error_col = p->tok->parencolstack[p->tok->level-1];
                    _error_known_location(p, error_lineno, error_col, error_lineno, -1, "'%c' was never closed", p->tok->parenstack[p->tok->level-1]);
                } else _Tokenizer_syntaxerror_known_range(p->tok, -1, -1, "unexpected EOF while parsing");
                return FAIL;
            case E_DEDENT:
                _Tokenizer_syntaxerror_known_range(p->tok, -1, -1, "unindent does not match any outer indentation level");
                return FAIL;
            case E_NOMEM:
                memory_error();
                return FAIL;
            case E_TABSPACE:
                msg = "inconsistent use of tabs and spaces in indentation";
                break;
            case E_TOODEEP:
                msg = "too many levels of indentation";
                break;
            case E_COLUMNOVERFLOW:
                _error_no_err_line(p, "parser column offset overflow - source line is too big");
                return FAIL;
            default:
                msg = "unknown parsing error";        
        }
        _error_known_location(p, p->tok->lineno, 0, p->tok->lineno, -1, msg);
        return FAIL;
    }

    return SUCCESS;
}

struct Parser* _Parser_New(struct tok_state* tok) {
    struct Parser* p = MEM_ALLOC(sizeof(struct Parser), "parser");
    if(p == NULL) return NULL;

    p->tok = tok;
    p->tokens = NULL;
    p->last_token = NULL;
    return p;
}

unint is_at_identifier(struct Parser* p, int32_t* identifier) {
    // Reject if its not an identifier
    if(p->last_token->type != NAME) return FAIL;

    // Get the len of the identifier
    unint identifier_len = 0;
    while (identifier[identifier_len] != -1) ++identifier_len;

    // First size check
    if(p->last_token->len < identifier_len + 1) return FAIL;

    // Check for @
    if(p->last_token->cps[0] != '@') return FAIL;

    for(unint i=0; i<(identifier_len); ++i) if(p->last_token->cps[i+1] != identifier[i]) return FAIL;
    return SUCCESS;
}

void* _run_parser(struct Parser* p) {

    void* ast = NULL;
    unint suc = 0;

    // Skip comments && new lines
    while((suc = _fill_token(p)) == SUCCESS && (p->last_token->type == COMMENT || p->last_token->type == NEWLINE));
    if(suc == FAIL) return NULL;

    DBG(1, "DONE SKIPING\n");

    if(is_at_identifier(p, IF_IDENTIDIER) == SUCCESS) DBG(1, "FOUND AN IF\n");
    else if (is_at_identifier(p, INCLUDE_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN INCLUDE\n");
    

    while(_fill_token(p) == SUCCESS && p->last_token->type != ENDMARKER);
    return NULL;
}