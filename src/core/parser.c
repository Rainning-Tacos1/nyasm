#include <stdarg.h>
#include "types.h"
#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "err.h"
#include "ast.h"
#include "variables.h"

#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#define IF_IDENTIDIER ((int32_t[]){'i', 'f', -1})
#define INCLUDE_IDENTIFIER ((int32_t[]){'i', 'n', 'c', 'l', 'u', 'd', 'e', -1})

// Do not call on multiline comment tokens
void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);
    nint col_offset = 1;
    nint end_col_offset = _token->end_col_offset - _token->col_offset + 1;
    _format_syntax_error(stype, format, p->tok->source, &_token->lineno, &col_offset, &_token->end_lineno, &end_col_offset, _token->start, _token->end, va);
    va_end(va);
}

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

void memory_error(struct Parser* p, const char* format, ...) {
    va_list va;
    va_start(va, format);
    _format_syntax_error(ERROR_TYPE_MEMORY, format, p->tok->source, NULL, NULL, NULL, NULL, NULL, NULL, va);
    va_end(va);
}

unint _fill_token(struct Parser* p) {
    // Allocate
    struct token* new_token = (struct token*)MEM_ALLOC(sizeof(struct token), "token struct");
    if(new_token == NULL) {
        memory_error(p, "no available memory for token structure"); 
        return FAIL;
    }

    unint type = tokenize(p->tok, new_token);

    // Link
    if(p->tail != NULL) p->tail->next = new_token;
    p->tail = new_token;

    // Firt token
    if(p->head == NULL) p->head = new_token;

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
                memory_error(p, "no available memory for token");
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

struct token* _read_token(struct Parser* p) {
    // Advance if we already have tokens
    if (p->last_token && p->last_token != p->tail)
        return (p->last_token = p->last_token->next);

    // Ensure we have tokens
    if (!p->last_token) {
        if (!p->head && _fill_token(p) == FAIL)
            return NULL;
        return (p->peek = p->last_token = p->head);
    }

    // We're at tail, try to extend
    if (_fill_token(p) == FAIL)
        return NULL;

    return (p->peek = p->last_token = p->tail);
}

struct token* _peek_token(struct Parser* p) {
    // Advance if we already have tokens
    if (p->peek && p->peek != p->tail)
        return (p->peek = p->peek->next);

    // Ensure we have tokens
    if (!p->peek) {
        if (!p->head && _fill_token(p) == FAIL)
            return NULL;
        return (p->peek = p->head);
    }

    // We're at tail, try to extend
    if (_fill_token(p) == FAIL)
        return NULL;

    return (p->peek = p->tail);
}

struct Parser* _Parser_New(struct tok_state* tok) {
    struct Parser* p = MEM_ALLOC(sizeof(struct Parser), "parser");
    if(p == NULL) return NULL;

    p->tok = tok;
    p->head = p->tokens = p->last_token = p->peek = p->tail = NULL;

    return p;
}

unint is_at_identifier(struct token* _token, int32_t* identifier) {
    // Reject if its not an identifier
    if(_token->type != NAME) return FAIL;

    // Get the len of the identifier
    unint identifier_len = 0;
    while (identifier[identifier_len] != -1) ++identifier_len;

    // First size check
    if(_token->len < identifier_len + 1) return FAIL;

    // Check for @
    if(_token->cps[0] != '@') return FAIL;

    for(unint i=0; i<(identifier_len); ++i) if(_token->cps[i+1] != identifier[i]) return FAIL;
    return SUCCESS;
}

// Will return NULL or an AST
struct Ast_node* _parse_expr_prefix(struct Parser* p) {
    // Request a token
    struct token* _token = _read_token(p);
    if(_token == NULL) return NULL;

    switch(_token->type) {
        case STRING:
            struct Ast_node* str = new_ast_string(_token->cps, _token->len);
            if(str == NULL) _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the string");
            return str;
        case NUMBER:
            struct Ast_node* number = new_ast_string(_token->cps, _token->len);
            if(number == NULL) _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the number");
            return number;
        case NAME:
            // TODO
            break;

        case PLUS: break;
        case MINUS : break;

        default:
            _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "invalid token for expression");
            return NULL;
    }
    // Just in case
    return NULL;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_prec) {
    // Prefix
    struct Ast_node* left = _parse_expr_prefix(p);
    if(left == NULL) return NULL;

    while(true) {
        struct token* op = _peek_token(p);
        if(op == NULL) return NULL;

        if(op->type == ENDMARKER) break;

        // ...

    }

    return left;
}

extern const char * const _Parser_TokenNames[];

void* _run_parser(struct Parser* p) {

    struct token* _token;
    void* ast = NULL;
    unint suc = 0;

    // Create the first AST node
    if(new_ast(p) == FAIL) {
        memory_error(p, "no available memory for AST's root node");
        return NULL;
    }

    // Skip comments && new lines
    while((_token = _read_token(p)) != NULL && (_token->type == COMMENT || _token->type == NEWLINE));
    if(_token == NULL) return NULL;

    DBG(1, "DONE SKIPING\n");

    if(is_at_identifier(p, IF_IDENTIDIER) == SUCCESS) DBG(1, "FOUND AN IF\n");
    else if (is_at_identifier(p, INCLUDE_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN INCLUDE\n");
    

    while((_token = _read_token(p)) != NULL && _token->type != ENDMARKER);
    return NULL;
}