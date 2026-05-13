#include <stdarg.h>
#include <math.h>
#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#include "token.h"
#include "err.h"

#include "lexer.h"
#include "parser.h"
#include "variables.h"
#include "ast.h"

#include "types.h"

#define MACRO_IDENTIDIER ((int32_t[]){'m', 'a', 'c', 'r', 'o', -1})
#define IF_IDENTIFIER ((int32_t[]){'i', 'f', -1})
#define INCLUDE_IDENTIFIER ((int32_t[]){'i', 'n', 'c', 'l', 'u', 'd', 'e', -1})
#define EXPORT_IDENTIFIER ((int32_t[]){'e', 'x', 'p', 'o', 'r', 't', -1})
#define EXTERN_IDENTIFIER ((int32_t[]){'e', 'x', 't', 'e', 'r', 'n', -1})
#define ASSERT_IDENTIFIER ((int32_t[]){'a', 's', 's', 'e', 'r', 't', -1})
#define WARN_IDENTIFIER ((int32_t[]){'w', 'a', 'r', 'n', -1})
#define ERROR_IDENTIFIER ((int32_t[]){'e', 'r', 'r', 'o', 'r', -1})

extern const char * const _Parser_TokenNames[];

const char * const _Parser_TokenSymbols[] = {
    "<ENDMARKER>",     // ENDMARKER
    "identifier",          // NAME
    "number",        // NUMBER
    "string",        // STRING
    "new line",              // NEWLINE
    "indent",        // INDENT
    "dedent",        // DEDENT
    "(",               // LPAR
    ")",               // RPAR
    "[",               // LSQB
    "]",               // RSQB
    ":",               // COLON
    ",",               // COMMA
    ";",               // SEMI
    "+",               // PLUS
    "-",               // MINUS
    "*",               // STAR
    "/",               // SLASH
    "|",               // VBAR
    "&",               // AMPER
    "<",               // LESS
    ">",               // GREATER
    "=",               // EQUAL
    ".",               // DOT
    "%",               // PERCENT
    "{",               // LBRACE
    "}",               // RBRACE
    "==",              // EQEQUAL
    "!=",              // NOTEQUAL
    "<=",              // LESSEQUAL
    ">=",              // GREATEREQUAL
    "~",               // TILDE
    "^",               // CIRCUMFLEX
    "<<",              // LEFTSHIFT
    ">>",              // RIGHTSHIFT
    "**",              // DOUBLESTAR
    "+=",              // PLUSEQUAL
    "-=",              // MINEQUAL
    "*=",              // STAREQUAL
    "/=",              // SLASHEQUAL
    "%=",              // PERCENTEQUAL
    "&=",              // AMPEREQUAL
    "|=",              // VBAREQUAL
    "^=",              // CIRCUMFLEXEQUAL
    "<<=",             // LEFTSHIFTEQUAL
    ">>=",             // RIGHTSHIFTEQUAL
    "**=",             // DOUBLESTAREQUAL
    "//",              // DOUBLESLASH
    "//=",             // DOUBLESLASHEQUAL
    "@",               // AT
    "@=",              // ATEQUAL
    "->",              // RARROW
    "...",             // ELLIPSIS
    ":=",              // COLONEQUAL
    "!",               // EXCLAMATION
    "<OP>",            // OP (generic operator)
    "<TYPE_IGNORE>",   // TYPE_IGNORE
    "<TYPE_COMMENT>",  // TYPE_COMMENT
    "<SOFT_KEYWORD>",  // SOFT_KEYWORD
    "<FSTRING_START>", // FSTRING_START
    "<FSTRING_MIDDLE>",// FSTRING_MIDDLE
    "<FSTRING_END>",   // FSTRING_END
    "<TSTRING_START>", // TSTRING_START
    "<TSTRING_MIDDLE>",// TSTRING_MIDDLE
    "<TSTRING_END>",   // TSTRING_END
    "<COMMENT>",               // COMMENT
    "\\n",              // NL
    "<ERRORTOKEN>",    // <ERRORTOKEN>
    "<ENCODING>",      // <ENCODING>
    "<N_TOKENS>",      // <N_TOKENS>
    "||",              // DOUBLEVBAR
    "&&",              // DOUBLEAMPER
};

// Errors
unint macro_trace_len(struct MacroTrace* mt) {
    unint n = 0;
    while(mt) {
        ++n;
        mt = mt->parent;
    }
    return n;
}

void _print_macro_trace_select(struct MacroTrace* mt, unint i, unint n) {
    if (mt->parent)
        _print_macro_trace_select(mt->parent, i + 1, n);

    unint do_print = 0;

    if (n <= MACRO_EXPANSION_TRACE_START_LIMIT + MACRO_EXPANSION_TRACE_END_LIMIT) {
        do_print = 1;
    } else {
        if (i < MACRO_EXPANSION_TRACE_START_LIMIT) do_print = 1;
        if (i >= n - MACRO_EXPANSION_TRACE_END_LIMIT) do_print = 1;
    }
    if (do_print) {
        LOG("  macro '");
        struct Macro* macro = mt->macro;
        for (unint j = 0; j < macro->name.len; ++j)
            LOG_CP(macro->name.cps[j]);
        LOG("' line: %d\n", mt->lineno);
    }
    if (n > MACRO_EXPANSION_TRACE_START_LIMIT + MACRO_EXPANSION_TRACE_END_LIMIT && i == MACRO_EXPANSION_TRACE_START_LIMIT) {
        LOG("  ...\n");
    }
}
void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);

    struct MacroTrace* mt = _token->macro_trace;
    
    if(mt) {
        LOG("Traceback:\n");
        unint n = macro_trace_len(mt);
        _print_macro_trace_select(mt, 0, n);
        LOG("\n");
    }

    const char* line_start = p->tok->line_start;
    p->tok->line_start = _token->line_start;
    _syntaxerror_range_with_type(p->tok, stype, format, _token->lineno, _token->end_lineno, _token->col_offset+1, _token->end_col_offset+1, va);
    p->tok->line_start = line_start;

    va_end(va);
}

void _error_line_with_cursor(struct Parser* p, struct token* _token, nint col_offset, nint end_col_offset, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);

    struct MacroTrace* mt = _token->macro_trace;
    
    if(mt) {
        LOG("Traceback:\n");
        unint n = macro_trace_len(mt);
        _print_macro_trace_select(mt, 0, n);
        LOG("\n");
    }

    const char* line_start = p->tok->line_start;
    p->tok->line_start = _token->line_start;
    _syntaxerror_range_with_type(p->tok, stype, format, _token->lineno, _token->end_lineno, col_offset+1, end_col_offset+1, va);
    p->tok->line_start = line_start;

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
    
unint is_at_identifier(struct token* _token, int32_t* identifier);
unint is_macro_declared(struct Parser* p, int32_t* cps, unint len);
struct Macro* get_macro(struct Parser* p, int32_t* cps, unint len);

