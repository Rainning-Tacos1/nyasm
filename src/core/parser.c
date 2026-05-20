#include <sys/stat.h>
#include <stdarg.h>
#include <math.h>

#include "api/unicode.h"
#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"
#include "api/file.h"

#include "token.h"
#include "err.h"

#include "lexer.h"
#include "parser.h"
#include "variables.h"
#include "ast.h"

#include "types.h"

#define MACRO_IDENTIDIER ((int32_t[]){'m', 'a', 'c', 'r', 'o', -1})
#define INCLUDE_IDENTIFIER ((int32_t[]){'i', 'n', 'c', 'l', 'u', 'd', 'e', -1})

#define ASSERT_IDENTIFIER ((int32_t[]){'a', 's', 's', 'e', 'r', 't', -1})
#define WARN_IDENTIFIER ((int32_t[]){'w', 'a', 'r', 'n', -1})
#define ERROR_IDENTIFIER ((int32_t[]){'e', 'r', 'r', 'o', 'r', -1})

#define IF_IDENTIFIER ((int32_t[]){'i', 'f', -1})
#define ELIF_IDENTIFIER ((int32_t[]){'e', 'l', 'i', 'f', -1})
#define ELSE_IDENTIFIER ((int32_t[]){'e', 'l', 's', 'e', -1})

#define WHILE_IDENTIFIER ((int32_t[]){'w', 'h', 'i', 'l', 'e', -1})
#define REPEAT_IDENTIFIER ((int32_t[]){'r', 'e', 'p', 'e', 'a', 't', -1})
#define BREAK_IDENTIFIER ((int32_t[]){'b', 'r', 'e', 'a', 'k', -1})

#define IMPORT_IDENTIFIER ((int32_t[]){'i', 'm', 'p', 'o', 'r', 't', -1})

#define BYTE_IDENTIFIER ((int32_t[]){'b', 'y', 't', 'e', -1})
#define WORD_IDENTIFIER ((int32_t[]){'w', 'o', 'r', 'd', -1})
#define DWORD_IDENTIFIER ((int32_t[]){'d', 'w', 'o', 'r', 'd', -1})
#define QWORD_IDENTIFIER ((int32_t[]){'q', 'w', 'o', 'r', 'd', -1})
#define FLOAT_IDENTIFIER ((int32_t[]){'f', 'l', 'o', 'a', 't', -1})
#define DOUBLE_IDENTIFIER ((int32_t[]){'d', 'o', 'u', 'b', 'l', 'e', -1})


#define SAVEB_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'b', -1})
#define SAVEW_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'w', -1})
#define SAVEDW_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'd', 'w',  -1})
#define SAVEQ_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'q', -1})
#define SAVEF_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'f', -1})
#define SAVED_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'd', -1})


#define EXPORT_IDENTIFIER ((int32_t[]){'e', 'x', 'p', 'o', 'r', 't', -1})
#define EXTERN_IDENTIFIER ((int32_t[]){'e', 'x', 't', 'e', 'r', 'n', -1})


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
        if(mt->parent == NULL || mt->parent->file != mt->file) LOG("  file '%s'\n", mt->file);
        LOG("    macro '");
        struct Macro* macro = mt->macro;
        for (unint j = 0; j < macro->name.len; ++j)
            LOG_CP(macro->name.cps[j]);
        LOG("' line: %d\n", mt->lineno);
    }
    if (n > MACRO_EXPANSION_TRACE_START_LIMIT + MACRO_EXPANSION_TRACE_END_LIMIT && i == MACRO_EXPANSION_TRACE_START_LIMIT) {
        LOG("  ...\n");
    }
}

void _print_macro_trace_back(struct MacroTrace* mt) {
    if(mt) {
        LOG("Traceback:\n");
        unint n = macro_trace_len(mt);
        _print_macro_trace_select(mt, 0, n);
        LOG("\n");
    }
}

