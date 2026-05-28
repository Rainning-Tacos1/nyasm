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

#include "asm_lang.h"
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
#define CONTINUE_IDENTIFIER ((int32_t[]){'c', 'o', 'n', 't', 'i', 'n', 'u', 'e', -1})

#define BYTE_IDENTIFIER ((int32_t[]){'b', 'y', 't', 'e', -1})
#define WORD_IDENTIFIER ((int32_t[]){'w', 'o', 'r', 'd', -1})
#define DWORD_IDENTIFIER ((int32_t[]){'d', 'w', 'o', 'r', 'd', -1})
#define QWORD_IDENTIFIER ((int32_t[]){'q', 'w', 'o', 'r', 'd', -1})
#define FLOAT_IDENTIFIER ((int32_t[]){'f', 'l', 'o', 'a', 't', -1})
#define DOUBLE_IDENTIFIER ((int32_t[]){'d', 'o', 'u', 'b', 'l', 'e', -1})
#define PTR_IDENTIFIER ((int32_t[]){'p', 't', 'r', -1})

#define SAVEB_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'b', -1})
#define SAVEW_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'w', -1})
#define SAVEDW_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'd', 'w',  -1})
#define SAVEQ_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'q', -1})
#define SAVEF_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'f', -1})
#define SAVED_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'd', -1})
#define SAVEP_IDENTIFIER ((int32_t[]){'s', 'a', 'v', 'e', 'p', -1})

#define ALIGN_IDENTIFIER ((int32_t[]){'a', 'l', 'i', 'g', 'n', -1})

// #define IMPORT_IDENTIFIER ((int32_t[]){'i', 'm', 'p', 'o', 'r', 't', -1})
#define STRUCT_IDENTIFIER ((int32_t[]){'s', 't', 'r', 'u', 'c', 't', -1})
#define STRING_IDENTIFIER ((int32_t[]){'s', 't', 'r', 'i', 'n', 'g', -1})
#define RETURN_IDENTIFIER ((int32_t[]){'r', 'e', 't', 'u', 'r', 'n', -1})
#define DEL_IDENTIFIER ((int32_t[]){'d', 'e', 'l', -1})
// #define FUN_IDENTIFIER ((int32_t[]){'f', 'u', 'n', -1})
#define ORG_IDENTIFIER ((int32_t[]){'o', 'r', 'g', -1})

#define CODE_IDENTIFIER ((int32_t[]){'c', 'o', 'd', 'e', -1})

// #define P_IDENTIFIER ((int32_t[]){'p', -1})
#define LEN_IDENTIFIER ((int32_t[]){'l', 'e', 'n', -1})


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
    "$",               // DOLLAR
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
            //DBG(DO_PARSER_RADOM_STUFF_DBG, "(*re)->next = %p", (*re)->next);
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
    // DBG(DO_PARSER_RADOM_STUFF_DBG, "[READ Token] re = %p | &p->tail = %p | p->tail = [%s:l%d:c%d] --------------------------------\n", re, &p->tail, _Parser_TokenNames[p->tail->type], p->tail->lineno, p->tail->col_offset);

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
    p->ctx_block_cursor = p->pending_dedents = p->macro_end_cursor = p->macro_expansion_count = p->is_inside_macro_decl = 0;

    p->struct_decl = p->struct_decl_tail = NULL;
    // p->func_decl = p->func_decl_tail = NULL;

    p->global_label_decl = p->global_label_decl_tail = NULL;
    // p->func_label_decl = p->func_label_decl_tail = NULL;

    for(unint i=0; i<MAX_MACRO_EXPANSION_LIMIT; ++i) p->macro_ends[i] = NULL;

    p->addr = 0;

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
unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val, struct Value** var_ref_idx);
struct Ast_node* parse_expr_with_start_and_end(struct Parser* p, struct token** _s, struct token** _e, unint min_bp, unint stop_on_comma);
void _error_from_multiple_tokens(struct Parser* p, struct token* _s, struct token* _e, const char *stype, const char *format);


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
            DBG(DO_PARSER_RADOM_STUFF_DBG, "Expr pref is string\n");
            return new_ast_string(p, _token);
        case NUMBER:
            DBG(DO_PARSER_RADOM_STUFF_DBG, "Expr pref is number\n");
            return new_ast_number(p, _token, 0);
        case MINUS:
            // might be a negation or a negative number
            struct token* _peek = _peek_token(p);
            if(_peek == NULL) return NULL;

            // Handle the number
            if(_peek->type == NUMBER) {
                DBG(DO_PARSER_RADOM_STUFF_DBG, "Expr pref is negative number\n");

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
            if(is_at_identifier(_token, LEN_IDENTIFIER) == SUCCESS) {
                struct token *_s, *_e;
                struct Ast_node* len = parse_expr_with_start_and_end(p, &_s, &_e, UNARY_BP, stop_on_comma);
                if(len == NULL) return NULL;

                return new_ast_len(p, _token, _s, _e, len);

            } else if(is_at_identifier(_token, NULL) == SUCCESS) {
                _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "@ identifiers are not allowed on expressions");
                return NULL;
            }

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
                if(_token->type == RSQB) {
                    _read_token(p);
                    break;
                }

                // parse the expression
                struct Ast_node* el = _parse_expr(p, 0, 1); // Do stop on commas
                if(el == NULL) return NULL;

                // Eval the expression
                // struct Value val;
                // struct Value* var_ref_idx;
                // if(_eval_expr(p, el, &val, &var_ref_idx) == FAIL) return NULL;

                // Append to the array
                if(append_array(p, _token, arr, el) == FAIL) return NULL;

                // Read possibly the next comma or ]
                if((_token = _peek_token(p)) == NULL) return NULL;
                if(_token->type == COMMA) {
                    _read_token(p); // COMMA
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
        case DOLLAR:
            return new_ast_dollar(p, _token);

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
            
            struct token *_s, *_e;
            
            struct Ast_node* right = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0); // Do not allow commas
            if(right == NULL) return NULL;

            struct token * rsqb = _read_token(p);
            if(rsqb == NULL) return NULL;
            if(rsqb->type != RSQB) return NULL;

            left = new_ast_binop(p, op, LSQB, left, right);
            if(left == NULL) return NULL;

            left->node.binop._s = _s;
            left->node.binop._e = _e;

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