unint _fill_token_error_check(struct Parser* p, unint type) {
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

unint _compare_identifiers(int32_t* cps1, unint len1, int32_t* cps2, unint len2) {
    if(len1 != len2) return FAIL;

    unint i;
    for(i = 0; i < len1; ++i) if(cps1[i] != cps2[i]) break;
        
    // Full match
    if(i == len1) return SUCCESS;
    return FAIL;
}

struct token* _advance(struct Parser* p, struct token** rp) {
    // If there are tokens available, walk without allocating
    if(*rp && (*rp)->next != NULL) return (*rp = (*rp)->next);

    struct token* _token = (struct token*)MEM_ALLOC(sizeof(struct token), "token struct");
    if(_token == NULL) {
        memory_error(p, "no available memory for token structure"); 
        return NULL;
    }

    unint type = tokenize(p->tok, _token);
    if(_fill_token_error_check(p, type) == FAIL) return NULL;

    // Link & Advance
    if(*rp != NULL) (*rp)->next = _token;
    return (*rp = _token);
}

struct token* _parse_macro_call_arg_until_comma_or_macro_call_end(struct Parser* p, struct token** start, struct token** end, struct token** rp) {
    unint level = 0;
    *end = *start = NULL;
    while (true) {
        struct token* _token;
        if ((_token = _advance(p, rp)) == NULL) return NULL;

        if (level == 0 && (_token->type == COMMA || _token->type == RPAR)) return _token;

        if(!*start) *start = _token;

        *end = _token;
        if (_token->type == LPAR) ++level;
        else if (_token->type == RPAR) --level;
    }
}

// [re == where to start reading (NULL)] _read(p, &s, &e, macro_trace, &re)
// s != NULL && (s == e) && s == re: one token (tokens to replace)
// s == NULL && e == NULL: empty expansion (re == end token of macro call)
// s != NULL && e != NULL && s != re: macro expansion (re == end token of macro call)
// ... : never
unint _read(struct Parser* p, struct token** start,  struct token** end, struct token** re, struct MacroTrace* mt) {
    struct token* _token = _advance(p, re);
    if(_token == NULL) return FAIL;


    *end = *start = _token; // Begin the chain

    unint do_expand = 1;

    // Block everything other than a macro
    if(p->is_inside_macro_decl || 
        _token->type != NAME || 
        is_at_identifier(_token, NULL) == SUCCESS ||
        is_macro_declared(p, _token->cps, _token->len) == FAIL
    ) do_expand = 0;

    if(p->is_inside_macro_decl) DBG(DO_MACRO_NOT_EXPANDING_DBG, "Not expanding tokens\n");

    // Look for a call
    if(do_expand) {
        struct token *lpar;
        if((lpar = _advance(p, re)) == NULL) return FAIL;
        if(lpar->type != LPAR) do_expand = 0;
    }

    // No expansion: return the tokens read and where to replace it
    if(!do_expand) return SUCCESS;

    // Get macro
    struct Macro* macro = get_macro(p, _token->cps, _token->len);
    DBG(DO_MACRO_EXPANSION_DBG, "Expanding macro '");
    for(unint i=0; i<macro->name.len; ++i) DBG_CP(DO_MACRO_EXPANSION_DBG, macro->name.cps[i]);
    DBG(DO_MACRO_EXPANSION_DBG, "' (%d tokens)\n", macro->tok_len);

    // Limit check
    if(++p->macro_expansion_count > MAX_MACRO_EXPANSION_LIMIT) {
        _error_from_token(p, _token, ERROR_TYPE_MACRO_LIMIT, "reached macro expansion limit: %d\n", MAX_MACRO_EXPANSION_LIMIT);
        return FAIL;
    }

    // Parse macro declaration args
    unint is_variadic = 0;
    unint left_args_count = 0;
    unint right_args_count = 0;
    unint min_args_count = 0;
 
    for (struct MacroArg* m_arg = macro->args; m_arg; m_arg = m_arg->next) {
        if(m_arg->is_variadic) {
            if (is_variadic) {
                (void)0; // ERROR: multiple variadic arguments
            }
            is_variadic = 1;
            continue;
        }
 
        if (!is_variadic) ++left_args_count;
        else ++right_args_count;
    }
    min_args_count = left_args_count + right_args_count;
    
    // Parse call arguments
    struct MacroCallArgs* ca = NULL;
    struct MacroCallArgs* ca_tail = NULL;

    unint call_args_count = 0;
    unint ca_alloc_error = 0;
    while(true) {
        struct token *ca_start, *ca_end, *term;
        term = _parse_macro_call_arg_until_comma_or_macro_call_end(p, &ca_start, &ca_end, re);
        if(term == NULL) return FAIL;

        // Empty call
        if((!call_args_count && !ca_start && !ca_end && term->type == RPAR)) {
            // No args decl
            if(min_args_count == 0 && !is_variadic) break;

            // For every arg declared allocate one call arg
            ca = (struct MacroCallArgs*)MEM_ALLOC(sizeof(struct MacroCallArgs) * (min_args_count + is_variadic), "macro call arg struct");
            if(ca == NULL) { ca_alloc_error = 1; break; }

            call_args_count = (min_args_count + is_variadic);

            // Link call args
            for(unint i=0; i<call_args_count-1; ++i) {
                ca[i].head = ca[i].tail = NULL;
                ca[i].next = &ca[i+1];
                ca[i].terminator = term;
            }
            ca[call_args_count-1].head = NULL;
            ca[call_args_count-1].tail = NULL;
            ca[call_args_count-1].next = NULL;
            ca[call_args_count-1].terminator = term;

            ca_tail = &ca[call_args_count-1];
            break;
        }

        struct MacroCallArgs* _ca = (struct MacroCallArgs*)MEM_ALLOC(sizeof(struct MacroCallArgs));

        _ca->head = ca_start;
        _ca->tail = ca_end;
        _ca->next = NULL;
        _ca->terminator = term;

        // First Call arg
        if(ca == NULL) ca = _ca;

        // Link the new call arg
        if(ca_tail != NULL) ca_tail->next = _ca;
        ca_tail = _ca;
        
        ++call_args_count;

        if(term->type == RPAR) break;
    }

    if(ca_alloc_error) {
        memory_error(p, "no available memory for the macro call argument structure"); 
        return FAIL;
    }
    DBG(DO_MACRO_CALL_ARGS_DBG, "Allocated %d call args\n", call_args_count);


    // Call argument count vs Arg decl count
    if (call_args_count < min_args_count) {
        // Too little args
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "missing arguments for macro call\n");
        return FAIL;
    }
    else if(!is_variadic && call_args_count > min_args_count) {
        // Too many
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "too many arguments for macro call\n");
        return FAIL;
    }

    // DBG call args
    if(ca == NULL) DBG(DO_MACRO_CALL_ARGS_DBG, "NO call args\n");
    else {
        DBG(DO_MACRO_CALL_ARGS_DBG, "Calling macro with [\n");

        struct MacroCallArgs* _ca = ca;
        while(true) {
            DBG(DO_MACRO_CALL_ARGS_DBG, "  ");
            struct token* _tk = _ca->head;
            if(_tk == NULL) DBG(DO_MACRO_CALL_ARGS_DBG, "<NULL>");
            else {
                while(true) {
                    DBG(DO_MACRO_CALL_ARGS_DBG, "[%s] ", _Parser_TokenNames[_tk->type]);
                    if(_tk == _ca->tail) break;
                    _tk = _tk->next;
                }
            }
            DBG(DO_MACRO_CALL_ARGS_DBG, "\n");
            if(_ca == ca_tail) break;
            _ca = _ca->next;
        }
    }
    DBG(DO_MACRO_CALL_ARGS_DBG, "]\n");

    // Va arguments
    struct MacroCallArgs* va_start = NULL;
    struct MacroCallArgs* va_end = NULL;

    if(is_variadic && call_args_count > min_args_count) {
        // Skip left args
        va_end = va_start = ca;
        for(unint i=0; i<left_args_count; ++i) va_start = va_start->next;
        
        // End
        for(unint i=0; i<(call_args_count-right_args_count)-1; ++i) va_end = va_end->next;
    }

    if(va_start == NULL || va_end == NULL) DBG(DO_MACRO_CALL_ARGS_DBG, "No var args\n");
    else {
        DBG(DO_MACRO_CALL_ARGS_DBG, "Var args [\n");

        struct MacroCallArgs* _ca = va_start;
        while(true) {
            DBG(DO_MACRO_CALL_ARGS_DBG, "  ");
            struct token* _tk = _ca->head;
            if(_tk == NULL) DBG(DO_MACRO_CALL_ARGS_DBG, "<NULL>");
            else {
                while(true) {
                    DBG(DO_MACRO_CALL_ARGS_DBG, "[%s] ", _Parser_TokenNames[_tk->type]);
                    if(_tk == _ca->tail) break;
                    _tk = _tk->next;
                }
            }
            DBG(DO_MACRO_CALL_ARGS_DBG, "\n");
            if(_ca == va_end) break;
            _ca = _ca->next;
        }
    }
    DBG(DO_MACRO_CALL_ARGS_DBG, "]\n");

    // Expand the macro tokens

    // Empty expansion
    if(macro->tok_len == 0) {
        // Deallocate the tokens from start to end
        *start = NULL; *end = NULL;
        return SUCCESS;
    }

    struct MacroTrace* macro_trace = (struct MacroTrace*)MEM_ALLOC(sizeof(struct MacroTrace), "macro trace");
    if(macro_trace == NULL) {
        *end = *start = NULL;
        memory_error(p, "no available memory for macro trace structure"); 
        return FAIL;
    }
    
    macro_trace->parent = (mt == NULL) ? NULL : mt;
    macro_trace->macro = macro;
    macro_trace->lineno = _token->lineno;

    unint extra_macro_arg_tokens = 0;
    unint args_in_macro_body = 0;

    struct token* mtoken = macro->tokens;
    for(unint i=0; i<macro->tok_len; ++i, mtoken = mtoken->next) {
        // Check if the token is an arg
        if(mtoken->type == NAME) {
            struct MacroArg* ma_args = macro->args;
            struct MacroCallArgs* _ca = ca;
            unint j=0;
            while(j<macro->arg_len && _compare_identifiers(mtoken->cps, mtoken->len, ma_args->arg_name.cps, ma_args->arg_name.len) == FAIL) {
                // Iterator code
                if(!ma_args->is_variadic) _ca = _ca->next;
                else {
                    
                    if(va_end) _ca = va_end->next;
                }

                ma_args = ma_args->next;
                ++j;
            }

            // Could not find any maching arg decl
            if(ma_args == NULL) continue;

            ++args_in_macro_body;

            if(!ma_args->is_variadic) {
                unint t = 0;
                if(_ca->head) {
                    struct token* _t = _ca->head;
                    while(true) {
                        ++t;
                        if(_t == _ca->tail) break;
                        _t = _t->next;
                    }
                }
                extra_macro_arg_tokens += t;
            } else {
                if(va_start && va_end) {
                    // if not the last add also a comma
                    struct MacroCallArgs* _ca = va_start;
                    while(true) {

                        unint t = 0;
                        if(_ca->head) {
                            struct token* _t = _ca->head;
                            while(true) {
                                ++t;
                                if(_t == _ca->tail) break;
                                _t = _t->next;
                            }
                        }
                        extra_macro_arg_tokens += t;

                        // Comma
                        if(_ca != va_end) extra_macro_arg_tokens += 1;

                        if(_ca == va_end) break;
                        _ca = _ca->next;
                    }
                }
            }
        }
    }

    DBG(DO_MACRO_TOTAL_TOKEN_COUNT_DBG, "Adding %d extra tokens to macro alloc\nRemoving %d tokens\n", extra_macro_arg_tokens, args_in_macro_body);
    unint total_macro_tokens = macro->tok_len + extra_macro_arg_tokens - args_in_macro_body;
    DBG(DO_MACRO_TOTAL_TOKEN_COUNT_DBG, "total_macro_tokens = %d\n", total_macro_tokens);

    if(total_macro_tokens == 0) { // the body had an argument that was called with nothing
        *start = NULL; *end = NULL;
        return SUCCESS;
    }

    // Allocate the macro tokens
    struct token* mcopy = (struct token*)MEM_ALLOC(sizeof(struct token) * total_macro_tokens, "macro body copy");
    if(mcopy == NULL) {
        *end = *start = NULL;
        memory_error(p, "no available memory to copy the macro body"); 
        return FAIL;
    }


    // Link & macro-trace
    mtoken = macro->tokens;
    for(unint i=0; i<total_macro_tokens-1; ++i) {
        mcopy[i].next = &mcopy[i+1];
        mcopy[i].macro_trace = macro_trace;
    }
    mcopy[total_macro_tokens-1].next = NULL;
    mcopy[total_macro_tokens-1].macro_trace = macro_trace;

    // Copy
    struct token* cursor = mcopy;
    mtoken = macro->tokens;
    for(unint i=0; i<macro->tok_len; ++i, mtoken = mtoken->next) {
        if(mtoken->type == NAME) {
            struct MacroArg* ma_args = macro->args;
            struct MacroCallArgs* _ca = ca;
            unint j=0;
            while(j<macro->arg_len && _compare_identifiers(mtoken->cps, mtoken->len, ma_args->arg_name.cps, ma_args->arg_name.len) == FAIL) {
                // Iterator code
                if(!ma_args->is_variadic) _ca = _ca->next;
                else {
                    if(va_end) _ca = va_end->next;
                }

                ma_args = ma_args->next;
                ++j;
            }

            // Could not find any maching arg decl
            if(ma_args == NULL) goto copy_macro_body_token;          

            // Var arg
            if(ma_args->is_variadic && (va_start != NULL && va_end != NULL)) {

                struct MacroCallArgs* __ca = va_start;
                while(true) {
                    if(__ca->head != NULL && __ca->tail != NULL) {
                        struct token* _t = __ca->head;
                        while(true) {
                            struct token* _tmp = cursor->next;
                            struct MacroTrace* _mt = cursor->macro_trace;
                            *cursor = *_t;
                            cursor->macro_trace = _mt;
                            cursor = (cursor->next = _tmp);
                            if(_t == __ca->tail) break;
                            _t = _t->next;
                        }
                    }

                    if(__ca != va_end) { // Place the terminator
                        struct token* _tmp = cursor->next;
                        struct MacroTrace* _mt = cursor->macro_trace;
                        *cursor = *(__ca->terminator);
                        cursor->macro_trace = _mt;
                        cursor = (cursor->next = _tmp);
                    }

                    if(__ca == va_end) break;
                    __ca = __ca->next;
                }
                // if va_start->head -> place tokens
                
            } 
            
            // Normal arg
            else if(!ma_args->is_variadic && (_ca->head != NULL && _ca->tail != NULL)) {
                struct token* _t = _ca->head;
                while(true) {
                    struct token* _tmp = cursor->next;
                    struct MacroTrace* _mt = cursor->macro_trace;
                    *cursor = *_t;
                    cursor->macro_trace = _mt;
                    cursor = (cursor->next = _tmp);
                    if(_t == _ca->tail) break;
                    _t = _t->next;
                }
            }
        } else {
copy_macro_body_token:
            // Copy
            struct token* _tmp = cursor->next;
            struct MacroTrace* _mt = cursor->macro_trace;
            *cursor = *mtoken;
            cursor->macro_trace = _mt;
            cursor = (cursor->next = _tmp);
        }
    }
    
    *start = mcopy;
    *end = &mcopy[total_macro_tokens-1];
    
    DBG(DO_MACRO_SANITY_CHECK_DBG, "SANITY CHECK\n  ");
    struct token* _t = *start;
    while(true) {
        DBG(DO_MACRO_SANITY_CHECK_DBG, "[%p:%s:l%d:c%d] ", _t, _Parser_TokenNames[_t->type], _t->lineno,_t->col_offset);
        if(_t == *end) break;
        _t = _t->next;
    }
    DBG(DO_MACRO_SANITY_CHECK_DBG,"\n");

    return SUCCESS;
}
// Common(used by _read_token and _peek_token) function used to walk and expand macros
// Works with p->tail
unint _fill_and_expand_macros(struct Parser* p) {

    // [re == where to start reading (NULL)] _read(p, &s, &e, macro_trace, &re)
    // s != NULL && (s == e) && s == *re: one token (tokens to replace)
    // s == NULL && e == NULL: empty expansion (*re == end token of macro call)
    // s != NULL && e != NULL && s != *re: macro expansion (*re == end token of macro call)
    // ... : never

    struct token** re = &p->tail;
    struct token* cursor;
    while(true) {
        struct MacroTrace* mt = NULL;
        if (*re) mt = (*re)->macro_trace;
        struct token *s, *e;
        struct token* rs = *re; // Replace start

        if(_read(p, &s, &e, re, mt) == FAIL) return FAIL;

        // When its the 1st time reading tokens p->head will start at s
        if(p->head == NULL) p->head = s;

        // Macro expansion token
        if(s != NULL && e != NULL && s != *re) {

            // We assume that rs != NULL bc the first token ever read cant be a macro
            rs->next = s;

            if((*re)->next == NULL) p->tail = e;
            else e->next = (*re)->next;

            cursor = rs;
            re = &cursor;

            continue;
        }

        // Empty macro expansion
        else if(s == NULL && e == NULL) {

            // We assume that rs != NULL bc the first token ever read cant be a macro
            //DBG(1, "(*re)->next = %p", (*re)->next);
            rs->next = (*re)->next;
            if((*re)->next == NULL) {
                re = &p->tail;
                p->tail = rs;
            }
            continue;
        }

        if(re != &p->tail && *re == p->tail) {
            DBG(1, "Recovering tail\n");
            re = &p->tail;
        }

        if(*re == p->tail) break;
    } 
    // DBG(1, "[READ Token] re = %p | &p->tail = %p | p->tail = [%s:l%d:c%d] --------------------------------\n", re, &p->tail, _Parser_TokenNames[p->tail->type], p->tail->lineno, p->tail->col_offset);

    return SUCCESS;
}

