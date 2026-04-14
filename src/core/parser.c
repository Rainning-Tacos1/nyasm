#include <stdarg.h>
#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#include "token.h"
#include "err.h"

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "variables.h"

#include "types.h"

#define IF_IDENTIDIER ((int32_t[]){'i', 'f', -1})
#define INCLUDE_IDENTIFIER ((int32_t[]){'i', 'n', 'c', 'l', 'u', 'd', 'e', -1})
#define EXPORT_IDENTIFIER ((int32_t[]){'e', 'x', 'p', 'o', 'r', 't', -1})
#define EXTERN_IDENTIFIER ((int32_t[]){'e', 'x', 't', 'e', 'r', 'n', -1})

// Errors

// Do not call on multiline comment tokens
void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);
    //DBG(1, "col_offset = %d | end_col_offset = %d\n", _token->col_offset, _token->end_col_offset);
    _syntaxerror_range_with_type(p->tok, stype, format, _token->lineno, _token->end_lineno, _token->col_offset+1, _token->end_col_offset+1, va);
    //_format_syntax_error(stype, format, p->tok->source, &_token->lineno, &col_offset, &_token->end_lineno, &end_col_offset, _token->start, _token->end, va);
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

// Token

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
    if (type == ERRORTOKEN && p->tok->done == E_DECODE) return FAIL;
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

void _reset_peek(struct Parser* p) {
    p->peek = p->last_token;
}

struct Parser* _Parser_New(struct tok_state* tok) {
    struct Parser* p = MEM_ALLOC(sizeof(struct Parser), "parser");
    if(p == NULL) return NULL;

    p->tok = tok;
    p->variables = p->variables_tail = NULL;
    p->head = p->tokens = p->last_token = p->peek = p->tail = NULL;

    return p;
}