void _error_from_multiple_tokens(struct Parser* p, struct token* _s, struct token* _e, const char *stype, const char *format) {
    nint col_offset = -1;
    struct token* _c = _s;

    while(_c != _e) {
        if (_c->lineno == _e->lineno) {
            col_offset = _c->col_offset;
            break;
        }
        _c = _c->next;
    }

    struct token* _v = _s;
    while (_v && _v->next != _e) _v = _v->next;

    _error_line_with_cursor(p, _c, col_offset, _v->end_col_offset, stype, format);
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
    DBG(DO_PARSER_RADOM_STUFF_DBG, "@macro ");
    for(unint i=0; i<macro->name.len; ++i) DBG_CP(1, macro->name.cps[i]);
    DBG(DO_PARSER_RADOM_STUFF_DBG, " (");
    struct MacroArg* arg = macro->args;
    while(arg != NULL) {
        DBG(DO_PARSER_RADOM_STUFF_DBG, "\n\t");
        if(arg->is_variadic) DBG(DO_PARSER_RADOM_STUFF_DBG, "*");
        for(unint i=0; i<arg->arg_name.len; ++i) DBG_CP(1, arg->arg_name.cps[i]);
        arg = arg->next;
    }
    DBG(DO_PARSER_RADOM_STUFF_DBG, "\n) [ ");

    struct token* tok = macro->tokens;
    for(unint i=0; i<macro->tok_len; ++i) {
        DBG(DO_PARSER_RADOM_STUFF_DBG, "%s ", _Parser_TokenNames[tok->type]);
        tok = tok->next;
    }
    DBG(DO_PARSER_RADOM_STUFF_DBG, "]\n");

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

unint _parse_statement(struct Parser* p, struct Ast_node** stmt_ast, unint* flags, unint block_ctx);

unint is_inside_block(struct Parser* p, unint block_ctx) {
    for(unint i=0; i<p->ctx_block_cursor; ++i) if(p->ctx_block_stack[i] == block_ctx) return SUCCESS;
    return FAIL;
}

unint _parse_block(struct Parser* p, struct AstStatements* statements, unint block_ctx) {
    // Add ctx to the stack
    if(p->ctx_block_cursor >= MAX_CTX_BLOCK_LEVEL) {
        memory_error(p, "Too many context blocks");
        return FAIL;
    }

    p->ctx_block_stack[p->ctx_block_cursor++] = block_ctx;

    if(block_ctx != CTX_GLOBAL && expect_token(p, INDENT) == NULL) return FAIL;

    while(true) {
        if(block_ctx != CTX_GLOBAL) {
            _reset_peek(p);
            struct token* dedent = _peek_token(p);
            if(dedent == NULL) return FAIL;
    
            DBG(DO_PARSER_RADOM_STUFF_DBG, "BLOCK PEEKED: [%s:l%d:c%d]\n", _Parser_TokenNames[dedent->type], dedent->lineno, dedent->col_offset);
            if(dedent->type == DEDENT) {
                _read_token(p);

                // Pop the stack
                --p->ctx_block_cursor;
                return SUCCESS;
            }
        }
        
        // Parse statements
        struct Ast_node* stmt_ast = NULL;
        unint _flags = 0;
        if(_parse_statement(p, &stmt_ast, &_flags, block_ctx) == FAIL) return FAIL;

        // skip new lines after
        struct token *nl;

        while ((nl = _peek_token(p)) != NULL && nl->type == NEWLINE) _read_token(p);
        if (nl == NULL) return FAIL;
        
        _reset_peek(p);

        if(_flags == FLAG_MACRO_DECL || _flags == FLAG_INCLUDE_DECL) continue; // Skip macro
        // Eof
        if(stmt_ast == NULL) {
            // Pop the stack
            --p->ctx_block_cursor;
            return SUCCESS;
        }

        if(insert_ast_statement_node(p, statements, stmt_ast) == NULL) return FAIL;
    }  
}

struct Ast_node* parse_expr_with_start_and_end(struct Parser* p, struct token** _s, struct token** _e, unint min_bp, unint stop_on_comma) {
    *_s = *_e = NULL;
    _reset_peek(p);           
    if((*_s = _peek_token(p)) == NULL) return NULL;

    // Expr
    struct Ast_node* cond = _parse_expr(p, min_bp, stop_on_comma);
    if(cond == NULL) {
        DBG(DO_PARSER_RADOM_STUFF_DBG, "Error building expression\n");
        return NULL;
    }

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

unint read_possible_alignemnt(struct Parser* p, struct Ast_node** align_expr) {
    *align_expr = NULL;
    // Possible ":"
    struct token* colon = _peek_token(p);
    if(colon == NULL) return FAIL;

    if(colon->type == COLON) {
        _read_token(p); // COLON

        struct token* align_tok = _read_token(p);
        if(align_tok == NULL) return FAIL;
        
        if(align_tok->type == NUMBER) {
            *align_expr = new_ast_number(p, align_tok, 0);
            if(*align_expr == NULL) return FAIL;

            if((*align_expr)->node.literal.value->type != VALUE_INT || (*align_expr)->node.literal.value->val.number == 0) goto _invalid_alignment;

            return (align_expr == NULL) ? FAIL : SUCCESS;
        } else {
_invalid_alignment:
            _error_from_token(p, align_tok, ERROR_TYPE_MESSAGE, "invalid alignment");
            return FAIL;
        }
    }
    _reset_peek(p);
    return SUCCESS;

}

unint read_possible_array(struct Parser* p, struct Ast_node** len_expr, struct Ast_node** align_expr) {
    *len_expr = *align_expr = NULL;

    // Possible "["
    struct token* lsqb = _peek_token(p);
    if(lsqb == NULL) return FAIL;

    if(lsqb->type == LSQB) {
        _read_token(p); // LSQB

        // Len
        struct token* len_tok = _read_token(p);
        if(len_tok == NULL) return FAIL;

        if(len_tok->type == NUMBER) {
            *len_expr = new_ast_number(p, len_tok, 0);
            if(*len_expr == NULL) return FAIL;

            if((*len_expr)->node.literal.value->type != VALUE_INT || (*len_expr)->node.literal.value->val.number == 0) goto _invalid_len;

        } else {
_invalid_len:
            _error_from_token(p, len_tok, ERROR_TYPE_MESSAGE, "invalid length");
            return FAIL;        
        }

        // Possible alignment
        if(read_possible_alignemnt(p, align_expr) == FAIL) return FAIL;

        // Closing "]"
        if(expect_token(p, RSQB) == NULL) return FAIL;

        return SUCCESS;
    }

    _reset_peek(p);
    return SUCCESS;
}

unint handle_struct_decl_field(struct Parser* p, struct Ast_node* _struct_decl, struct Ast_node** align_start_expr, struct Ast_node** len_expr, struct Ast_node** align_per_el_expr, struct token** field_name, unint is_struct, struct token** struct_name) {
    if(read_possible_array(p, len_expr, align_per_el_expr) == FAIL) return FAIL;
    
    if(read_possible_alignemnt(p, align_start_expr) == FAIL) return FAIL;

    // Struct names
    if(is_struct && ((*struct_name = expect_token(p, NAME)) == NULL)) return FAIL;

    if((*field_name = expect_token(p, NAME)) == NULL) return FAIL;

    struct AstStructDecl* struct_decl = &_struct_decl->node.struct_decl;
    for(struct StructDeclField* field = struct_decl->head; field; field = field->next) {
        if(_compare_identifiers(field->name->cps, field->name->len, (*field_name)->cps, (*field_name)->len) == SUCCESS ) {
            _error_from_token(p, *field_name, ERROR_TYPE_MESSAGE, "redeclared struct field");
            return FAIL;
        }
    }

    // Read new line
    if(expect_token(p, NEWLINE) == NULL) return FAIL;

    return SUCCESS;
}

#define IS_SAVE_TYPE(type) ((type) == SAVEB_NODE || (type) == SAVEW_NODE || (type) == SAVEDW_NODE || (type) == SAVEQ_NODE || (type) == SAVEF_NODE || (type) == SAVED_NODE || (type) == SAVEP_NODE)

struct Ast_node* handle_space_identifiers(struct Parser* p, struct token* _token, unint type) {
    struct Ast_node *align_start_expr, *len_expr, *align_per_el_expr;
    struct token* _s = NULL;
    struct token* _e = NULL;
    // struct token* name = NULL;
    struct Ast_node* value = NULL;

    // Save types must have array expression
    if(IS_SAVE_TYPE(type)) {
        struct token* p1 = _peek_token(p);
        if(p1 == NULL) return NULL;

        if(expected_token(p, p1, LSQB) == NULL) return NULL;
        _reset_peek(p);
    }

    if(read_possible_array(p, &len_expr, &align_per_el_expr) == FAIL) return NULL;

    if(read_possible_alignemnt(p, &align_start_expr) == FAIL) return NULL;

    if(!IS_SAVE_TYPE(type)) {
        // Depending on the context

        /* if(is_inside_block(p, CTX_FUNC) == SUCCESS) {
            if((name = expect_token(p, NAME)) == NULL) return NULL;
        } else */{
            value = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
            if(value == NULL) return NULL;
        }

    } /* else if(is_inside_block(p, CTX_FUNC) == SUCCESS) {
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@save- inside function");
        return NULL;
    } */

    // Read new line
    if(expect_token(p, NEWLINE) == NULL) return NULL;

    struct Ast_node* _space = new_ast_space(p, _token, align_start_expr, len_expr, align_per_el_expr, type, /* Extra*/ value, _s, _e/*, name */);
    if(_space == NULL) return NULL;

    return _space;
}

unint expect_data_type(struct Parser* p, unint* data_type) {
    struct token* data_type_tok = _read_token(p);
    if(data_type_tok == NULL) return FAIL;

    if(is_at_identifier(data_type_tok, BYTE_IDENTIFIER) == SUCCESS) *data_type = BYTE_NODE;
    else if(is_at_identifier(data_type_tok, WORD_IDENTIFIER) == SUCCESS) *data_type = WORD_NODE;
    else if(is_at_identifier(data_type_tok, DWORD_IDENTIFIER) == SUCCESS) *data_type = DWORD_NODE;
    else if(is_at_identifier(data_type_tok, QWORD_IDENTIFIER) == SUCCESS) *data_type = QWORD_NODE;
    else if(is_at_identifier(data_type_tok, FLOAT_IDENTIFIER) == SUCCESS) *data_type = FLOAT_NODE;
    else if(is_at_identifier(data_type_tok, DOUBLE_IDENTIFIER) == SUCCESS) *data_type = DOUBLE_NODE;
    else if(is_at_identifier(data_type_tok, PTR_IDENTIFIER) == SUCCESS) *data_type = PTR_NODE;
    else {
        _error_from_token(p, data_type_tok, ERROR_TYPE_MESSAGE, "invalid type");
        return FAIL;  
    }
    return SUCCESS;
}

unint parse_struct_assign_block(struct Parser* p, struct StructAssignField** head, struct StructAssignField** tail) {
    if(expect_token(p, INDENT) == NULL) return FAIL;
    while(true) {
        struct token* p2 = _peek_token(p);
        if(p2 == NULL) return FAIL;

        if(p2->type == DEDENT) {
            _read_token(p);
            return SUCCESS;
        }

        struct token *field_name, *_s, *_e;
        if((field_name = expect_token(p, NAME)) == NULL) return FAIL;

        // Check for duplicate
        for(struct StructAssignField* field = *head; field; field = field->next) {
            if(_compare_identifiers(field->field_name->cps, field->field_name->len, field_name->cps, field_name->len) == SUCCESS ) {
                _error_from_token(p, field_name, ERROR_TYPE_MESSAGE, "duplicate field in stuct variable");
                return FAIL;
            }
        }

        if((p2 = _peek_token(p)) == NULL) return FAIL;

        // Struct
        if(p2->type == NEWLINE) {
            _read_token(p);
            if(insert_struct_field_assignment(p, field_name, head, tail, field_name, NULL) == FAIL) return FAIL;
            if(parse_struct_assign_block(p, &((*tail)->head), &((*tail)->tail)) == FAIL) return FAIL;
            continue;
        }

        // "="
        if((expect_token(p, EQUAL)) == NULL) return FAIL;

        struct Ast_node* val = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(val == NULL) return FAIL;

        if((expect_token(p, NEWLINE)) == NULL) return FAIL;

        if(insert_struct_field_assignment(p, field_name, head, tail, field_name, val) == FAIL) return FAIL;
    }
}
/*
unint _parse_potential_function_call(struct Parser* p, struct token* func_name, struct Ast_node** func_call) {
    *func_call = NULL;
    // Function name hasnt been read yet
    if(func_name == NULL) {
        if((func_name = _peek_token(p)) == NULL) return FAIL;

        if(func_name->type != NAME || is_at_identifier(func_name, NULL) == SUCCESS) return SUCCESS;
    }

    struct token* lpar = _peek_token(p);
    if(lpar == NULL) return FAIL;

    // "("
    if(lpar->type != LPAR) {
        _reset_peek(p);
        return SUCCESS;
    }

    _read_token(p); // "("

    *func_call = new_ast_function_call(p, func_name, func_name);
    if(*func_call == NULL) return FAIL;
    
    unint is_p = 0;
    struct token* p1 = _peek_token(p);
    if(p1 == NULL) return FAIL;

    // No args
    if(p1->type == RPAR) {
        _read_token(p);
        return SUCCESS;
    }

    _reset_peek(p);

    while(true) {
        if((p1 = _peek_token(p)) == NULL) return FAIL;
        // Check for @p
        if(is_at_identifier(p1, P_IDENTIFIER) == SUCCESS) {
            _read_token(p);
            is_p = 1;
        }

        struct token *_s, *_e;
        struct Ast_node* arg = parse_expr_with_start_and_end(p, &_s, &_e, 0, 1);
        if(arg == NULL) return FAIL;

        if(insert_func_call_arg(p, func_name, &((*func_call)->node.func_call), arg, is_p, _s, _e) == FAIL) return FAIL;

        if((p1 = _read_token(p)) == NULL) return FAIL;

        if(p1->type == RPAR) return SUCCESS;
    }
}
*/

unint _parse_potential_indexation(struct Parser* p, struct token* var_name, struct Ast_node** idx_expr) {

    struct token* lsqb = _peek_token(p);
    if(lsqb == NULL) return FAIL;

    if(lsqb->type != LSQB) {
        _reset_peek(p);
        return SUCCESS;
    }

    struct token* rsqb = _peek_token(p);
    if(rsqb == NULL) return FAIL;

    if(rsqb->type == RSQB) {
        _reset_peek(p);
        return SUCCESS;
    }

    _read_token(p); // "["
 
    struct Ast_node* _idx_expr;
    struct token *_s, *_e;
    if((_idx_expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0)) == NULL) return FAIL;

    if(expect_token(p, RSQB) == NULL) return FAIL;

    if(*idx_expr == NULL) {
        struct Ast_node* left = new_ast_variable(p, var_name);
        if(left == NULL) return FAIL;

        *idx_expr = new_ast_binop(p, lsqb, LSQB, left, _idx_expr);
    } else {

        *idx_expr = new_ast_binop(p, lsqb, LSQB, *idx_expr, _idx_expr);
    }
    
    if(*idx_expr == NULL) return FAIL;
    (*idx_expr)->node.binop._s = _s;
    (*idx_expr)->node.binop._e = _e;

    if(_parse_potential_indexation(p, var_name, idx_expr) == FAIL) return FAIL;
    return SUCCESS;
}

unint cstr_equals_codepoints(const char* c_str, const int32_t* cps, unint len) {
    unint i = 0;

    while (true) {
        unsigned char c = (unsigned char)c_str[i];

        if (c == '\0') return (i == len) ? SUCCESS : FAIL;

        if (i >= len) return FAIL;

        if ((int32_t)c != cps[i]) return FAIL;
        i++;
    }
}

#define IS_ASSIGNMENT(_token) (_token == EQUAL || _token == PLUSEQUAL || _token == MINEQUAL || _token == STAREQUAL || _token == SLASHEQUAL || _token == PERCENTEQUAL || _token == AMPEREQUAL || _token == VBAREQUAL || _token == CIRCUMFLEXEQUAL || _token == LEFTSHIFTEQUAL || _token == RIGHTSHIFTEQUAL)

unint _parse_statement(struct Parser* p, struct Ast_node** stmt_ast, unint* flags, unint block_ctx) {
    struct token* _token;
    // Skip comments && new lines
    _reset_peek(p);
    struct token* last_token = p->last_token;

    while ((_token = _read_token(p)) != NULL && (_token->type == NEWLINE)) {
        last_token = _token;
    }

    if (_token == NULL) return FAIL;

    DBG(DO_PARSER_RADOM_STUFF_DBG, "DONE SKIPING (using last)\n");

    // Non AST stuff
    if(is_at_identifier(_token, MACRO_IDENTIDIER) == SUCCESS) {
        struct token* macro_name_token;

        // @macro
        if((macro_name_token = expect_token(p, NAME)) == NULL) return FAIL;

        // '('
        if(expect_token(p, LPAR) == NULL) return FAIL;

        // Already declared?
        if(is_macro_declared(p, macro_name_token->cps, macro_name_token->len) == SUCCESS) {
            _error_from_token(p, macro_name_token, ERROR_TYPE_MESSAGE, "macro is already declared");
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

        DBG(DO_PARSER_RADOM_STUFF_DBG, "-----------------------------------------------------------------\n");
        print_macro(macro);
        DBG(DO_PARSER_RADOM_STUFF_DBG, "-----------------------------------------------------------------\n");

        *flags = FLAG_MACRO_DECL;
        *stmt_ast = NULL;
        return SUCCESS;
    }

    else if (is_at_identifier(_token, INCLUDE_IDENTIFIER) == SUCCESS) {
        
        // Include path
        struct token* _include = _read_token(p);
        if(_include == NULL) return FAIL;

        if(_include->type != STRING) {
            _error_from_token(p, _include, ERROR_TYPE_TYPE, "invalid type for include");
            return FAIL;
        }

        struct token* _e = expect_token(p, NEWLINE);
        if(_e == NULL) return FAIL;

        // Check file
        char* include_path = cp_string_to_encoding(_include->cps+1, _include->len-2); // Remove the quotes
        if(include_path == NULL) {
            _error_from_token(p, _include, ERROR_TYPE_MEMORY, "no available memory for include string");
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
            DBG(DO_PARSER_RADOM_STUFF_DBG, "last_token: [%p:%s:l%d:c%d]\n", last_token, _Parser_TokenNames[last_token->type], last_token->lineno,last_token->col_offset);
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
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(expr == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* assert = new_ast_assert(p, _token, expr, _s, _e);
        if(assert == NULL) return FAIL;

        *stmt_ast = assert;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, WARN_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(expr == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* warn = new_ast_warn(p, _token, expr, _s, _e);
        if(warn == NULL) return FAIL;

        *stmt_ast = warn;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, ERROR_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(expr == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* error = new_ast_error(p, _token, expr, _s, _e);
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

        // Parse the block
        if(_parse_block(p, &_if->node._if.statements, CTX_IF) == FAIL) return FAIL;

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
            if(_parse_block(p, &_if->node._if._elifs_end->statements, CTX_IF) == FAIL) return FAIL;

        }

        if(is_at_identifier(_elif, ELSE_IDENTIFIER) == FAIL) goto _end_if;

        _read_token(p); // Read else
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        // Else
        if(insert_else(p, _elif, _if) == FAIL) return FAIL;  
        
        // Block
        if(_parse_block(p, _if->node._if._else, CTX_IF) == FAIL) return FAIL;
        
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

        // Parse the block
        if(_parse_block(p, &_while->node._while.statements, CTX_LOOP) == FAIL) return FAIL;

        *stmt_ast = _while;
        return SUCCESS;
    }
    
    else if(is_at_identifier(_token, REPEAT_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* iter = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(iter == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* _repeat = new_ast_repeat(p, _token, iter, _s, _e);
        if(_repeat == NULL) return FAIL;

        // Parse the block
        if(_parse_block(p, &_repeat->node._repeat.statements, CTX_LOOP) == FAIL) return FAIL;

        *stmt_ast = _repeat;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, BREAK_IDENTIFIER) == SUCCESS) {
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        if(is_inside_block(p, CTX_LOOP) == FAIL) {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@break outside loop");
            return FAIL;
        }

        struct Ast_node* _break = new_ast_break(p, _token);
        if(_break == NULL) return FAIL;

        *stmt_ast = _break;
        return SUCCESS; 
    }

    else if(is_at_identifier(_token, CONTINUE_IDENTIFIER) == SUCCESS) {
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        if(is_inside_block(p, CTX_LOOP) == FAIL) {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@continue outside loop");
            return FAIL;
        }

        struct Ast_node* _continue = new_ast_continue(p, _token);
        if(_continue == NULL) return FAIL;

        *stmt_ast = _continue;
        return SUCCESS;     
    }
    /*
    else if(is_at_identifier(_token, IMPORT_IDENTIFIER) == SUCCESS) {
        // Include path
        struct token* _import = _read_token(p);
        if(_import == NULL) return FAIL;

        if(_import->type != STRING) {
            _error_from_token(p, _import, ERROR_TYPE_TYPE, "invalid type for import");
            return FAIL;
        }

        struct token* _e = expect_token(p, NEWLINE);
        if(_e == NULL) return FAIL;

        struct Ast_node* _i = new_ast_import(p, _token, _import);
        if(_i == NULL) return FAIL;

        *stmt_ast = _i;
        return SUCCESS;
    }
    */

    else if(is_at_identifier(_token, CODE_IDENTIFIER) == SUCCESS) {
        struct token* name;
        if((name = expect_token(p, NAME)) == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        unint c = langs_count();
        for(unint i=0; i<c; ++i) {
            struct asm_lang_t* l = &asm_langs[i];
            if(cstr_equals_codepoints(l->code_name, name->cps, name->len) == SUCCESS) {    
                struct Ast_node* code = new_ast_code(p, name, l);
                if(code == NULL) return FAIL;

                *stmt_ast = code;
                return SUCCESS;
            }
        }

        _error_from_token(p, name, ERROR_TYPE_MESSAGE, "unknown assembly language");
        return FAIL;
    }

    else if(is_at_identifier(_token, ALIGN_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(expr == NULL) return FAIL;

        struct Ast_node* align = new_ast_align(p, _token, expr, _s, _e);
        if(align == NULL) return FAIL;

        *stmt_ast = align;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, BYTE_IDENTIFIER) == SUCCESS) return   ((*stmt_ast = handle_space_identifiers(p, _token, BYTE_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, WORD_IDENTIFIER) == SUCCESS) return   ((*stmt_ast = handle_space_identifiers(p, _token, WORD_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, DWORD_IDENTIFIER) == SUCCESS) return  ((*stmt_ast = handle_space_identifiers(p, _token, DWORD_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, QWORD_IDENTIFIER) == SUCCESS) return  ((*stmt_ast = handle_space_identifiers(p, _token, QWORD_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, FLOAT_IDENTIFIER) == SUCCESS) return  ((*stmt_ast = handle_space_identifiers(p, _token, FLOAT_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, DOUBLE_IDENTIFIER) == SUCCESS) return ((*stmt_ast = handle_space_identifiers(p, _token, DOUBLE_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, PTR_IDENTIFIER) == SUCCESS) return ((*stmt_ast = handle_space_identifiers(p, _token, PTR_NODE)) == NULL) ? FAIL : SUCCESS;

    else if(is_at_identifier(_token, SAVEB_IDENTIFIER) == SUCCESS) return   ((*stmt_ast = handle_space_identifiers(p, _token, SAVEB_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, SAVEW_IDENTIFIER) == SUCCESS) return   ((*stmt_ast = handle_space_identifiers(p, _token, SAVEW_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, SAVEDW_IDENTIFIER) == SUCCESS) return  ((*stmt_ast = handle_space_identifiers(p, _token, SAVEDW_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, SAVEQ_IDENTIFIER) == SUCCESS) return  ((*stmt_ast = handle_space_identifiers(p, _token, SAVEQ_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, SAVEF_IDENTIFIER) == SUCCESS) return  ((*stmt_ast = handle_space_identifiers(p, _token, SAVEF_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, SAVED_IDENTIFIER) == SUCCESS) return ((*stmt_ast = handle_space_identifiers(p, _token, SAVED_NODE)) == NULL) ? FAIL : SUCCESS;
    else if(is_at_identifier(_token, SAVEP_IDENTIFIER) == SUCCESS) return ((*stmt_ast = handle_space_identifiers(p, _token, SAVEP_NODE)) == NULL) ? FAIL : SUCCESS;

    else if(is_at_identifier(_token, ORG_IDENTIFIER) == SUCCESS) {
        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(expr == NULL) return FAIL;

        struct Ast_node* org = new_ast_org(p, _token, expr, _s, _e);
        if(org == NULL) return FAIL;

        *stmt_ast = org;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, STRUCT_IDENTIFIER) == SUCCESS) {
        // Declaration or var
        struct token* struct_name;
        if((struct_name = expect_token(p, NAME)) == NULL) return FAIL;

        struct token* p1 = _peek_token(p);
        if(p1 == NULL) return FAIL;

        // Struct Variable
        if(p1->type != NEWLINE) {
            struct token* struct_var_name;
            if((struct_var_name = expect_token(p, NAME)) == NULL) return FAIL;

            if(expect_token(p, NEWLINE) == NULL) return FAIL;

            struct Ast_node* struct_var = new_ast_struct_var(p, _token, struct_name, struct_var_name);
            if(struct_var == NULL) return FAIL;

            // Both functions / other contexts can have struct variable without values
            struct token* p1 = _peek_token(p);
            if(p1 == NULL) return FAIL;
            _reset_peek(p);

            if(p1->type == INDENT /*&& is_inside_block(p, CTX_FUNC) == FAIL */) {
                if(parse_struct_assign_block(p, &struct_var->node.struct_var.head, &struct_var->node.struct_var.tail) == FAIL) return FAIL;
            }
            _reset_peek(p);

            *stmt_ast = struct_var;
            return SUCCESS; 
        }

        /*
        if(is_inside_block(p, CTX_FUNC) == SUCCESS) {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@struct declaration inside function");
            return FAIL;   
        }
        */

        // Struct Declaration
        _read_token(p);
        if(expect_token(p, INDENT) == NULL) return FAIL;

        // new ast struct decl
        struct Ast_node* struct_decl = new_ast_struct_decl(p, _token, struct_name);
        if(struct_decl == NULL) return FAIL;

        while(true) {
            struct Ast_node *align_start_expr, *len_expr, *align_per_el_expr;
            struct token *field_name, *struct_name;
            unint data_type;

            struct token* type = _read_token(p);
            if(type == NULL) return FAIL;

            if(type->type == DEDENT) {
                *stmt_ast = struct_decl;
                return SUCCESS;
            } 

            else if(is_at_identifier(type, BYTE_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = BYTE_NODE;
            }

            else if(is_at_identifier(type, WORD_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = WORD_NODE;
            }

            else if(is_at_identifier(type, DWORD_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = DWORD_NODE;
            }

            else if(is_at_identifier(type, QWORD_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = QWORD_NODE;
            }

            else if(is_at_identifier(type, FLOAT_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = FLOAT_NODE;
            }

            else if(is_at_identifier(type, DOUBLE_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = DOUBLE_NODE;
            }

            else if(is_at_identifier(type, PTR_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 0, &struct_name) == FAIL) return FAIL;
                data_type = PTR_NODE;
            }

            else if(is_at_identifier(type, STRUCT_IDENTIFIER) == SUCCESS) {
                if(handle_struct_decl_field(p, struct_decl, &align_start_expr, &len_expr, &align_per_el_expr, &field_name, 1, &struct_name) == FAIL) return FAIL;
                data_type = STRUCT_DECL_NODE;
            }

            else {
                _error_from_token(p, type, ERROR_TYPE_MESSAGE, "invalid type");
                return FAIL;      
            }

            // Append node to struct declaration
            if(insert_struct_field(p, type, &struct_decl->node.struct_decl, field_name, struct_name, data_type, len_expr, align_per_el_expr, align_start_expr) == FAIL) return FAIL;
        }
    }

    else if(is_at_identifier(_token, STRING_IDENTIFIER) == SUCCESS) {

        struct Ast_node* align_start_expr;
        if(read_possible_alignemnt(p, &align_start_expr) == FAIL) return FAIL;

        struct token *_s, *_e;
        struct Ast_node* expr = parse_expr_with_start_and_end(p, &_s, &_e, 0, 0);
        if(expr == NULL) return FAIL;

        // Read new line
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        /*
        if(is_inside_block(p, CTX_FUNC) == SUCCESS) {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@string inside function");
            return FAIL;
        }
        */

        struct Ast_node* string = new_ast_at_string(p, _token, expr, _s, _e, align_start_expr);
        if(string == NULL) return FAIL;

        *stmt_ast = string;
        return SUCCESS;     
    }

    else if(is_at_identifier(_token, DEL_IDENTIFIER) == SUCCESS) {
        struct token* ident;

        if((ident = expect_token(p, NAME)) == NULL) return FAIL;
        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        struct Ast_node* del = new_ast_del(p, _token, ident);
        if(del == NULL) return FAIL;

        *stmt_ast = del;
        return SUCCESS;
    }
    /*
    else if(is_at_identifier(_token, RETURN_IDENTIFIER) == SUCCESS) {
        struct token* ident = _peek_token(p);
        if(ident == NULL) return FAIL;

        if(ident->type == NEWLINE) ident = NULL;
        else if((ident = expect_token(p, NAME)) == NULL) return FAIL;

        if(expect_token(p, NEWLINE) == NULL) return FAIL;
        
        struct Ast_node* _return = new_ast_return(p, _token, ident);
        if(_return == NULL) return FAIL;

        if(is_inside_block(p, CTX_FUNC) == FAIL) {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@return outside function");
            return FAIL;   
        }

        *stmt_ast = _return;
        return SUCCESS;
    }

    else if(is_at_identifier(_token, FUN_IDENTIFIER) == SUCCESS) {
        if(expect_token(p, LESS) == NULL) return FAIL;

        struct token* calling_conv;
        if((calling_conv = expect_token(p, NAME)) == NULL) return FAIL;

        if(expect_token(p, GREATER) == NULL) return FAIL;

        struct token* func_name;
        if((func_name = expect_token(p, NAME)) == NULL) return FAIL;

        if(expect_token(p, LPAR) == NULL) return FAIL;

        struct Ast_node* fun_decl = new_ast_fun_decl(p, _token, calling_conv, func_name, VOID_RETURN_TYPE);
        if(fun_decl == NULL) return FAIL;

        unint data_type;

        // Arguments
        struct token* p1 = _peek_token(p);
        if(p1->type != RPAR) {
            _reset_peek(p);
            while(true) {
                // Data type
                if(expect_data_type(p, &data_type) == FAIL) return FAIL;

                // Argument name
                struct token* arg_name;
                if((arg_name = expect_token(p, NAME)) == NULL) return FAIL;

                // Check for repeated
                struct AstFuncDecl* _fun_decl = &fun_decl->node.fun_decl;
                for(struct FuncDeclArg* arg = _fun_decl->head; arg; arg = arg->next) {
                    if(_compare_identifiers(arg->arg_name->cps, arg->arg_name->len, arg_name->cps, arg_name->len) == SUCCESS ) {
                        _error_from_token(p, arg_name, ERROR_TYPE_MESSAGE, "duplicate argument name");
                        return FAIL; 
                    }
                }

                if(insert_fun_decl_arg(p, _token, _fun_decl, arg_name, data_type) == FAIL) return FAIL;

                // Do args end?
                if((p1 = _peek_token(p)) == NULL) return FAIL;
                _reset_peek(p);

                if(p1->type == RPAR) break;
                else if(expect_token(p, COMMA) == NULL) return FAIL;

            }
        }

        if(expect_token(p, RPAR) == NULL) return FAIL;

        // return type
        struct token* return_type_tok = _peek_token(p);
        if(return_type_tok == NULL) return FAIL;

        fun_decl->node.fun_decl.return_type = VOID_RETURN_TYPE;
        if(return_type_tok->type != NEWLINE) {
            if(expect_data_type(p, &data_type) == FAIL) return FAIL;
            fun_decl->node.fun_decl.return_type = data_type;
        }

        if(expect_token(p, NEWLINE) == NULL) return FAIL;

        if(is_inside_block(p, CTX_FUNC) == SUCCESS) {
            _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "@fun inside function");
            return FAIL;
        }

        if(_parse_block(p, &fun_decl->node.fun_decl.statements, CTX_FUNC) == FAIL) return FAIL;

        *stmt_ast = fun_decl;
        return SUCCESS;

    }
    */
    else if(_token->type == NAME && is_at_identifier(_token, NULL) == FAIL) {
        unint level = 0;

        // Assignment & Labels & function call
        struct token* _t = _peek_token(p);
        if(_t == NULL) return FAIL;
        _reset_peek(p);

        // Labels
        if(_t->type == COLON) {
            _read_token(p);

            if(expect_token(p, NEWLINE) == NULL) return FAIL;

            //struct Ast_node* label = new_ast_label(p, _token, _token, is_inside_block(p, CTX_FUNC) == SUCCESS);
            struct Ast_node* label = new_ast_label(p, _token, _token);
            if(label == NULL) return FAIL;

            *stmt_ast = label;
            return SUCCESS;
        } 
        
        /*
        struct Ast_node* func_call;
        if(_parse_potential_function_call(p, _token, &func_call) == FAIL) return FAIL;
        
        if(func_call != NULL) {
            if(expect_token(p, NEWLINE) == NULL) return FAIL;
            *stmt_ast = func_call;
            return SUCCESS;
        }
        */

        // Assignment
        while(true) {
            if((_t = _peek_token(p)) == NULL) return FAIL;

            else if(_t->type == LPAR || _t->type == LSQB) ++level;
            else if(_t->type == RPAR || _t->type == RSQB) --level;
            else if(_t->type == NEWLINE) break;
            else if((IS_ASSIGNMENT(_t->type)) && level == 0) {
                _reset_peek(p); // Go back

                struct Ast_node* expr;
                unint node_type = ASSIGN_VAR_NODE;

                // Possible indexation
                struct Ast_node* idx_expr = NULL;
                if(_parse_potential_indexation(p, _token, &idx_expr) == FAIL) return FAIL;
                _reset_peek(p);

                DBG(DO_PARSER_RADOM_STUFF_DBG, "PARSER INDEXATION\n");

                // Possible append
                struct token* p1 = _read_token(p);
                if(p1 == NULL) return FAIL;


                if(p1->type == LSQB) {
                    // Next token must be "]"
                    if(expect_token(p, RSQB) == NULL) return FAIL;
                    node_type = ASSIGN_APPEND_ARRAY_NODE;

                    if((p1 = _read_token(p)) == NULL) return FAIL;
                }


                // Expect a valid assignment
                if(IS_ASSIGNMENT(p1->type)) {
                    DBG(1, "STARTING EXPRE PARSING\n");
                    expr = _parse_expr(p, (unint)(0), 0);
                    if(expr == NULL) {
                        DBG(DO_PARSER_RADOM_STUFF_DBG, "Error building expression\n");
                        return FAIL;
                    }
                    dbg_ast(expr);
                    DBG(1, "DONE EXPRE PARSING\n");
                    // expect new line
                    if(expect_token(p, NEWLINE) == NULL) return FAIL;

                    *stmt_ast = NULL;
                    if(node_type == ASSIGN_APPEND_ARRAY_NODE && p1->type != EQUAL) goto _invalid_assignment;

                    *stmt_ast = new_ast_assign_variable(p, _token, p1, idx_expr, expr, p1->type, node_type);

                    return (*stmt_ast == NULL) ? FAIL : SUCCESS;
                }
_invalid_assignment:
                // Invalid assignment
                _error_from_token(p, p1, ERROR_TYPE_MESSAGE, "invalid assignment");
                return FAIL;
            }
        }

        _reset_peek(p); // Go back
        // Instruction
        DBG(1, "PARSING INSTRUCTION\n");
        struct Ast_node* instruction = new_ast_instruction(p, _token, _token/*, is_inside_block(p, CTX_FUNC) == SUCCESS*/);
        if(instruction == NULL) return FAIL;

        struct token* p1 = _peek_token(p);
        if(p1 == NULL) return FAIL;

        if(p1->type == NEWLINE) {
            _read_token(p);
            *stmt_ast = instruction;
            return SUCCESS;
        }

        // Args

        while(true) {
            struct token* _s = NULL;
            struct token* _e = NULL;

            while (true) {
                unint level = 0;

                if ((p1 = _read_token(p)) == NULL) return FAIL;

                if(is_at_identifier(p1, NULL) == SUCCESS) {
                    _error_from_token(p, p1, ERROR_TYPE_MESSAGE, "instructions cannot contain at identifiers");
                    return FAIL;
                }
                else if(level == 0 && (p1->type == COMMA || p1->type == NEWLINE)) break;

                if(!_s) _s = p1;
                _e = p1;
            
                if (p1->type == LPAR) ++level;
                else if (p1->type == RPAR) --level;
            }

            if(insert_instruction_arg(p, _token, &instruction->node.instruction, _s, _e) == FAIL) return FAIL;
            if(p1->type == NEWLINE) break;
        }

        *stmt_ast = instruction;
        return SUCCESS;
    }

    else if (_token->type == ENDMARKER) {
        DBG(DO_PARSER_RADOM_STUFF_DBG, "END OF PARSING\n");
        *stmt_ast = NULL;
        return SUCCESS;
    } 
    
    else if (_token->type == INDENT){
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "unexpected indent");
        return FAIL;
    }    
    
    else {
        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "invalid syntax");
        return FAIL;
    }
    return FAIL;
}


struct Ast_node* _run_parser(struct Parser* p) {
    struct Ast_node* ast = new_ast(p);
    if(ast == NULL) return NULL;

    if(_parse_block(p, &ast->node.statements, CTX_GLOBAL) == FAIL) return NULL;
    return ast;
}