// Read / Peek

struct token* _read_token_impl(struct Parser* p) {
    // Advance if we already have tokens
    if (p->last_token && p->last_token != p->tail)
        return (p->peek = p->last_token = p->last_token->next);

    // Ensure we have tokens
    if (!p->last_token) {
        if (!p->head && _fill_and_expand_macros(p) == FAIL)
            return NULL;
        return (p->peek = p->last_token = p->head);
    }

    // We're at tail, try to extend
    if (_fill_and_expand_macros(p) == FAIL)
        return NULL;

    return (p->peek = p->last_token = p->last_token->next);
}

struct token* _read_token(struct Parser* p) {
    struct token* token = _read_token_impl(p);
    if(token == NULL) return NULL;

    const char* start = token->start;
    const char* end = token->end;
    unint size = end - start;
    (void)size;

    DBG(DO_PARSER_READ_TOKEN_DBG, "Read: [%s] (trace: %p): %d bytes, line:%d col:%d-%d '", _Parser_TokenNames[token->type], token->macro_trace, size, token->lineno, token->col_offset, token->end_col_offset);
    if(start == NULL || end == NULL) DBG(DO_PARSER_READ_TOKEN_DBG, "<NULL>");
    else for(const char* i=start; i<end; ++i) DBG(DO_PARSER_READ_TOKEN_DBG, "%c", *i);
    DBG(DO_PARSER_READ_TOKEN_DBG, "'\n\n");