unint is_at_identifier(struct token* _token, int32_t* identifier) {
    // Reject if its not an identifier
    if(_token->type != NAME) return FAIL;

    // Just check if it starts with @
    if(identifier == NULL) {
        if(_token->len < 1) return FAIL;
        return (_token->cps[0] == '@') ? SUCCESS : FAIL;
    }

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

// Pratt Parsing
#define UNARY_BP 120

unint _expr_get_binding_power(unint type, unint* lbp, unint* rbp) {
    switch(type) {
        case DOUBLEVBAR: { *lbp = 10; *rbp = 11; break; }
        case DOUBLEAMPER:  { *lbp = 20; *rbp = 21; break; }

        case NOTEQUAL:
        case GREATER:
        case GREATEREQUAL:
        case EQEQUAL:
        case LESS:
        case LESSEQUAL: { *lbp = 30; *rbp = 31; break; }

        case VBAR: { *lbp = 40; *rbp = 41; break; }
        case CIRCUMFLEX: { *lbp = 50; *rbp = 51; break; }
        case AMPER: { *lbp = 60; *rbp = 61; break; }

        case LEFTSHIFT:
        case RIGHTSHIFT: { *lbp = 70; *rbp = 71; break; }

        case PLUS:
        case MINUS: { *lbp = 100; *rbp = 101; break; }

        case SLASH:
        case DOUBLESLASH:
        case PERCENT:
        case STAR: { *lbp = 110; *rbp = 111; break; }

        case DOUBLESTAR: { *lbp = 140; *rbp = 130; break; }
        default : return FAIL;
    }
    return SUCCESS;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_bp);

// Will return NULL or an AST
struct Ast_node* _parse_expr_prefix(struct Parser* p) {
    // Request a token
    struct token* _token = _read_token(p);
    if(_token == NULL) return NULL;

    switch(_token->type) {
        case LPAR:
            struct Ast_node* left = _parse_expr(p, 0);

            struct token * rpar = _read_token(p);
            if(rpar == NULL || rpar->type != RPAR) {
                DBG(1, "does not end on ')'\n");
                return NULL;
            }
            
            return left;
        case STRING:
            DBG(1, "Expr pref is string\n");
            struct Ast_node* str = new_ast_string(_token->cps, _token->len);
            if(str == NULL) _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the string");
            return str;
        case NUMBER:
            DBG(1, "Expr pref is number\n");
            struct Ast_node* number = new_ast_number(_token->cps, _token->len);
            if(number == NULL) _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the number");
            return number;
        case MINUS:
        case PLUS:
        case TILDE:
        case EXCLAMATION:
            struct Ast_node* node = _parse_expr(p, UNARY_BP);
            if(node == NULL) return NULL;

            struct Ast_node* unary = new_ast_binop(_token->type, node, NULL);
            if(unary == NULL) _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for unary expression");
            return unary;

        case NAME:
            if(is_at_identifier(_token, NULL) == SUCCESS) {
                _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "@ identifiers are not allowed on expressions");
                return NULL;
            }

            struct Variable* var = get_variable(p, _token->cps, _token->len);
            if(var == NULL) {
                _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "variable is not declared");
                return NULL;
            }

            struct Ast_node* var_node = new_ast_variable(var);
            if(var_node == NULL) _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the variable");
            return var_node;
        default:
            _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "invalid token for expression");
            return NULL;
    }
    // Just in case
    return NULL;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_bp) {
    // Prefix
    struct Ast_node* left = _parse_expr_prefix(p);
    if(left == NULL) return NULL;

    unint lbp, rbp;
    while(true) {
        struct token* op = _peek_token(p);
        if(op == NULL) return NULL;

        if(op->type == ENDMARKER || op->type == NEWLINE || op->type == RPAR) break;

        unint suc = _expr_get_binding_power(op->type, &lbp, &rbp);
        if(suc == FAIL) {
            _error_from_token(p, op, ERROR_TYPE_EXPRESSION, "Invalid operator in expression");
            return NULL;
        }

        if(lbp < min_bp) break;

        op = _read_token(p);
        if(op == NULL) return NULL;

        struct Ast_node* right = _parse_expr(p, rbp);
        if(right == NULL) return NULL;

        // build the AST node
        left = new_ast_binop(op->type, left, right);
        if(left == NULL) return NULL;
    }

    // reset the peek so it doesnt keep peeking forward
    _reset_peek(p);
    return left;
}

extern const char * const _Parser_TokenNames[];

void* _run_parser(struct Parser* p) {

    struct token* _token;
    void* ast = NULL;
    unint suc = 0;

    // Create the first AST node
    if(new_ast(p) == FAIL) {
        memory_error(p, "No available memory for AST's root node");
        return NULL;
    }

    struct Variable* var = new_variable(p, (int32_t[]){'h','e','l','l','o','_','w','o','r','l','d'}, 11);
    if(var == NULL) {
        DBG(1, "Error building variable\n");
        return NULL;
    }

    (is_variable_declared(p, (int32_t[]){'h', 'i'}, 2) == SUCCESS) ?
        DBG(1, "Variable is declared\n") :
        DBG(1, "Variable is NOT declared\n");

    struct Ast_node* expr = _parse_expr(p, (unint)(0));
    if(expr == NULL) {
        DBG(1, "Error building expression\n");
        return NULL;
    }
    
    DBG(1, "#################################\n");
    dbg_ast(expr);

    DBG(1, "#################################\n");

    // Skip comments && new lines
    while((_token = _read_token(p)) != NULL && (_token->type == COMMENT || _token->type == NEWLINE));
    if(_token == NULL) return NULL;

    DBG(1, "DONE SKIPING\n");

    if(is_at_identifier(_token, IF_IDENTIDIER) == SUCCESS) DBG(1, "FOUND AN IF\n");
    else if (is_at_identifier(_token, INCLUDE_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN INCLUDE\n");
    else if (is_at_identifier(_token, EXTERN_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN EXTER\n");
    else if (is_at_identifier(_token, EXPORT_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN EXPORT\n");

    while((_token = _read_token(p)) != NULL && _token->type != ENDMARKER);
    return NULL;
}