void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);

    _print_macro_trace_back(_token->macro_trace);

    const char* line_start = p->tok->line_start;
    p->tok->line_start = _token->line_start;
    _syntaxerror_range_with_type(p->tok, stype, format, _token->lineno, _token->end_lineno, _token->col_offset+1, _token->end_col_offset+1, va);
    p->tok->line_start = line_start;

    va_end(va);
}

void _error_line_with_cursor(struct Parser* p, struct token* _token, nint col_offset, nint end_col_offset, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);

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
unint _read(struct Parser* p, struct token** start,  struct token** end, struct token** re) {
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

    struct MacroTrace* mt = (struct MacroTrace*)MEM_ALLOC(sizeof(struct MacroTrace), "macro trace");
    if(mt == NULL) {
        *end = *start = NULL;
        memory_error(p, "no available memory for macro trace structure"); 
        return FAIL;
    }
    
    mt->parent = NULL;
    mt->macro = macro;
    mt->file = _token->file;
    mt->lineno = _token->lineno;

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
        mcopy[i].macro_trace = mt;
    }
    mcopy[total_macro_tokens-1].next = NULL;
    mcopy[total_macro_tokens-1].macro_trace = mt;

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
        DBG(DO_MACRO_SANITY_CHECK_DBG, "[%p:%s:l%d:c%d:t%p] ", _t, _Parser_TokenNames[_t->type], _t->lineno, _t->col_offset, _t->macro_trace);
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
    // struct MacroTrace* mt = NULL;
    while(true) {
        struct token *s, *e;
        struct token* rs = *re; // Replace start

        if(_read(p, &s, &e, re) == FAIL) return FAIL;

        // When its the 1st time reading tokens p->head will start at s
        if(p->head == NULL) p->head = s;

        // Macro expansion token
        if(s != NULL && e != NULL && s != *re) {
            s->macro_trace->parent = rs->next->macro_trace;

            // We assume that rs != NULL bc the first token ever read cant be a macro
            rs->next = s;

            if((*re)->next == NULL) p->tail = e;
            else e->next = (*re)->next;

            cursor = rs;
            re = &cursor;


            DBG(DO_MACRO_END_MACRO_TOKEN_DBG, "[%p:%s:l%d:c%d] ADDED TO THE STACK\n", e, _Parser_TokenNames[e->type], e->lineno, e->col_offset);
            p->macro_ends[p->macro_end_cursor++] = e;

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

        else if(s != NULL && (s == e) && s == *re) {
            // Macro ends dont end on new lines
            if(s->type == NEWLINE) {
                // We can safely access s->next because macros dont end on new lines
                DBG(DO_MACRO_PENDING_DEDENTS_DBG, "s->next = %p | EMITING %d DEDENT TOKENS\n", s->next, p->pending_dedents);

                if(p->pending_dedents > 0 ) {
                    struct token* dedents = (struct token*)MEM_ALLOC(sizeof(struct token) * p->pending_dedents, "macro dedent tokens");
                    for(unint i=0; i<(p->pending_dedents-1); ++i) {

                        dedents[i] = *s; // Copy
                        dedents[i].type = DEDENT;
                        dedents[i].lineno += 1;     // Next line
                        dedents[i].end_lineno += 1; // Next line
                        dedents[i].col_offset = dedents[i].end_col_offset = -1;
                        dedents[i].start = dedents[i].end = NULL;
                        dedents[i].cps = NULL;
                        dedents[i].len = 0;

                        dedents[i].line_start = NULL;
                        dedents[i].next = &dedents[i+1];
                        dedents[i].macro_trace = s->macro_trace;
                        dedents[i].file = s->file;

                    }
                    dedents[p->pending_dedents-1] = *s; // Copy
                    dedents[p->pending_dedents-1].type = DEDENT;
                    dedents[p->pending_dedents-1].lineno += 1;     // Next line
                    dedents[p->pending_dedents-1].end_lineno += 1; // Next line
                    dedents[p->pending_dedents-1].col_offset = dedents[p->pending_dedents-1].end_col_offset = -1;
                    dedents[p->pending_dedents-1].start = dedents[p->pending_dedents-1].end = NULL;
                    dedents[p->pending_dedents-1].cps = NULL;
                    dedents[p->pending_dedents-1].len = 0;

                    dedents[p->pending_dedents-1].line_start = NULL;
                    dedents[p->pending_dedents-1].next = s->next;
                    dedents[p->pending_dedents-1].macro_trace = s->macro_trace;
                    dedents[p->pending_dedents-1].file = s->file;
                    s->next = dedents;
                }
                
                p->pending_dedents = 0;
            }
            else if(p->macro_end_cursor > 0 && s == p->macro_ends[p->macro_end_cursor-1]) {
                p->macro_ends[p->macro_end_cursor--] = NULL;
                DBG(DO_MACRO_MACRO_END_DBG, "[%p:%s:l%d:c%d]: ENDED MACRO\n", s, _Parser_TokenNames[s->type], s->lineno, s->col_offset);
                p->pending_dedents += s->macro_trace->macro->dedents_needed;
            }
        }

        if(re != &p->tail && *re == p->tail) {
            DBG(DO_MACRO_RECOVER_TAIL_DBG, "Recovering tail\n");
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

    p->tok->source = token->file;
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

    p->tok->source = token->file;
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
    p->head = p->last_token = p->peek = p->tail = NULL;
    p->macros = p->macros_tail = NULL;
    p->pending_dedents = p->macro_end_cursor = p->macro_expansion_count = p->is_inside_macro_decl = 0;

    for(unint i=0; i<MAX_MACRO_EXPANSION_LIMIT; ++i) p->macro_ends[i] = NULL;

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

            // struct Variable* var = get_variable(p, _token->cps, _token->len);
            // if(var == NULL) {
            //     _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "variable is not declared");
            //     return NULL;
            // }

            return new_ast_variable(p, _token);
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
                if(_eval_expr(p, el, &val) == FAIL) return NULL;

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

        if(_expr_get_binding_power(op->type, &lbp, &rbp) == FAIL) {
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
            // Check if variable is declared
            // Get the variable
            struct Variable* var = get_variable(p, expr->node.var);
            if(var == NULL) {
                _error_from_token(p, expr->node.var, ERROR_TYPE_EXPRESSION, "variable is not declared");
                return FAIL;
            }
            *val = var->val;
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
    macro->dedents_needed = 0;

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

char* cp_string_to_encoding(int32_t* cps, nint cp_len) {
    if (!cps || cp_len == 0) {
        char *empty = MEM_ALLOC(1, "string null term");
        if (empty) empty[0] = '\0';
        return empty;
    }

    nint capacity = CP_ENCODING_BUF;
    nint used = 0;

    char *result = MEM_ALLOC(capacity, "include string path");
    if (!result) return NULL;

    unsigned char buf[CP_ENCODING_BUF];

    for (nint i = 0; i < cp_len; i++) {
        nint written = CP_TO_ENCODING_BUF_GET_LEN(cps[i], buf);
        if (written == 0) continue;

        nint bytes_to_copy = written - 1; // skip null terminator

        if (used + bytes_to_copy + 1 > capacity) {
            capacity += CP_ENCODING_BUF;
            result = MEM_RESIZE_LAST(capacity);
            if (!result) return NULL;
        }

        for (nint j = 0; j < bytes_to_copy; j++) result[used + j] = buf[j];
        used += bytes_to_copy;
    }

    result[used] = '\0';

    // shrink
    result = MEM_RESIZE_LAST(used + 1);
    return result;
}

#define FLAG_MACRO_DECL 1
#define FLAG_INCLUDE_DECL 2

unint _parse_statement(struct Parser* p, struct Ast_node** stmt_ast, unint* flags);

unint _parse_block(struct Parser* p, struct AstStatements* statements, unint* found_eof) {
    if(expect_token(p, INDENT) == NULL) return FAIL;

    while(true) {
        _reset_peek(p);
        struct token* dedent = _peek_token(p);
        if(dedent == NULL) return FAIL;

        DBG(1, "BLOCK PEEKED: [%s:l%d:c%d]\n", _Parser_TokenNames[dedent->type], dedent->lineno, dedent->col_offset);
        if(dedent->type == DEDENT) {
            _read_token(p);
            *found_eof = 0;
            return SUCCESS;
        }
        
        // Parse statements
        struct Ast_node* stmt_ast = NULL;
        unint _flags = 0;
        if(_parse_statement(p, &stmt_ast, &_flags) == FAIL) return FAIL;

        if(_flags == FLAG_MACRO_DECL || _flags == FLAG_INCLUDE_DECL) continue; // Skip macro
        if(stmt_ast == NULL) {
            *found_eof = 1;
            return SUCCESS;
        }

        if(insert_ast_statement_node(p, statements, stmt_ast) == NULL) return FAIL;
    }  
}

struct Ast_node* parse_expr_with_start_and_end(struct Parser* p, struct token** _s, struct token** _e) {
    *_s = *_e = NULL;
    _reset_peek(p);           
    if((*_s = _peek_token(p)) == NULL) return NULL;

    // Expr
    struct Ast_node* cond = _parse_expr(p, (unint)(0), 0);
    if(cond == NULL) {
        DBG(1, "Error building expression\n");
        return NULL;
    }

    DBG(1, "#################################\n");
    dbg_ast(cond);
    DBG(1, "#################################\n");

    // New line
    if((*_e = _peek_token(p)) == NULL) return NULL;
    _reset_peek(p);

    return cond;
}

void report_expr_error_with_start_and_end(struct Parser* p, struct token* _s, struct token* _e, const char *stype, const char* string) {
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

    _error_line_with_cursor(p, _e, col_offset, _e->end_col_offset, stype, string);
}

#define IS_ASSIGNMENT(_token) (_token == EQUAL || _token == PLUSEQUAL || _token == MINEQUAL || _token == STAREQUAL || _token == SLASHEQUAL || _token == PERCENTEQUAL || _token == AMPEREQUAL || _token == VBAREQUAL || _token == CIRCUMFLEXEQUAL || _token == LEFTSHIFTEQUAL || _token == RIGHTSHIFTEQUAL)

unint _parse_statement(struct Parser* p, struct Ast_node** stmt_ast, unint* flags) {
    struct token* _token;
    // Skip comments && new lines
    _reset_peek(p);
    struct token* last_token = p->last_token;

    while ((_token = _read_token(p)) != NULL &&
        (_token->type == COMMENT || _token->type == NEWLINE)) {
        last_token = _token;
    }

    if (_token == NULL) return FAIL;

    DBG(1, "DONE SKIPING (using last)\n");

    // Non AST stuff
    if(is_at_identifier(_token, MACRO_IDENTIDIER) == SUCCESS) {
        struct token* macro_name_token;

        // @macro
        if((macro_name_token = expect_token(p, NAME)) == NULL) return FAIL;

        // '('
        if(expect_token(p, LPAR) == NULL) return FAIL;

        // Already declared?
        if(is_macro_declared(p, macro_name_token->cps, macro_name_token->len) == SUCCESS) {
            _error_from_token(p, macro_name_token, ERROR_TYPE_DECLARATION, "macro is already declared");
            return FAIL;
        }

        // [PARSER] Init/Append macro
        struct Macro* macro = new_empty_macro(p, macro_name_token, macro_name_token->cps, macro_name_token->len);
        if(macro == NULL) return FAIL;

        // Fill with the arguments
        struct token* arg_token;
        unint has_variadic_arg = 0;
        unint is_variadic_arg = 0;
        unint read_macro_args = 1;

        // Easy end?
        arg_token = _peek_token(p);
        if(arg_token == NULL) return FAIL;

        if(arg_token->type == RPAR) read_macro_args = 0;
        else _reset_peek(p);

        while(read_macro_args) {
            is_variadic_arg = 0;
            arg_token = _peek_token(p);
            if(arg_token == NULL) return FAIL;

            if(arg_token->type == STAR) {
                _read_token(p); // Consume the star
                if(has_variadic_arg) {
                    _error_from_token(p, arg_token, ERROR_TYPE_MESSAGE, "macros can only have one variadic argument");
                    return FAIL;
                } else has_variadic_arg = 1;
                
                is_variadic_arg = 1;
                if((arg_token = expect_token(p, NAME)) == NULL) return FAIL;
            } else {
                if((arg_token = expect_token(p, NAME)) == NULL) return FAIL;
            }

            // DO not allow repeated names
            if(is_macro_arg(macro, arg_token->cps, arg_token->len) == SUCCESS) {
                _error_from_token(p, arg_token, ERROR_TYPE_MESSAGE, "duplicate argument in macro declaration");
                return FAIL;     
            }
            
            // Store the arg name
            if(append_macro_arg(p, arg_token, macro, arg_token->cps, arg_token->len, is_variadic_arg) == FAIL) return FAIL;
            
            // Commas
            struct token* comma_token = _peek_token(p);
            if(comma_token == NULL) return FAIL;
            _reset_peek(p);
            // If we are not ending the macro, expect a comma
            if(comma_token->type == RPAR) break;

            if(expect_token(p, COMMA) == NULL) return FAIL;
        }

        if(expect_token(p, RPAR) == NULL) return FAIL;

        // Read the new line, or expect it?
        if(expect_token(p, NEWLINE) == NULL) return FAIL;
        // if(_read_token(p) == NULL) return FAIL;

        p->is_inside_macro_decl = 1;

        // peek the indentation token
        struct token* indentation = _peek_token(p);
        if(indentation == NULL) return FAIL;

        if(indentation->type == INDENT) {
            _read_token(p); // Consume indentation
            // Store the begining of the macro, skiping the first indent
            macro->tokens = _peek_token(p);
            if(macro->tokens == NULL) return FAIL;

            unint level = 1;
            unint last_dedents = 0;
            // Read tokens until indentation is back to the start
            while(level != 0) {
                indentation = _read_token(p);
                if(indentation == NULL) return FAIL;

                if(indentation->type == INDENT) { ++level; }
                if(indentation->type == DEDENT) {
                    ++last_dedents;
                    level--;
                } else last_dedents = 0;
                ++macro->tok_len;
            }
            macro->tok_len -= (last_dedents + 1); // remove dedents & last newline
            macro->dedents_needed = --last_dedents;

            DBG(DO_MACRO_LAST_DEDENTS_DBG, "macro->tok_len = %d | last_dedents = %d\n", macro->tok_len, last_dedents);

        }

        p->is_inside_macro_decl = 0;

        DBG(1, "-----------------------------------------------------------------\n");
        print_macro(macro);
        DBG(1, "-----------------------------------------------------------------\n");

        *flags = FLAG_MACRO_DECL;
        *stmt_ast = NULL;
        return SUCCESS;
    }

    else if (is_at_identifier(_token, INCLUDE_IDENTIFIER) == SUCCESS) {
        
        // Include path
        struct token* _include = _read_token(p);
        if(_include == NULL) return FAIL;

        if(_include->type != STRING) {
            _error_from_token(p, _include, ERROR_TYPE_TYPE, "invalid type for @include");
            return FAIL;
        }

        struct token* _e = expect_token(p, NEWLINE);
        if(_e == NULL) return FAIL;

        // Check file
        char* include_path = cp_string_to_encoding(_include->cps+1, _include->len-2); // Remove the quotes
        if(include_path == NULL) {
            _error_from_token(p, _include, ERROR_TYPE_MEMORY, "no available memory for @include string");
            return FAIL;
        }

        _stat sfile;
        if(stat(include_path, &sfile) != 0) {
            _error_from_token(p, _include, ERROR_TYPE_PATH, "could not open file: \"%s\"", include_path);
            return FAIL;
        }
        if(!__S_ISREG(sfile.st_mode)){
            _error_from_token(p, _include, ERROR_TYPE_PATH, "path: \"%s\" is not a file", include_path);
            return FAIL;
        }

        // Include
        unint size = 0;
        char* file = LOAD_FILE(include_path, &size);
        if(file == NULL) {
            memory_error(p, "error loading file: \"%s\" into memory", include_path);
            return FAIL;
        }

        // Make room for a potential implicit new line
        if(file != NULL) file = MEM_RESIZE_LAST(size + SIZEOF_IMPLICIT_NEWLINE);

        // new Tokenizer
        struct tok_state* _tok = _Tokenizer_tok_new();
        if(_tok == NULL) {
            memory_error(p, "no available memory for the tokenizer\n");            
            return FAIL;
        }

        _tok->uc.curr = _tok->uc.buf = file;
        _tok->inp = _tok->uc.curr; // Trigger an underflow/verification
        _tok->uc.end = file+size; // There is still space for an implicit newline
        _tok->source = include_path;

        // new Parser
        struct Parser* _p = _Parser_New(_tok);
        if(_p == NULL) {
            memory_error(p, "no available memory for the parser\n");
            return FAIL;
        }

        _p->is_inside_macro_decl = 1; // Do not expand macros
        _p->macro_expansion_count = p->macro_expansion_count;

        struct token* prev = NULL;
        while(true){
            struct token* _token = _read_token(_p);

            if(_token == NULL || _token->type == ERRORTOKEN) return FAIL;
            if(_token->type == ENDMARKER) break;
            prev = _token;
        }

        // Link
        prev->next = NULL; // Unlink [ENDMARKER]
        if(last_token == NULL) p->head = _p->head;
        else {
            last_token->next = _p->head;
            DBG(1, "last_token: [%p:%s:l%d:c%d]\n", last_token, _Parser_TokenNames[last_token->type], last_token->lineno,last_token->col_offset);
        }
        prev->next = _e;
        p->last_token = (p->peek = last_token);            
        _p->is_inside_macro_decl = 0;

        *flags = FLAG_INCLUDE_DECL;
        *stmt_ast = NULL;
        return SUCCESS;
    }

    // Ast stuff
    else if(is_at_identifier(_token, ASSERT_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e);
        if(expr == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* assert = new_ast_assert(p, expr, _s, _e);
        if(assert == NULL) return FAIL;

        *stmt_ast = assert;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, WARN_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e);
        if(expr == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* warn = new_ast_warn(p, expr, _s, _e);
        if(warn == NULL) return FAIL;

        *stmt_ast = warn;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, ERROR_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e);
        if(expr == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* error = new_ast_error(p, expr, _s, _e);
        if(error == NULL) return FAIL;

        *stmt_ast = error;
        return SUCCESS;
    }        

    else if(is_at_identifier(_token, IF_IDENTIFIER) == SUCCESS) {
        struct Ast_node* cond = _parse_expr(p, (unint)(0), 0);
        if(cond == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _if = new_ast_if(p, _token, cond);
        if(_if == NULL) return FAIL;

        unint found_eof = 0;
        // Parse the block
        if(_parse_block(p, &_if->node._if.statements, &found_eof) == FAIL) return FAIL;
        if(found_eof == 1) goto _end_if;

        // Elif 
        struct token* _elif;
        while(true) {
            _elif = _peek_token(p);
            if(_elif == NULL) return FAIL;

            if(is_at_identifier(_elif, ELIF_IDENTIFIER) == FAIL) break;
            
            _read_token(p);

            // Condition
            struct Ast_node* elif_cond = _parse_expr(p, (unint)(0), 0);
            if(cond == NULL) return FAIL;
            
            // Read the new line
            if(expect_token(p, NEWLINE) == NULL) return FAIL;

            if(insert_elif(p, _elif, _if, elif_cond) == FAIL) return FAIL;
            
            _if->node._if._elifs_end->cond = elif_cond;

            // Block
            if(_parse_block(p, &_if->node._if._elifs_end->statements, &found_eof) == FAIL) return FAIL;
            if(found_eof == 1) goto _end_if;
        }

        if(is_at_identifier(_elif, ELSE_IDENTIFIER) == FAIL) goto _end_if;

        _read_token(p); // Read else
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        // Else
        if(insert_else(p, _elif, _if) == FAIL) return FAIL;  
        
        // Block
        if(_parse_block(p, _if->node._if._else, &found_eof) == FAIL) return FAIL;
        
_end_if:
        _reset_peek(p);
        *stmt_ast = _if;
        return SUCCESS;
    }
    
    else if(is_at_identifier(_token, WHILE_IDENTIFIER) == SUCCESS) {
        struct Ast_node* cond = _parse_expr(p, (unint)(0), 0);
        if(cond == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _while = new_ast_while(p, _token, cond);
        if(_while == NULL) return FAIL;

        unint found_eof = 0;
        // Parse the block
        if(_parse_block(p, &_while->node._while.statements, &found_eof) == FAIL) return FAIL;

        *stmt_ast = _while;
        return SUCCESS;
    }
    
    else if(is_at_identifier(_token, REPEAT_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* iter = parse_expr_with_start_and_end(p, &_s, &_e);
        if(iter == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _repeat = new_ast_repeat(p, _token, iter, _s, _e);
        if(_repeat == NULL) return FAIL;

        unint found_eof = 0;
        // Parse the block
        if(_parse_block(p, &_repeat->node._repeat.statements, &found_eof) == FAIL) return FAIL;

        *stmt_ast = _repeat;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, BREAK_IDENTIFIER) == SUCCESS) {

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _break = new_ast_break(p, _token);
        if(_break == NULL) return FAIL;

        *stmt_ast = _break;
        return SUCCESS; 
    }

    else if(is_at_identifier(_token, IMPORT_IDENTIFIER) == SUCCESS) {
        // Include path
        struct token* _import = _read_token(p);
        if(_import == NULL) return FAIL;

        if(_import->type != STRING) {
            _error_from_token(p, _import, ERROR_TYPE_TYPE, "invalid type for @import");
            return FAIL;
        }

        struct token* _e = expect_token(p, NEWLINE);
        if(_e == NULL) return FAIL;

        struct Ast_node* _i = new_ast_import(p, _import);
        if(_i == NULL) return FAIL;

        *stmt_ast = _i;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, BYTE_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, BYTE_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, WORD_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, WORD_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, DWORD_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, DWORD_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, QWORD_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, QWORD_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, FLOAT_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, FLOAT_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, DOUBLE_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, DOUBLE_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, SAVEB_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, SAVEB_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, SAVEW_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, SAVEW_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, SAVEDW_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, SAVEDW_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, SAVEQ_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, SAVEQ_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, SAVEF_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, SAVEF_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, SAVED_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* total = parse_expr_with_start_and_end(p, &_s, &_e);
        if(total == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _space = new_ast_space(p, total, _s, _e, SAVED_NODE);
        if(_space == NULL) return FAIL;

        *stmt_ast = _space;
        return SUCCESS;
    }

    else if(_token->type == NAME && is_at_identifier(_token, NULL) == FAIL) {
        unint level = 0;

        // Assignment
        while(true) {
            struct token* _t = _peek_token(p); 
            if(_t == NULL) return FAIL;

            if     (_t->type == LPAR || _t->type == LSQB) ++level;
            else if(_t->type == RPAR || _t->type == RSQB) --level;
            else if(_t->type == NEWLINE) break;
            else if((IS_ASSIGNMENT(_t->type)) && level == 0) {
                _reset_peek(p); // Go back

                struct token *_s, *_e;
                struct Ast_node* idx;
                struct Ast_node* expr;
                unint node_type = ASSIGN_VAR_NODE;

                // After the identifier only "[" or ASSIGNMENT are valid
                struct token* __t = _read_token(p);
                if(__t == NULL) return FAIL;

                // Append or indexation
                if(__t->type == LSQB) {
                    // index or append 
                    struct token* _p1 = _peek_token(p); // RSQB or start of expression
                    if(_p1 == NULL) return FAIL;

                    // Append
                    if(_p1->type == RSQB) { _read_token(p); node_type = ASSIGN_APPEND_ARRAY_NODE; }
                    
                    // Expression
                    else {
                        _reset_peek(p);

                        idx = parse_expr_with_start_and_end(p, &_s, &_e);
                        if(idx == NULL) {
                            DBG(1, "Error building expression\n");
                            return FAIL;
                        }

                        DBG(1, "#################################\n");
                        dbg_ast(idx);
                        DBG(1, "#################################\n");

                        // Closing RSQB
                        if(expect_token(p, RSQB) == NULL) return FAIL;
                        
                        node_type = ASSIGN_VAR_IDX_NODE;
                    }
                    // After append / idx
                    if((__t = _read_token(p)) == NULL) return FAIL; // Look forward
                } 
                
                // Expect a valid assignment
                if(IS_ASSIGNMENT(__t->type)) {
                    expr = _parse_expr(p, (unint)(0), 0);
                    if(expr == NULL) {
                        DBG(1, "Error building expression\n");
                        return FAIL;
                    }

                    DBG(1, "#################################\n");
                    dbg_ast(expr);
                    DBG(1, "#################################\n");

                    // expect new line
                    if(expect_token(p, NEWLINE) == NULL) return FAIL;

                    *stmt_ast = NULL;
                    switch(node_type) {
                        case ASSIGN_VAR_NODE:
                            *stmt_ast = new_ast_assign_variable(p, _token, expr, __t->type);
                            break;
                        case ASSIGN_VAR_IDX_NODE:
                            *stmt_ast = new_ast_assign_variable_idx(p, _token, idx, expr, __t->type);
                            break;
                        case ASSIGN_APPEND_ARRAY_NODE:
                            *stmt_ast = new_ast_assign_append_array(p, _token, expr, __t->type);
                            break;
                    }

                    return (*stmt_ast == NULL) ? FAIL : SUCCESS;
                } else {
                    // Invalid assignment
                    _error_from_token(p, __t, ERROR_TYPE_MESSAGE, "invalid assignment");
                    return FAIL;
                }

            }
        }

        // Instruction
        DBG(1, "INSTRUCTION\n");
        return FAIL;
    }

    else if (_token->type == ENDMARKER) {
        DBG(1, "END OF PARSING\n");
        *stmt_ast = NULL;
        return SUCCESS;
    } else {
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "invalid syntax");
        return FAIL;
    }
    return FAIL;
}


struct Ast_node* _run_parser(struct Parser* p) {
    struct Ast_node* ast = new_ast(p);
    if(ast == NULL) return NULL;
    
    while(true) {
        struct Ast_node* stmt_ast = NULL;
        unint flags = 0;
        if(_parse_statement(p, &stmt_ast, &flags) == FAIL) return NULL; // Something wrong that should not recover the AST

        if(flags == FLAG_MACRO_DECL || flags == FLAG_INCLUDE_DECL) continue; // Skip macro
        if(stmt_ast == NULL) return ast; // Eof
        if(insert_ast_statement_node(p, &ast->node.statements, stmt_ast) == NULL) return NULL;
    }
}