    return token;
}

struct token* _peek_token_impl(struct Parser* p) {
    // Advance if we already have tokens
    if (p->peek && p->peek != p->tail)
        return (p->peek = p->peek->next);

    // Ensure we have tokens
    if (!p->peek) {
        if (!p->head && _fill_and_expand_macros(p) == FAIL)
            return NULL;
        return (p->peek = p->head);
    }

    // We're at tail, try to extend
    if (_fill_and_expand_macros(p) == FAIL)
        return NULL;

    return (p->peek = p->peek->next);
}

struct token* _peek_token(struct Parser* p) {
    struct token* token = _peek_token_impl(p);
    if(token == NULL) return NULL;

    const char* start = token->start;
    const char* end = token->end;
    unint size = end - start;
    (void)size;

    DBG(DO_PARSER_PEEK_TOKEN_DBG, "peek: [%s] (trace: %p): %d bytes, col:%d-%d '", _Parser_TokenNames[token->type], token->macro_trace, size, token->col_offset, token->end_col_offset);
    if(start == NULL || end == NULL) DBG(DO_PARSER_PEEK_TOKEN_DBG, "<NULL>");
    else for(const char* i=start; i<end; ++i) DBG(DO_PARSER_PEEK_TOKEN_DBG, "%c", *i);
    DBG(DO_PARSER_PEEK_TOKEN_DBG, "'\n");

    return token;
}

void _reset_peek(struct Parser* p) {
    p->peek = p->last_token;
}

// Parser

struct Parser* _Parser_New(struct tok_state* tok) {
    struct Parser* p = MEM_ALLOC(sizeof(struct Parser), "parser");
    if(p == NULL) return NULL;

    p->tok = tok;
    p->variables = p->variables_tail = NULL;
    p->head = p->tokens = p->last_token = p->peek = p->tail = NULL;
    p->macros = p->macros_tail = NULL;
    p->macro_expansion_count = p->expanded_macro_is_blank = p->is_inside_macro_decl = 0;

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

struct token* expected_token(struct Parser *p, struct token* _token,  unint token) {
    if(_token->type == NAME && token == AT_IDENTIFIER_TOKEN) {
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected at-identifier");
        return NULL;
    } else if(is_at_identifier(_token, NULL) == SUCCESS && token == NAME) {
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected non-at-identifier"); 
        return NULL;      
    } else if(_token->type != token){
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected '%s'", _Parser_TokenSymbols[token]);
        return NULL;
    }

    return _token;
}

struct token* expect_token(struct Parser *p, unint token) {
    struct token* _token = _read_token(p);
    if(_token == NULL) return NULL;

    return expected_token(p, _token, token);
}


// Pratt Parsing
#define UNARY_BP 120
#define INDEX_BP 130

unint _expr_get_binding_power(unint type, unint* lbp, unint* rbp) {
    switch(type) {
        case DOUBLEVBAR: { *lbp = 10; *rbp = 11; break; } // Done
        case DOUBLEAMPER:  { *lbp = 20; *rbp = 21; break; } // Done

        case NOTEQUAL: // Done
        case GREATER: // Done
        case GREATEREQUAL:
        case EQEQUAL:
        case LESS:
        case LESSEQUAL: { *lbp = 30; *rbp = 31; break; }

        case VBAR: { *lbp = 40; *rbp = 41; break; } // Done
        case AMPER: { *lbp = 60; *rbp = 61; break; } // Done

        case LEFTSHIFT:
        case RIGHTSHIFT: { *lbp = 70; *rbp = 71; break; } // Done

        case DOT: { *lbp = 90; *rbp = 80; break; } // Done

        case PLUS:
        case MINUS: { *lbp = 100; *rbp = 101; break; } // Done

        case SLASH:
        case PERCENT:
        case STAR: { *lbp = 110; *rbp = 111; break; } // Done

        default : return FAIL;
    }
    return SUCCESS;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_bp, unint stop_on_comma);
unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val);

// Will return NULL or an AST
struct Ast_node* _parse_expr_prefix(struct Parser* p, unint stop_on_comma) {
    // Request a token
    struct token* _token = _read_token(p);
    if(_token == NULL) return NULL;

    switch(_token->type) {
        case LPAR:
            struct Ast_node* left = _parse_expr(p, 0, 0); // Do not allow commas
            if(left == NULL) return NULL;

            struct token * rpar = _read_token(p);
            if(rpar == NULL) return NULL;

            if(rpar->type != RPAR) return NULL;

            return left;
        case STRING:
            DBG(1, "Expr pref is string\n");
            return new_ast_string(p, _token);
        case NUMBER:
            DBG(1, "Expr pref is number\n");
            return new_ast_number(p, _token, 0);
        case MINUS:
            // might be a negation or a negative number
            struct token* _peek = _peek_token(p);
            if(_peek == NULL) return NULL;

            // Handle the number
            if(_peek->type == NUMBER) {
                DBG(1, "Expr pref is negative number\n");

                // Eat the number
                _token = _read_token(p);
                if(_token == NULL) return NULL;

                return new_ast_number(p, _token, 1);
            }

            // Reset the peek and treat the token as unary
            _reset_peek(p);
        case PLUS:
        case TILDE:
        case EXCLAMATION:
            /*
                It doesnt really matter to pass stop_on_comma or 0 to the _parse_expr as the expression being parsed is just one token
                and expression_prefix does not allow ',' so ...
            */
            struct Ast_node* node = _parse_expr(p, UNARY_BP, stop_on_comma);
            if(node == NULL) return NULL;

            return new_ast_binop(p, _token, _token->type, node, NULL);

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

            return new_ast_variable(p, _token, var);
        case LSQB:
            if((_token = _peek_token(p)) == NULL) return NULL;
            
            if(_token->type == COMMA) {
                _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected a value before the comma");
                return NULL;
            }
            _reset_peek(p); // Reset so it does not consume any valid start

            // Create Array
            struct Value* arr = new_array(p, _token);
            if(arr == NULL) return NULL;

            while(true) {
                // Finished
                if(_token->type == RSQB) break;

                // parse the expression
                struct Ast_node* el = _parse_expr(p, 0, 1); // Do stop on commas
                if(el == NULL) return NULL;

                // Eval the expression
                struct Value val;
                unint suc = _eval_expr(p, el, &val);
                if(suc == FAIL) return NULL;

                // Append to the array
                if(append_array(p, _token, arr, &val) == FAIL) return NULL;

                // Read possibly the next comma or ]
                if((_token = _read_token(p)) == NULL) return NULL;
                if(_token->type == COMMA) {
                    struct token* rsqb = _peek_token(p);
                    if(rsqb == NULL) return NULL;

                    if(rsqb->type == RSQB) {
                        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected a value after the comma");
                        return NULL;
                    }
                    _reset_peek(p);
                }
            }

            return new_ast_array(p, _token, arr);
        default:
            _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "invalid syntax for expression");
            return NULL;
    }
    // Just in case
    return NULL;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_bp, unint stop_on_comma) {
    // Prefix
    // On an array, _parse_expr will never be called when _parse_expr_prefix is a comma, no need to do checks
    struct Ast_node* left = _parse_expr_prefix(p, stop_on_comma);
    if(left == NULL) return NULL;

    unint lbp, rbp;
    while(true) {
        struct token* op = _peek_token(p);
        if(op == NULL) return NULL;

        if(op->type == ENDMARKER || op->type == NEWLINE || op->type == RPAR || op->type == RSQB || (stop_on_comma && op->type == COMMA)) break;

        // Postfix operator
        if(op->type == LSQB) {

            if(_read_token(p) == NULL) return NULL;
            
            struct Ast_node* right = _parse_expr(p, 0, 0); // Do not allow commas
            if(right == NULL) return NULL;

            struct token * rsqb = _read_token(p);
            if(rsqb == NULL) return NULL;
            if(rsqb->type != RSQB) return NULL;

            left = new_ast_binop(p, op, LSQB, left, right);
            continue;
        }

        
        unint suc = _expr_get_binding_power(op->type, &lbp, &rbp);
        if(suc == FAIL) {
            _error_from_token(p, op, ERROR_TYPE_EXPRESSION, "invalid operator in expression");
            return NULL;
        }

        if(lbp < min_bp) break;

        op = _read_token(p);
        if(op == NULL) return NULL;

        struct Ast_node* right = _parse_expr(p, rbp, stop_on_comma); // Propagate the setting used to the other nodes
        if(right == NULL) return NULL;

        // build the AST node
        left = new_ast_binop(p, op, op->type, left, right);
        if(left == NULL) return NULL;
    }

    // reset the peek so it doesnt keep peeking forward
    _reset_peek(p);
    return left;
}

unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val);

unint _type_check(struct Parser* p, struct AstBinOp* binop, struct Value* vleft, struct Value* vright, unint tcleft, unint tcright) {
    struct token* op_token = binop->op_token;
    unint op = binop->op;

    // Type checks

    // Concatnation only allowed on Strings/characters
    if(op == DOT && ( 
        (tcleft && (vleft->type != VALUE_STR)) ||
        (tcright && (vright->type != VALUE_STR))
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "can only perform concatnation on strings types");
        return FAIL;
    }

    // Arithmetic only on ints/floats 
    if ((op == PLUS || op == MINUS || op == SLASH || op == STAR || op == PERCENT) && ( 
        (tcleft && (vleft->type != VALUE_INT && vleft->type != VALUE_DOUBLE)) ||
        (tcright && (vright->type != VALUE_INT && vright->type != VALUE_DOUBLE)) 
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "can only perform arithmetic operations on integer/decimal types");
        return FAIL;
    }
    DBG(1, "[TYPE_CHECK]: tcleft = %d | tcright = %d\n", tcleft, tcright);
    // Bitwise operators only on ints
    if((op == LEFTSHIFT || op == RIGHTSHIFT || op == AMPER || op == VBAR || op == CIRCUMFLEX || op == TILDE) && (
        (tcleft && vleft->type != VALUE_INT) ||
        (tcright && vright->type != VALUE_INT)
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "can only perform bitwise operations on integer types");
        return FAIL;
    }

    // Relational operators

    // >= > <= < only to integers/floats
    if((op == GREATER || op == GREATEREQUAL || op == LESS || op == LESSEQUAL) && (
        (tcleft && vleft->type != VALUE_INT && vleft->type != VALUE_DOUBLE) ||
        (tcright && vright->type != VALUE_INT && vright->type != VALUE_DOUBLE)
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_TYPE, "can only perform comparison on integer/double types");
        return FAIL;
    }
    
    // Indexation only on array types or strings
    if(op == LSQB && 
        (tcleft && vleft->type != VALUE_ARRAY && vleft->type != VALUE_STR)
    ) {
        _error_from_token(p, op_token, ERROR_TYPE_TYPE, "can only perform indexation on array/string types");
        return FAIL;
    }

    // Index only with integer
    if(op == LSQB &&
        (tcright && vright->type != VALUE_INT)
    ) {
        _error_from_token(p, op_token, ERROR_TYPE_TYPE, "indices can only by of integer type");
        return FAIL;
    }
    
    return SUCCESS;
}


unint bool_eval(struct Value* val) {
    switch(val->type) {
        case VALUE_INT:
            return !!val->val.number;
        case VALUE_STR:
            return !!val->val.string.len;
        case VALUE_DOUBLE:
            return !!val->val.flt;
        case VALUE_ARRAY:
            return !!val->val.arr.len;
        default:
            return 0;
    }
}

unint _equality_check(struct Value* vleft, unint op, struct Value* vright) {
    // except for int and floats, if both types dont match, return false(==), or true(!=)
    if((vleft->type == VALUE_DOUBLE || vleft->type == VALUE_INT) &&
        (vright->type == VALUE_DOUBLE || vright->type == VALUE_INT)
    ) {
        // if one is float, promote all to float
        if(vleft->type == VALUE_DOUBLE || vright->type == VALUE_DOUBLE) {

            double da = (vleft->type == VALUE_DOUBLE) ? vleft->val.flt : (double)vleft->val.number;
            double db = (vright->type == VALUE_DOUBLE) ? vright->val.flt : (double)vright->val.number;

            return (op == EQEQUAL) ? (nint)(da == db) : (nint)(da != db);;
        }

        nint a = vleft->val.number;
        nint b = vright->val.number;

        return (op == EQEQUAL) ? (nint)(a == b) : (nint)(a != b);

    }

    // Do types miss-match?
    if(vleft->type != vright->type) return !(op == EQEQUAL);

    // types left: string == string and array == array
    unint alen;
    unint blen;

    if(vleft->type == VALUE_STR) {
        alen = vleft->val.string.len;
        blen = vright->val.string.len;

        if(alen != blen) return !(op == EQEQUAL);

        for(unint i=0; i<alen; ++i) if(vleft->val.string.str[i] != vright->val.string.str[i]) return !(op == EQEQUAL);

        // Strings match
        return (op == EQEQUAL);
    }

    // Array:
    alen = vleft->val.arr.len;
    blen = vright->val.arr.len;

    if(alen != blen) return !(op == EQEQUAL);

    struct ArrayElement* tleft = vleft->val.arr.head;
    struct ArrayElement* tright = vright->val.arr.head;
    while(tleft != NULL) {
        if(_equality_check(&tleft->this, NOTEQUAL, &tright->this)) return !(op == EQEQUAL);
        tleft = tleft->next;
        tright = tright->next;
    }

    // Arrays match
    return (op == EQEQUAL);
}

unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val) {
    switch(expr->type) {
        case LITERAL_NODE:
            *val = *expr->node.literal.value;
            return SUCCESS;
        case VAR_NODE:
            *val = expr->node.var.val;
            return SUCCESS;
        case BINOP_NODE:
            struct AstBinOp* binop = &expr->node.binop;
            struct Ast_node* pleft = binop->left;
            struct Ast_node* pright = binop->right;
            struct token* op_token = binop->op_token;
            
            struct Value vleft, vright;

            // Early type check
            unint tcleft = 0, tcright = 0;

            if(pleft->type == LITERAL_NODE || pleft->type == VAR_NODE){
                if(_eval_expr(p, pleft, &vleft) == FAIL) return FAIL;
                tcleft = 1;
            }

            if(pright && (pright->type == LITERAL_NODE || pright->type == VAR_NODE)) {
                if(_eval_expr(p, pright, &vright) == FAIL) return FAIL;
                tcright = 1;
            }

            DBG(DO_EXPRESSION_EVAL_TYPE_CHECK_DBG, "Type check #1: tcleft = %d | tcright = %d\n", tcleft, tcright);
            if(_type_check(p, binop, &vleft, &vright, tcleft, tcright) == FAIL) return FAIL;

            // Evaluate
            if(_eval_expr(p, pleft, &vleft) == FAIL) return FAIL;

            DBG(DO_EXPRESSION_EVAL_TYPE_CHECK_DBG, "Type check #2: tcleft = %d | tcright = %d\n", 1, 0);
            if(_type_check(p, binop, &vleft, &vright, 1, 0) == FAIL) return FAIL;

            // May not exist depending on the operator
            if(pright && _eval_expr(p, pright, &vright) == FAIL) return FAIL;

            DBG(DO_EXPRESSION_EVAL_TYPE_CHECK_DBG, "Type check #3: tcleft = %d | tcright = %d\n", 0, 1);
            if(pright && _type_check(p, binop, &vleft, &vright, 0, 1) == FAIL) return FAIL;

            nint a, b;
            double da, db;

            // Eval operators
            switch(binop->op) {
                case PLUS:
                    // unary plus, if right is not present, preserve the original type
                    if(!pright) { *val = vleft; return SUCCESS; }

                    // Convert types
                    if(vleft.type == VALUE_INT && vright.type == VALUE_INT) {
                        val->type = VALUE_INT;
                        // Check for overflows
                        
                        a = vleft.val.number;
                        b = vright.val.number;

                        // Overflow?
                        if ((b > 0 && a > NINT_MAX - b) ||
                            (b < 0 && a < NINT_MIN - b)) {
                            _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in addition");
                            return FAIL;
                        }

                        val->type = VALUE_INT;
                        val->val.number = a + b;
                        return SUCCESS;
                    }

                    // Promote to flt
                    da = (vleft.type == VALUE_INT)  ? (double)vleft.val.number  : vleft.val.flt;
                    db = (vright.type == VALUE_INT) ? (double)vright.val.number : vright.val.flt;

                    val->type = VALUE_DOUBLE;
                    val->val.flt = da + db;
                    return SUCCESS;

                case MINUS:
                    if(vleft.type == VALUE_INT || (vleft.type == VALUE_INT && (expr->node.binop.right && vright.type == VALUE_INT))) {

                        a = expr->node.binop.right ? vleft.val.number : 0;
                        b = expr->node.binop.right ? vright.val.number : vleft.val.number;

                        if ((b < 0 && a > NINT_MAX + b) ||
                            (b > 0 && a < NINT_MIN + b)) {
                            _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in subtraction");
                            return FAIL;
                        }

                        val->type = VALUE_INT;
                        val->val.number = a - b;
                        return SUCCESS;
                    }

                    // Promote to flt
                    da = expr->node.binop.right ? (
                        (vleft.type == VALUE_INT) ?  (double)vleft.val.number : vleft.val.flt
                    ) : (
                        (vleft.type == VALUE_INT) ? 0 : (double)0.0
                    );
                    db = expr->node.binop.right ? (
                        (vright.type == VALUE_INT) ? (double)vright.val.number : vright.val.flt
                    ) : (
                        (vleft.type == VALUE_INT) ?  (double)vleft.val.number : vleft.val.flt
                    );
                    
                    val->type = VALUE_DOUBLE;
                    val->val.flt = da - db;
                    return SUCCESS;
                
                case PERCENT:
                    if( (vright.type == VALUE_DOUBLE && vright.val.flt == (double)0.0) || (vright.type == VALUE_INT && vright.val.number == 0) ) {
                        _error_from_token(p, op_token, ERROR_TYPE_DIVISION_ERROR, "modulo by 0");
                        return FAIL;
                    }
                    // if one is float, promote all to float
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {
                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.flt;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.flt;

                        val->type = VALUE_DOUBLE;
                        val->val.flt = fmod( fmod(da, db) + db, db);
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    val->type = VALUE_INT;
                    val->val.number = ((a % b) + b) % b;
                    return SUCCESS;
                    
                case SLASH:
                    if( (vright.type == VALUE_DOUBLE && vright.val.flt == (double)0.0) || (vright.type == VALUE_INT && vright.val.number == 0) ) {
                        _error_from_token(p, op_token, ERROR_TYPE_DIVISION_ERROR, "division by 0");
                        return FAIL;
                    }
                    // if one is float, promote all to float
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {

                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.number;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.number;

                        val->type = VALUE_DOUBLE;
                        val->val.flt = da / db;
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    if(a == NINT_MIN && b == -1) {
                        _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in division");
                        return FAIL;
                    }
                    val->type = VALUE_INT;
                    val->val.number = a / b;
                    return SUCCESS;

                case STAR:
                    // if one is float, promote all to float
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {

                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.number;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.number;

                        val->type = VALUE_DOUBLE;
                        val->val.flt = da * db;
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    if (a > 0) {
                        if (b > 0) {
                            if (a > NINT_MAX / b) goto mul_overflow;
                        } else {
                            if (b < NINT_MIN / a) goto mul_overflow;
                        }
                    } else {
                        if (b > 0) {
                            if (a < NINT_MIN / b) goto mul_overflow;
                        } else {
                            if (a != 0 && b < NINT_MAX / a) goto mul_overflow;
                        }
                    }

                    val->type = VALUE_INT;
                    val->val.number = a * b;
                    return SUCCESS;
mul_overflow:
                    _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in multiplication");
                    return FAIL;

                case LSQB:
                    // Valid index?
                    nint index = vright.val.number;
                    unint len = (vleft.type == VALUE_ARRAY) ? vleft.val.arr.len : vleft.val.string.len;
                    if(index < 0) index = len + index;

                    if(index < 0 || index >= len) {
                        _error_from_token(p, op_token, ERROR_TYPE_INDEX_ERROR, "index out of range");
                        return FAIL;
                    }

                    if(vleft.type == VALUE_ARRAY) {
                        struct ArrayElement* el = vleft.val.arr.head;
                        for(nint i=0; i<index; ++i) el = el->next;

                        *val = el->this;
                    } else { // String
                        int32_t* cp = MEM_ALLOC(sizeof(int32_t), "eval of string index");
                        if(cp == NULL) {
                            _error_from_token(p, op_token, ERROR_TYPE_MEMORY, "no available memory to index the string");
                            return FAIL;   
                        }

                        *cp = vleft.val.string.str[index];

                        val->type = VALUE_STR;
                        val->val.string.len = 1;
                        val->val.string.str = cp;
                    }

                    return SUCCESS;

                case DOT:
                    struct Value* non_empty_str = NULL;
                    unint alen = vleft.val.string.len;
                    unint blen = vright.val.string.len;
                    if(alen == 0) non_empty_str = &vright;
                    else if(blen == 0) non_empty_str = &vleft;
                    if(non_empty_str) {
                        *val = *non_empty_str;
                        return SUCCESS;
                    }

                    int32_t* cps = MEM_ALLOC((alen + blen) * sizeof(int32_t), "eval of concatnation");
                    if(cps == NULL) {
                        _error_from_token(p, op_token, ERROR_TYPE_MEMORY, "no available memory to concatnate the string");
                        return FAIL;   
                    }

                    // Copy
                    for(unint i=0; i<alen; ++i) cps[i] = vleft.val.string.str[i];
                    for(unint i=0; i<blen; ++i) cps[alen+i] = vright.val.string.str[i];

                    val->type = VALUE_STR;
                    val->val.string.str = cps;
                    val->val.string.len = (alen + blen);

                    return SUCCESS;
                case LEFTSHIFT:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number << vright.val.number;
                    return SUCCESS;
                case RIGHTSHIFT:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number >> vright.val.number;
                    return SUCCESS;

                case AMPER:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number & vright.val.number;
                    return SUCCESS;

                case VBAR:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number | vright.val.number;
                    return SUCCESS;

                case CIRCUMFLEX:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number ^ vright.val.number;
                    return SUCCESS;

                case TILDE:
                    val->type = VALUE_INT;
                    val->val.number = ~vleft.val.number;
                    return SUCCESS;

                case EXCLAMATION:
                    val->type = VALUE_INT;
                    val->val.number = !bool_eval(&vleft);
                    return SUCCESS;
                
                case DOUBLEAMPER:
                    val->type = VALUE_INT;
                    val->val.number = bool_eval(&vleft) && bool_eval(&vright);
                    return SUCCESS;

                case DOUBLEVBAR:
                    val->type = VALUE_INT;
                    val->val.number = bool_eval(&vleft) || bool_eval(&vright);
                    return SUCCESS;

                case NOTEQUAL:
                case EQEQUAL:
                    val->type = VALUE_INT;
                    val->val.number = _equality_check(&vleft, binop->op, &vright);
                    return SUCCESS;

                case GREATER:
                case GREATEREQUAL:
                case LESS:
                case LESSEQUAL:
                    // if one is float, promote all to float
                    val->type = VALUE_INT;
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {

                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.number;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.number;

                        switch(binop->op) {
                            case GREATER:      { val->val.number = da >  db; break; }
                            case GREATEREQUAL: { val->val.number = da >= db; break; }
                            case LESS:         { val->val.number = da <  db; break; }
                            case LESSEQUAL:    { val->val.number = da >= db; break; }
                        }
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    switch(binop->op) {
                        case GREATER:      { val->val.number = a >  b; break; }
                        case GREATEREQUAL: { val->val.number = a >= b; break; }
                        case LESS:         { val->val.number = a <  b; break; }
                        case LESSEQUAL:    { val->val.number = a >= b; break; }
                    }
                    return SUCCESS;

                    

                default:
                    _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "evaluation of that operator hasn't been implemented");
                    return FAIL;
            }
        default:
            return FAIL;  
    }
}

// Macros

struct Macro* get_macro(struct Parser* p, int32_t* cps, unint len) {
    if(p == NULL || p->macros == NULL) return NULL;

    // For now macros must have different names, later they can have repeated names and depending on the arguments a diferent macro is called
    for(struct Macro* macro = p->macros; macro != NULL; macro = macro->next) {
        if(macro->name.len != len) continue;

        unint i;
        for(i = 0; i < len; ++i) if(macro->name.cps[i] != cps[i]) break;
        
        // Full match
        if(i == len) return macro;
    }
    return NULL;
}

unint is_macro_declared(struct Parser* p, int32_t* cps, unint len) {
    return (get_macro(p, cps, len) == NULL) ? FAIL : SUCCESS;
}

struct Macro* new_empty_macro(struct Parser *p, struct token* _token, int32_t *cps, unint len) {
    struct Macro* macro = (struct Macro*)MEM_ALLOC(sizeof(struct Macro), "new macro");
    if(macro == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the macro initial structure");
        return NULL;   
    }

    // Fill(Empty)
    macro->args = NULL;
    macro->name.cps = cps;
    macro->name.len = len;
    macro->tokens = NULL;
    macro->arg_len = 0;
    macro->tok_len = 0;

    // First Macro
    if(p->macros == NULL) p->macros = macro;

    // Link the new variable
    if(p->macros_tail != NULL) p->macros_tail->next = macro;
    p->macros_tail = macro;

    p->macros_tail->next = NULL;

    return macro;
}

unint append_macro_arg(struct Parser* p, struct token* _token, struct Macro* macro, int32_t* cps, unint len, unint is_variadic) {
    struct MacroArg* arg = (struct MacroArg*)MEM_ALLOC(sizeof(struct MacroArg), "macro arg");
    if(arg == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the macro argument declaration");
        return FAIL;
    }
    
    // 1st time
    if(macro->args == NULL) macro->args = arg;

    // Link
    if(macro->args_tail != NULL) macro->args_tail->next = arg;
    macro->args_tail = arg;
    
    // Value
    arg->next = NULL;
    arg->is_variadic = is_variadic;
    arg->arg_name.cps = cps;
    arg->arg_name.len = len;

    // Update Len
    macro->arg_len += 1;
    return SUCCESS;
}

unint is_macro_arg(struct Macro* macro, int32_t* cps, unint len) {
    if(macro == NULL) return FAIL;

    for(struct MacroArg* arg = macro->args; arg != NULL; arg = arg->next) {
        if(arg->arg_name.len != len) continue;

        unint i;
        for(i = 0; i < len; ++i) if(arg->arg_name.cps[i] != cps[i]) break;
        
        // Full match
        if(i == len) return SUCCESS;
    }
    return FAIL; 
}

void print_macro(struct Macro* macro) {
    DBG(1, "@macro ");
    for(unint i=0; i<macro->name.len; ++i) DBG_CP(1, macro->name.cps[i]);
    DBG(1, " (");
    struct MacroArg* arg = macro->args;
    while(arg != NULL) {
        DBG(1, "\n\t");
        if(arg->is_variadic) DBG(1, "*");
        for(unint i=0; i<arg->arg_name.len; ++i) DBG_CP(1, arg->arg_name.cps[i]);
        arg = arg->next;
    }
    DBG(1, "\n) [ ");

    struct token* tok = macro->tokens;
    for(unint i=0; i<macro->tok_len; ++i) {
        DBG(1, "%s ", _Parser_TokenNames[tok->type]);
        tok = tok->next;
    }
    DBG(1, "]\n");

}

void* _run_parser(struct Parser* p) {

    struct token* _token;
    void* ast = NULL;
    unint suc = 0;

    // Create the first AST node
    if(new_ast(p) == FAIL) {
        memory_error(p, "No available memory for AST's root node");
        return NULL;
    }

    struct Variable* var = new_variable(p, (int32_t[]){'h','e','l','l','o','_','w','o','r','l','d'}, 11, &(struct Value){
        .type = VALUE_INT,
        .val.number = 0x10
    });

    if(var == NULL) {
        DBG(1, "Error building variable\n");
        return NULL;
    }

    (is_variable_declared(p, (int32_t[]){'h', 'i'}, 2) == SUCCESS) ?
        DBG(1, "Variable is declared\n") :
        DBG(1, "Variable is NOT declared\n");

    struct Ast_node* expr = _parse_expr(p, (unint)(0), 0);
    if(expr == NULL) {
        DBG(1, "Error building expression\n");
        return NULL;
    }
    
    DBG(1, "#################################\n");
    dbg_ast(expr);

    DBG(1, "#################################\n");    

    struct Value out;
    suc = _eval_expr(p, expr, &out);
    if(suc == FAIL) {
        DBG(1, "Error evaluating expression\n");
        return NULL;
    }

    DBG(1, "*********************************\n");
    print_value(&out);
    DBG(1, "*********************************\n");   

    while(true) {

        // Skip comments && new lines
        while((_token = _read_token(p)) != NULL && (_token->type == COMMENT || _token->type == NEWLINE));
        if(_token == NULL) return NULL;

        DBG(1, "DONE SKIPING (using last)\n");

        // Macro declaration
        if(is_at_identifier(_token, MACRO_IDENTIDIER) == SUCCESS) {
            struct token* macro_name_token;

            // @macro
            if((macro_name_token = expect_token(p, NAME)) == NULL) return NULL;

            // '('
            if(expect_token(p, LPAR) == NULL) return NULL;

            // Already declared?
            if(is_macro_declared(p, macro_name_token->cps, macro_name_token->len) == SUCCESS) {
                _error_from_token(p, macro_name_token, ERROR_TYPE_DECLARATION, "macro is already declared");
                return NULL;
            }

            // Variable has the same name?
            if(is_variable_declared(p, macro_name_token->cps, macro_name_token->len) == SUCCESS) {
                _error_from_token(p, macro_name_token, ERROR_TYPE_DECLARATION, "macro name is already in use by a variable");
                return NULL;
            }

            // [AST] Init, create macro structure
            struct Ast_node* macro_ast = new_ast_macro_decl(p, macro_name_token, macro_name_token->cps, macro_name_token->len);
            if(macro_ast == NULL) return NULL;
            
            // Insert the macro on the tree

            // [PARSER] Init/Append macro
            struct Macro* macro = new_empty_macro(p, macro_name_token, macro_name_token->cps, macro_name_token->len);
            if(macro == NULL) return NULL;

            // Fill with the arguments
            struct token* arg_token;
            unint has_variadic_arg = 0;
            unint is_variadic_arg = 0;
            unint read_macro_args = 1;

            // Easy end?
            arg_token = _peek_token(p);
            if(arg_token == NULL) return NULL;

            if(arg_token->type == RPAR) read_macro_args = 0;
            else _reset_peek(p);

            while(read_macro_args) {
                DBG(1, "READING ARGS");
                is_variadic_arg = 0;
                arg_token = _peek_token(p);
                if(arg_token == NULL) return NULL;

                if(arg_token->type == STAR) {
                    DBG(1, "READ THE STAR\n");
                    _read_token(p); // Consume the star
                    if(has_variadic_arg) {
                        _error_from_token(p, arg_token, ERROR_TYPE_MESSAGE, "macros can only have one variadic argument");
                        return NULL;
                    } else has_variadic_arg = 1;
                    
                    is_variadic_arg = 1;
                    if((arg_token = expect_token(p, NAME)) == NULL) return NULL;
                } else {
                    if((arg_token = expect_token(p, NAME)) == NULL) return NULL;
                }

                // DO not allow repeated names
                if(is_macro_arg(macro, arg_token->cps, arg_token->len) == SUCCESS) {
                    _error_from_token(p, arg_token, ERROR_TYPE_MESSAGE, "duplicate argument in macro declaration");
                    return NULL;     
                }
                
                // Store the arg name
                if(append_macro_arg(p, arg_token, macro, arg_token->cps, arg_token->len, is_variadic_arg) == FAIL) return NULL;
                
                // Commas
                struct token* comma_token = _peek_token(p);
                if(comma_token == NULL) return NULL;
                _reset_peek(p);
                // If we are not ending the macro, expect a comma
                if(comma_token->type == RPAR) break;

                if(expect_token(p, COMMA) == NULL) return NULL;
            }

            if(expect_token(p, RPAR) == NULL) return NULL;

            // Read the new line, or expect it?
            if(expect_token(p, NEWLINE) == NULL) return NULL;
            // if(_read_token(p) == NULL) return NULL;

            p->is_inside_macro_decl = 1;


            // peek the indentation token
            struct token* indentation = _peek_token(p);
            if(indentation == NULL) return NULL;

            if(indentation->type == INDENT) {
                _read_token(p); // Consume indentation
                // Store the begining of the macro, skiping the first indent
                macro->tokens = _peek_token(p);
                if(macro->tokens == NULL) return NULL;

                unint level = 1;
                // Read tokens until indentation is back to the start
                while(level != 0) {
                    indentation = _read_token(p);
                    if(indentation == NULL) return NULL;

                    if(indentation->type == INDENT) ++level;
                    else if(indentation->type == DEDENT) --level;
                    ++macro->tok_len;
                }
                macro->tok_len -= 2; // Remove the dedent and the last newline

            }

            p->is_inside_macro_decl = 0;

            DBG(1, "-----------------------------------------------------------------\n");
            print_macro(macro);
            DBG(1, "-----------------------------------------------------------------\n");
        }
        else if(is_at_identifier(_token, ASSERT_IDENTIFIER) == SUCCESS) {
            _reset_peek(p);           
            struct token* _s = _peek_token(p);
            if(_s == NULL) return NULL;

            // Expr
            struct Ast_node* expr = _parse_expr(p, (unint)(0), 0);
            if(expr == NULL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }

            DBG(1, "#################################\n");
            dbg_ast(expr);

            DBG(1, "#################################\n");

            struct Value assert;
            if(_eval_expr(p, expr, &assert) == FAIL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }

            // New line
            _reset_peek(p);
            struct token* _e = _peek_token(p);
            if(_e == NULL) return NULL;

            if(!bool_eval(&assert)) {
                // Get the cursor start from the first token that shares the end_lineno
                nint col_offset = -1;
                struct token* _c = _s;
                while(true) {
                    if(_c->lineno == _e->lineno) {
                        col_offset = _c->col_offset;
                        break;
                    } 
                    if(_c == _e) break;
                    _c = _c->next;
                }

                _error_line_with_cursor(p, _e, col_offset, _e->end_col_offset, ERROR_TYPE_ASSERT, "assertion of expression failed");
                return NULL;
            }
        }
        else if(is_at_identifier(_token, WARN_IDENTIFIER) == SUCCESS) {
            _reset_peek(p);           
            struct token* _s = _peek_token(p);
            if(_s == NULL) return NULL;

            // Expr
            struct Ast_node* expr = _parse_expr(p, (unint)(0), 0);
            if(expr == NULL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }

            DBG(1, "#################################\n");
            dbg_ast(expr);
            DBG(1, "#################################\n");

            struct Value warn;
            if(_eval_expr(p, expr, &warn) == FAIL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }

            // New line
            _reset_peek(p);
            struct token* _e = _peek_token(p);
            if(_e == NULL) return NULL;

            if(warn.type != VALUE_STR) {
                // Get the cursor start from the first token that shares the end_lineno
                nint col_offset = -1;
                struct token* _c = _s;
                while(true) {
                    if(_c->lineno == _e->lineno) {
                        col_offset = _c->col_offset;
                        break;
                    } 
                    if(_c == _e) break;
                    _c = _c->next;
                }

                _error_line_with_cursor(p, _e, col_offset, _e->end_col_offset, ERROR_TYPE_TYPE, "invalid type for warning");
                return NULL;
            }

            LOG("  File: \"%s\", line %d\n    @", p->tok->source, _token->lineno);
            int32_t* warn_ident = WARN_IDENTIFIER;
            for(unint i=0; warn_ident[i] != -1; ++i) LOG_CP(warn_ident[i]);
            LOG(" \"");
            for(unint i=0; i<warn.val.string.len; ++i) LOG_CP(warn.val.string.str[i]);
            LOG("\"\n");
        }
        else if(is_at_identifier(_token, ERROR_IDENTIFIER) == SUCCESS) {
            _reset_peek(p);           
            struct token* _s = _peek_token(p);
            if(_s == NULL) return NULL;

            // Expr
            struct Ast_node* expr = _parse_expr(p, (unint)(0), 0);
            if(expr == NULL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }

            DBG(1, "#################################\n");
            dbg_ast(expr);
            DBG(1, "#################################\n");

            struct Value error;
            if(_eval_expr(p, expr, &error) == FAIL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }

            // New line
            _reset_peek(p);
            struct token* _e = _peek_token(p);
            if(_e == NULL) return NULL;

            if(error.type != VALUE_STR) {
                // Get the cursor start from the first token that shares the end_lineno
                nint col_offset = -1;
                struct token* _c = _s;
                while(true) {
                    if(_c->lineno == _e->lineno) {
                        col_offset = _c->col_offset;
                        break;
                    } 
                    if(_c == _e) break;
                    _c = _c->next;
                }

                _error_line_with_cursor(p, _e, col_offset, _e->end_col_offset, ERROR_TYPE_TYPE, "invalid type for error");
                return NULL;
            }

            LOG("  File: \"%s\", line %d\n    @", p->tok->source, _token->lineno);
            int32_t* error_ident = ERROR_IDENTIFIER;
            for(unint i=0; error_ident[i] != -1; ++i) LOG_CP(error_ident[i]);
            LOG(" \"");
            for(unint i=0; i<error.val.string.len; ++i) LOG_CP(error.val.string.str[i]);
            LOG("\"\n");
            return NULL;
        }        
        else if(is_at_identifier(_token, IF_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN IF\n");
        else if (is_at_identifier(_token, INCLUDE_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN INCLUDE\n");
        else if (is_at_identifier(_token, EXTERN_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN EXTER\n");
        else if (is_at_identifier(_token, EXPORT_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN EXPORT\n");
        // For now, later names can be instructions/labels
        else if (_token->type == NAME && is_at_identifier(_token, NULL) == FAIL) {
            if(expect_token(p, EQUAL) == NULL) return NULL;
            
            struct Ast_node* expr = _parse_expr(p, (unint)(0), 0);
            if(expr == NULL) {
                DBG(1, "Error building expression\n");
                return NULL;
            }
            
            DBG(1, "#################################\n");
            dbg_ast(expr);

            DBG(1, "#################################\n");

        }
        else if (_token->type == ENDMARKER) {
            DBG(1, "END OF PARSING\n");
            break;
        } else {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "invalid syntax");
            return NULL;
        }
    }
    return NULL;
}