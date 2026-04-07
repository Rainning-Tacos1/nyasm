#include <stdarg.h>

#include "api/debug.h"
#include "api/memory.h"
#include "api/unicode.h"

#include "err.h"
#include "token.h"
#include "ctype.h"
#include "types.h"
#include "lexer.h"
#include "config.h"

#define is_potential_identifier_start(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || c == '_'\
               || (c >= 128))

#define is_potential_identifier_char(c) (\
              (c >= 'a' && c <= 'z')\
               || (c >= 'A' && c <= 'Z')\
               || (c >= '0' && c <= '9')\
               || c == '_'\
               || (c >= 128))

#define ADVANCE_LINENO() \
            tok->lineno++; \
            tok->col_offset = 0;

#define RETURN_NOMEM()        \
    do {                       \
        tok->done = E_NOMEM;   \
        return ERRORTOKEN;     \
    } while (0)

#define ISSTRINGLIT(x) ((x) == STRING)
#define TOKEN_ALLOC_NEEDED(tok) ((tok) == NAME || (tok) == NUMBER || (tok) == STRING)

const char * const _Parser_TokenNames[] = {
    "ENDMARKER",
    "NAME",
    "NUMBER",
    "STRING",
    "NEWLINE",
    "INDENT",
    "DEDENT",
    "LPAR",
    "RPAR",
    "LSQB",
    "RSQB",
    "COLON",
    "COMMA",
    "SEMI",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "VBAR",
    "AMPER",
    "LESS",
    "GREATER",
    "EQUAL",
    "DOT",
    "PERCENT",
    "LBRACE",
    "RBRACE",
    "EQEQUAL",
    "NOTEQUAL",
    "LESSEQUAL",
    "GREATEREQUAL",
    "TILDE",
    "CIRCUMFLEX",
    "LEFTSHIFT",
    "RIGHTSHIFT",
    "DOUBLESTAR",
    "PLUSEQUAL",
    "MINEQUAL",
    "STAREQUAL",
    "SLASHEQUAL",
    "PERCENTEQUAL",
    "AMPEREQUAL",
    "VBAREQUAL",
    "CIRCUMFLEXEQUAL",
    "LEFTSHIFTEQUAL",
    "RIGHTSHIFTEQUAL",
    "DOUBLESTAREQUAL",
    "DOUBLESLASH",
    "DOUBLESLASHEQUAL",
    "AT",
    "ATEQUAL",
    "RARROW",
    "ELLIPSIS",
    "COLONEQUAL",
    "EXCLAMATION",
    "OP",
    "TYPE_IGNORE",
    "TYPE_COMMENT",
    "SOFT_KEYWORD",
    "FSTRING_START",
    "FSTRING_MIDDLE",
    "FSTRING_END",
    "TSTRING_START",
    "TSTRING_MIDDLE",
    "TSTRING_END",
    "COMMENT",
    "NL",
    "<ERRORTOKEN>",
    "<ENCODING>",
    "<N_TOKENS>",
};


/*
  Initializes the buffer used to store the cp's of tokens
  Returns:
    NULL : Memory error
    X: memory of the allocated buffer
*/
int32_t* token_cp_buffer_init(struct tok_state* tok) {
    tok->token_cp_buffer_idx = 0;
    tok->token_cp_buffer_size = TOKEN_CP_BUFFER_INIT_SIZE;
    return (int32_t*)MEM_ALLOC(TOKEN_CP_BUFFER_INIT_SIZE * sizeof(int32_t), "token");
}

/*
  Pushes a codepoint onto the buffer, resizes if needed
  (OPERATES ON THE LAST ALLOCATION)
  Returns:
    NULL : Memory error
    X: memory of the allocated buffer (it will allways be the same returned by token_cp_buffer_init)
*/
int32_t* token_push_cp(struct tok_state* tok, int32_t cp) {
    int32_t* mem = MEM_GET_LAST();
    
    // Need more space?
    if(tok->token_cp_buffer_idx == tok->token_cp_buffer_size) {
        tok->token_cp_buffer_size = tok->token_cp_buffer_size + (tok->token_cp_buffer_size >> 1); // 1.5x growth

        mem = (int32_t*)MEM_RESIZE_LAST(tok->token_cp_buffer_size * sizeof(int32_t));
        if(mem == NULL) return NULL;
    }

    // Push the code point
    mem[tok->token_cp_buffer_idx++] = cp;
    return mem;
}

/*
  Trims the end of the token codepoint buffer to remove unwanted space
  (OPERATES ON THE LAST ALLOCATION)
  Returns:
    NULL : Memory error
    X: memory of the allocated buffer (it will allways be the same returned by token_cp_buffer_init)
*/
int32_t* token_trim_cp_buffer(struct tok_state* tok) {
    tok->token_cp_buffer_size = tok->token_cp_buffer_idx;
    return (int32_t*)MEM_RESIZE_LAST(tok->token_cp_buffer_idx * sizeof(int32_t));
}

void tok_state_init(struct tok_state* tok) {
    for(unint i=0; i<MAX_INDENT; ++i) tok->altindstack[i] = tok->indstack[i] = 0;
    for(unint i=0; i<MAX_PARENTHESES_LEVEL; ++i) tok->parencolstack[i] = tok->parenlinenostack[i] = tok->parenstack[0] = 0;

    tok->lineno = tok->first_lineno = tok->implicit_newline = tok->level = tok->starting_col_offset = tok->col_offset = tok->pendin = tok->indent = 0;
    tok->atbol = 1;
    tok->multi_line_start = tok->inp = tok->line_start = tok->source = tok->start = tok->end = NULL;
    tok->done = E_OK;
    UNICODE_INIT(&tok->uc);
}

/*
  Syntax error functions

*/

/*
  In python its implemented in python
  stype: error type (SyntaxError)
  msg: error message
  filename: file name
  ...
  text: the line of code that caused the error
  end: points to the end of the line of code that caused the error
  vargs args for formating the msg string
*/
void _format_syntax_error(const char* stype, const char* msg, const char* filename, nint* lineno, nint* offset, nint* end_lineno, nint* end_offset, const char* text, const char* end, va_list vargs) {
    // char* filename_sufix = "";
    if(lineno != NULL) {
        LOG(
            "  File: \"%s\", line %d\n", 
            filename != NULL ? filename : "<string>",
            *lineno
        );
    } else if(filename != NULL) { /* Handled later */}

    if(text != NULL && end != NULL) {
        struct unicode uc;
        int32_t* cp = &uc.cp;
        
        UNICODE_INIT(&uc);
        uc.buf = uc.curr = text;
        uc.end = end;
        
        
        const char* ltext = NULL;
        const char* rtext = NULL;
        (void)rtext; // Unused

        unint lspaces = 0;
        unint rspaces = 0;
        unint spaces = 0;
        unint len = 0;
        
        // Iterate through the text
        while(uc.curr != uc.end) {
            const char* _start = uc.curr;

            nbool suc = READ_CP(&uc);
        
            if(suc != SUCCESS) { *cp = 0xFFFD; uc.curr = _start + 1;}

            // Inc len
            ++len;

            // lstrip logic
            if(ltext == NULL) {
                if(*cp == ' ' || *cp == '\n' || *cp == '\f') { ++lspaces; continue; }

                ltext = _start; // The prev char
            }

            // rstrip logic
            if(*cp != '\n') { rtext = _start; rspaces = 0; } // The prev char
            else ++rspaces;
        }
        spaces = lspaces + rspaces;

        if(offset == NULL) {
            LOG("    ");
            if(ltext == NULL) LOG("\n");
            else {
                struct unicode uc;
                int32_t* cp = &uc.cp;
                
                UNICODE_INIT(&uc);
                uc.buf = uc.curr = ltext;
                uc.end = end;

                // Iterate through the text
                while(uc.curr != uc.end) {
                    const char* _start = uc.curr;

                    nbool suc = READ_CP(&uc);
                
                    if(suc != SUCCESS) { *cp = 0xFFFD; uc.curr = _start + 1;}
                    LOG_CP(*cp);
                }
                LOG("\n");

            }
        } else {
            nint _offset = *offset;
            nint _end_offset = 0;
            
            if(*lineno == *end_lineno) _end_offset = (*end_offset != 0) ? *end_offset : *offset;
            else _end_offset = (len - rspaces) + 1;
            
            if(text != NULL && _offset > (nint)len) _offset = (len - rspaces) + 1;
            if(text != NULL && _end_offset > (nint)len) _end_offset = (len - rspaces) + 1;
            if(_offset >= _end_offset || _end_offset < 0) _end_offset = _offset + 1;
            
            nint colno = _offset - 1 - spaces;
            nint end_colno = _end_offset - 1 - spaces;

            if(colno >= 0) {
                struct unicode uc;
                int32_t* cp = &uc.cp;

                unint dp_colno = 0;

                UNICODE_INIT(&uc);
                uc.end = uc.buf = uc.curr = ltext;
                uc.end += colno;
                
                // Iterate through the text
                while(uc.curr != uc.end) {
                    const char* _start = uc.curr;

                    nbool suc = READ_CP(&uc);
                    
                    if(suc != SUCCESS) { *cp = 0xFFFD; uc.curr = _start + 1;}
                    dp_colno += (CP_WIDTH(*cp) == 2) ? 2 : 1;
                }

                nint highlighted = end_colno - colno;

                unint caret_count = 0;
                if(!highlighted) caret_count = (end_colno - colno);
                else {
                    UNICODE_INIT(&uc);
                    uc.buf = uc.curr = ltext + colno;
                    uc.end = ltext + end_colno;

                    // Iterate through the text
                    while(uc.curr != uc.end) {
                        const char* _start = uc.curr;

                        nbool suc = READ_CP(&uc);
                        
                        if(suc != SUCCESS) { *cp = 0xFFFD; uc.curr = _start + 1;}
                        caret_count += (CP_WIDTH(*cp) == 2) ? 2 : 1;
                    }
                }

                // Print the cursor
                // Repeated code
                LOG("    ");
                if(ltext == NULL) LOG("\n");
                else {
                    UNICODE_INIT(&uc);
                    uc.buf = uc.curr = ltext;
                    uc.end = end;

                    // Iterate through the text
                    while(uc.curr != uc.end) {
                        const char* _start = uc.curr;

                        nbool suc = READ_CP(&uc);
                        if(suc != SUCCESS) { *cp = 0xFFFD; uc.curr = _start + 1;}
                        LOG_CP(*cp);
                    }
                    LOG("\n");

                }

                LOG("    ");
                for(nint i=0; i<dp_colno; ++i) LOG(" ");
                for(nint i=0; i<caret_count; ++i) LOG("^");
                LOG("\n");

            } else {
                // Repeated code
                LOG("    ");
                if(ltext == NULL) LOG("\n");
                else {
                    struct unicode uc;
                    int32_t* cp = &uc.cp;
                    
                    UNICODE_INIT(&uc);
                    uc.buf = uc.curr = ltext;
                    uc.end = end;

                    // Iterate through the text
                    while(uc.curr != uc.end) {
                        const char* _start = uc.curr;

                        nbool suc = READ_CP(&uc);
                    
                        if(suc != SUCCESS) { *cp = 0xFFFD; uc.curr = _start + 1;}
                        LOG_CP(*cp);
                    }
                    LOG("\n");

                }                
            }
        }

    }
    LOG("%s: ", stype);

    VLOG(msg != NULL ? msg : "<no detail available>", vargs);

    if(lineno == NULL && filename != NULL) LOG("(%s)", filename);
    LOG("\n");
}

unint _syntaxerror_range(struct tok_state *tok, const char *format, nint col_offset, nint end_col_offset, va_list vargs) {
    // Not needed for now
    if (tok->done == E_ERROR) {
        return ERRORTOKEN;
    }

    // Save
    const char* _curr =  tok->uc.curr;

    struct unicode _uc = tok->uc;
    int32_t* _cp = &_uc.cp;
    unint len = 0;

    // Line with error
    _uc.curr = tok->line_start;
    do {
        nbool suc = READ_CP(&_uc);

        if(suc != SUCCESS) { *_cp = 0xFFFD; ++_uc.curr; } // print replacement and skip

        if(_uc.curr <= _curr) ++len;

    } while(*_cp != '\n');

    // Cursor
    if (col_offset == -1) col_offset = len;
    if (end_col_offset == -1) end_col_offset = col_offset;

    // call func
    _format_syntax_error("SyntaxError", format, tok->source, &tok->lineno, &col_offset, &tok->lineno, &end_col_offset, tok->line_start, _uc.curr - _uc.nread, vargs);

    tok->done = E_ERROR;
    return ERRORTOKEN;
}

unint _Tokenizer_syntaxerror(struct tok_state *tok, const char *format, ...) {
    va_list vargs;
    va_start(vargs, format);
    unint ret = _syntaxerror_range(tok, format, -1, -1, vargs);
    va_end(vargs);
    return ret;
}

unint _Tokenizer_syntaxerror_known_range(struct tok_state *tok, nint col_offset, nint end_col_offset, const char *format, ...) {
    va_list vargs;
    va_start(vargs, format);
    unint ret = _syntaxerror_range(tok, format, col_offset, end_col_offset, vargs);
    va_end(vargs);
    return ret;
}

unint _Tokenizer_indenterror(struct tok_state *tok) {
    tok->done = E_TABSPACE;
    tok->uc.curr = tok->inp; // tok->inp = End of line
    return ERRORTOKEN;    
}


/* ----------- */

unint _Lexer_token_setup(struct tok_state *tok, struct token *token, unint type, const char *start, const char *end) {
    token->level = tok->level;
    if (ISSTRINGLIT(type)) {
        token->lineno = tok->first_lineno;
    }
    else {
        token->lineno = tok->lineno;
    }
    token->end_lineno = tok->lineno;
    token->col_offset = token->end_col_offset = -1;
    token->start = start;
    token->end = end;
    token->cps = NULL;
    token->len = 0;

    if (start != NULL && end != NULL) {
        token->col_offset = tok->starting_col_offset;
        token->end_col_offset = tok->col_offset;
    }

    unint size = end - start;

    DBG(DO_LEXER_TOKEN_DBG, "[%s]: %d bytes '", _Parser_TokenNames[type], size);
    if(start == NULL || end == NULL) DBG(DO_LEXER_TOKEN_DBG, "<NULL>");
    else for(const char* i=start; i<end; ++i) DBG(DO_LEXER_TOKEN_DBG, "%c", *i);
    DBG(DO_LEXER_TOKEN_DBG, "'\n");

    if(start == NULL || end == NULL) return type;

    int32_t* token_buf = NULL;

    if(TOKEN_ALLOC_NEEDED(type) && start != NULL && end != NULL) {
        // Allocate
        token_buf = token_cp_buffer_init(tok);
        if(token_buf == NULL) RETURN_NOMEM();

        // Fill
        struct unicode _uc;
        int32_t* _cp = &_uc.cp;
        _uc.curr = _uc.buf = start;
        _uc.end = end;

        while(READ_CP(&_uc) == SUCCESS && *_cp != EOF) {
            DBG(DO_LEXER_TOKEN_FILL_DBG, "Filling token with: '%s' (U+%04X)\n", CP_TO_ENCODING(*_cp), *_cp);
            token_buf = token_push_cp(tok, *_cp);
            if(token_buf == NULL) RETURN_NOMEM();
            ++token->len;
        }

        token_buf = token_trim_cp_buffer(tok);
        if(token_buf == NULL) RETURN_NOMEM();

        token->cps = token_buf;
    }

    // Normalization for names
    if(type == NAME) {
        nint _len = NORMALIZE_CP(token->cps, token->len);
        if(_len < 0) return _Tokenizer_syntaxerror(tok, "error normalizing token");

        // Normalization was needed
        if(_len != 0) {
            token->len = _len;

            // Manually override
            tok->token_cp_buffer_idx = _len;
            token_buf = token_trim_cp_buffer(tok);
            if(token_buf == NULL) RETURN_NOMEM();
        }
    }

    return type; 
}

#define MAKE_TOKEN(token_type) _Lexer_token_setup(tok, token, token_type, p_start, p_end)


/*
  Tries to read the next code point
  Python uses a system where a line is stored in a buffer and when the buffer is at the end it is replaced(tok->underflow) with the next line.
  Python also checks for UTF8 errors on the line read.

  The implicit new line logic is a bit dirty, and poorly written.
*/

void next_cp(struct tok_state* tok) {

    int32_t* cp = &tok->uc.cp;
    struct unicode* uc = &tok->uc;

    if(tok->done != E_OK) *cp = EOF;

    if (uc->curr != tok->inp && (unint) tok->col_offset >= (unint) NINT_MAX) {
        tok->done = E_COLUMNOVERFLOW;
        uc->nread = 0;
        uc->cp = EOF;
        return;
    }

    ++tok->col_offset;

    // Unfortunatelly do whole line verification for badly encoded characters
    
    // Add !tok->implicit_newline to stop it from doing a check on just the EOF after the implicit newline
    if(uc->curr == tok->inp && !tok->implicit_newline) { // First time or at the end of a line
        // Save uc values
        struct unicode _uc = *uc;
        int32_t* _cp = &_uc.cp;
        
        int32_t prev = 0;
        nint col_offset = 0;
        
        tok->line_start = uc->curr;
        
        DBG(DO_LEXER_CHAR_VERIFICATION_LOOKAHEAD_DBG, "Verification start:\n");
        // Read & verify a line or stop at EOF
        while (true) {
            nbool suc = READ_CP(&_uc);

            DBG(DO_LEXER_CHAR_VERIFICATION_LOOKAHEAD_DBG, "\tread/verifying: '%s', %d\n", CP_TO_ENCODING(*_cp), *_cp);
            
            if(suc != SUCCESS){
                (_uc.err == UNICODE_ERR_CODEPOINT) ?
                    _Tokenizer_syntaxerror_known_range(tok, col_offset+1, col_offset+1, "malformed character encoding") :
                    _Tokenizer_syntaxerror_known_range(tok, col_offset+1, col_offset+1, "unknown Unicode error");

                tok->done = E_DECODE;
                *cp = EOF;
                return;
            }

            tok->done = E_OK;
            ++col_offset;

            if(prev == '\r') {
                if (*_cp != '\n') BACKUP_CP(&_uc);
                break;
            }
            
            prev = *_cp;
            if (*_cp == EOF) { tok->done = E_EOF; break; }
            if (*_cp == '\n') break;
            if (*_cp == '\r') continue;
        }
        DBG(DO_LEXER_CHAR_VERIFICATION_LOOKAHEAD_DBG, "Verification complete\n\n");
        tok->inp = _uc.curr;

        tok->implicit_newline = 0;
        if(*_cp != '\n') { // Line did not end on an newline
            DBG(1, "Signaling implicit newline\n");
            // Python signals the implicit new line as it reloads the buffer, so we also do that here
            tok->implicit_newline = 1;
            unint size = WRITE_IMPLICIT_NL((char*)_uc.curr);
            uc->end += size;
            tok->inp += size;
        }

        ADVANCE_LINENO();

    }

    READ_CP(uc); // It is safe to not check for errors
    DBG(DO_LEXER_CHAR_DBG, "next_cp read: '%s', (U+%04X)\n", CP_TO_ENCODING(*cp), *cp);
}

nbool backup_cp(struct tok_state* tok) {
    if(tok->col_offset > 0) --tok->col_offset;
    nbool _ = BACKUP_CP(&tok->uc);

    DBG(DO_LEXER_CHAR_DBG, "backing up a codepoint, result = %s\n\n", (_ == SUCCESS) ? "SUCCESS" : "FAIL");
    return _;
}

/*
  Parse indentation and update tokenizer state
  Returns:
    0 : Success
    1 : Indentation error
*/
nbool get_indentation(struct tok_state* tok, unint* blankline) {
    unint col = 0;
    unint altcol = 0;

    int32_t* cp = &tok->uc.cp;

    for(;;) {
        next_cp(tok);
        if(*cp == ' ') { ++col; ++altcol; }
        else if(*cp == '\t') {
            col = (col / TAB_SIZE + 1) * TAB_SIZE;
            altcol = (altcol / ALT_TAB_SIZE + 1) * ALT_TAB_SIZE;
        }
        // No formfeed
        // Handle multiline
        else if(*cp == EOF && tok->uc.err != UNICODE_OK) return FAIL;
        else break;
    }
    // Blank lines; Comments ignore indentation
    if(*cp == '#' || *cp == '\n' || *cp == '\r') *blankline = 1;
    else if(*cp == '/') {
        next_cp(tok);
        if(*cp == '*') *blankline = 1;
        backup_cp(tok); // No problem on trying to backup and EOF/error

    }
    backup_cp(tok);

    // col = cont_line_col ? cont_line_col : col;
    // altcol = cont_line_col ? cont_line_col : altcol;
    if(!*blankline && tok->level == 0) {
        col = col;
        altcol = altcol;

        if(col == tok->indstack[tok->indent]) {
            if (altcol != tok->altindstack[tok->indent]) DO_FAIL(_Tokenizer_indenterror(tok));
        } else if(col > tok->indstack[tok->indent]) {
            if (tok->indent+1 >= MAX_INDENT) {
                tok->done = E_TOODEEP;
                tok->uc.curr = tok->inp;
                return FAIL;
            }
            if (altcol <= tok->altindstack[tok->indent]) DO_FAIL(_Tokenizer_indenterror(tok));
            tok->pendin++;
            tok->indstack[++tok->indent] = col;
            tok->altindstack[tok->indent] = altcol;
        } else {
            while (tok->indent > 0 &&
                col < tok->indstack[tok->indent]) {
                tok->pendin--;
                tok->indent--;
            }
            if (col != tok->indstack[tok->indent]) {
                tok->done = E_DEDENT;
                tok->uc.curr = tok->inp;
                return FAIL;
            }
            if (altcol != tok->altindstack[tok->indent]) DO_FAIL(_Tokenizer_indenterror(tok));
        }
    }

    return (tok->uc.err == UNICODE_OK) ? SUCCESS : FAIL;
}

/*
  Tells if a indent or a dedent needs to be generated
  Pair this function with a check for tok->pendin == 0
  Returns:
    indent or dedent
*/
unint generate_indent_dedent_token(struct tok_state* tok, struct token* token) {
    if (tok->pendin < 0) {
        tok->pendin++;
        return DEDENT;
    }
    else {
        tok->pendin--;
        return INDENT;
    }
}

/*
  Tells if a code point is a valid identifier start
  Allow: characters and '_'
*/
unint is_identifier_start(int32_t cp) {
    unint cat = UNICODE_CAT(cp);

    if (
        cp == '_' ||

        cat == UNICODE_CATEGORY_LU ||
        cat == UNICODE_CATEGORY_LL ||
        cat == UNICODE_CATEGORY_LT ||
        cat == UNICODE_CATEGORY_LM ||
        cat == UNICODE_CATEGORY_LO
    ) {
        return SUCCESS;
    }
    else {
        return FAIL;
    }
}

unint is_printable_cp(int32_t cp) {
    unint cat = UNICODE_CAT(cp);

    // Non-printable categories
    if (cat == UNICODE_CATEGORY_CC ||  // Control
        cat == UNICODE_CATEGORY_CF ||  // Format
        cat == UNICODE_CATEGORY_CS ||  // Surrogate
        cat == UNICODE_CATEGORY_CO ||  // Private use
        cat == UNICODE_CATEGORY_CN ||  // Unassigned
        cat == UNICODE_CATEGORY_ZL ||  // Line separator
        cat == UNICODE_CATEGORY_ZP)    // Paragraph separator
    {
        return FAIL;
    }

    // Exclude Unicode non-characters
    if ((cp & 0xFFFF) >= 0xFFFE || (cp >= 0xFDD0 && cp <= 0xFDEF)) return FAIL;

    return SUCCESS;
}

/*
  Tells if a code point is a valid identifier continuation
  Allow: characters, numbers(0..9) and '_'
  Returns: SUCCESS or FAIL
*/
unint is_identifier_continue(int32_t cp) {
    unint cat = UNICODE_CAT(cp);

    if (
        cp == '_' ||

        (cp >= '0' && cp <= '9') ||

        cat == UTF8PROC_CATEGORY_LU ||
        cat == UTF8PROC_CATEGORY_LL ||
        cat == UTF8PROC_CATEGORY_LT ||
        cat == UTF8PROC_CATEGORY_LM ||
        cat == UTF8PROC_CATEGORY_LO ||

        cat == UTF8PROC_CATEGORY_MN ||
        cat == UTF8PROC_CATEGORY_MC
    ) {
        return SUCCESS;
    }
    else if (
        cat == UTF8PROC_CATEGORY_ME ||
        cat == UTF8PROC_CATEGORY_ND ||
        cat == UTF8PROC_CATEGORY_NL ||
        cat == UTF8PROC_CATEGORY_NO ||
        cat == UTF8PROC_CATEGORY_PC ||
        cat == UTF8PROC_CATEGORY_PD ||
        cat == UTF8PROC_CATEGORY_PS ||
        cat == UTF8PROC_CATEGORY_PE ||
        cat == UTF8PROC_CATEGORY_PI ||
        cat == UTF8PROC_CATEGORY_PF ||
        cat == UTF8PROC_CATEGORY_PO ||
        cat == UTF8PROC_CATEGORY_SM ||
        cat == UTF8PROC_CATEGORY_SC ||
        cat == UTF8PROC_CATEGORY_SK ||
        cat == UTF8PROC_CATEGORY_SO ||
        cat == UTF8PROC_CATEGORY_ZS ||
        cat == UTF8PROC_CATEGORY_ZL ||
        cat == UTF8PROC_CATEGORY_ZP ||
        cat == UTF8PROC_CATEGORY_CC ||
        cat == UTF8PROC_CATEGORY_CF ||
        cat == UTF8PROC_CATEGORY_CS ||
        cat == UTF8PROC_CATEGORY_CO ||
        cat == UTF8PROC_CATEGORY_CN
    ) {
        return FAIL;
    }
    else {
        return FAIL; // just in case
    }
}
/*
  Reads a decimal number. Stops at the last valid character of the number.
  After an '_' there must be a valid number or it will emit a syntax error
  Returns:
    0: Success
    1: Fail
    and tok is at next codepoint after the number
*/
unint tok_decimal_tail(struct tok_state* tok) {
    int32_t* cp = &tok->uc.cp;

    while(1) {
        do {
            next_cp(tok);
        }while(ISDIGIT(*cp));

        if(*cp != '_') break;

        next_cp(tok);
        if(!ISDIGIT(*cp)) {
            backup_cp(tok);
            _Tokenizer_syntaxerror(tok, "invalid decimal literal");
            return FAIL;
        }
    }
    return SUCCESS;
}

unint verify_end_of_number(struct tok_state* tok, char* kind) {
    int32_t* cp = &tok->uc.cp;
    
    if(is_potential_identifier_char(*cp) || (!ISASCII(*cp) && is_identifier_continue(*cp) == SUCCESS)) {
        backup_cp(tok);
        _Tokenizer_syntaxerror(tok, "invalid %s literal", kind);
        return FAIL;
    }
    return SUCCESS;
}

/* Some token helper functions */

unint _Token_OneChar(int32_t c1){
    switch (c1) {
    case '!': return EXCLAMATION;
    case '%': return PERCENT;
    case '&': return AMPER;
    case '(': return LPAR;
    case ')': return RPAR;
    case '*': return STAR;
    case '+': return PLUS;
    case ',': return COMMA;
    case '-': return MINUS;
    case '.': return DOT;
    case '/': return SLASH;
    case ':': return COLON;
    // case ';': return SEMI;
    case '<': return LESS;
    case '=': return EQUAL;
    case '>': return GREATER;
    // case '@': return AT;
    case '[': return LSQB;
    case ']': return RSQB;
    case '^': return CIRCUMFLEX;
    // case '{': return LBRACE;
    case '|': return VBAR;
    // case '}': return RBRACE;
    case '~': return TILDE;
    }
    return OP;
}

unint _Token_TwoChars(int32_t c1, int32_t c2) {
    switch (c1) {
    case '!':
        switch (c2) {
        case '=': return NOTEQUAL;
        }
        break;
    case '%':
        switch (c2) {
        case '=': return PERCENTEQUAL;
        }
        break;
    case '&':
        switch (c2) {
        case '=': return AMPEREQUAL;
        }
        break;
    case '*':
        switch (c2) {
        case '*': return DOUBLESTAR;
        case '=': return STAREQUAL;
        }
        break;
    case '+':
        switch (c2) {
        case '=': return PLUSEQUAL;
        }
        break;
    case '-':
        switch (c2) {
        case '=': return MINEQUAL;
        case '>': return RARROW;
        }
        break;
    case '/':
        switch (c2) {
        case '/': return DOUBLESLASH;
        case '=': return SLASHEQUAL;
        }
        break;
    case ':':
        switch (c2) {
        case '=': return COLONEQUAL;
        }
        break;
    case '<':
        switch (c2) {
        case '<': return LEFTSHIFT;
        case '=': return LESSEQUAL;
        case '>': return NOTEQUAL;
        }
        break;
    case '=':
        switch (c2) {
        case '=': return EQEQUAL;
        }
        break;
    case '>':
        switch (c2) {
        case '=': return GREATEREQUAL;
        case '>': return RIGHTSHIFT;
        }
        break;
    // case '@':
    //     switch (c2) {
    //     case '=': return ATEQUAL;
    //     }
    //     break;
    case '^':
        switch (c2) {
        case '=': return CIRCUMFLEXEQUAL;
        }
        break;
    case '|':
        switch (c2) {
        case '=': return VBAREQUAL;
        }
        break;
    }
    return OP;
}

unint _Token_ThreeChars(int32_t c1, int32_t c2, int32_t c3) {
    switch (c1) {
    case '*':
        switch (c2) {
        case '*':
            switch (c3) {
            case '=': return DOUBLESTAREQUAL;
            }
            break;
        }
        break;
    // case '.':
    //     switch (c2) {
    //     case '.':
    //         switch (c3) {
    //         case '.': return ELLIPSIS;
    //         }
    //         break;
    //     }
    //     break;
    case '/':
        switch (c2) {
        case '/':
            switch (c3) {
            case '=': return DOUBLESLASHEQUAL;
            }
            break;
        }
        break;
    case '<':
        switch (c2) {
        case '<':
            switch (c3) {
            case '=': return LEFTSHIFTEQUAL;
            }
            break;
        }
        break;
    case '>':
        switch (c2) {
        case '>':
            switch (c3) {
            case '=': return RIGHTSHIFTEQUAL;
            }
            break;
        }
        break;
    }
    return OP;
}

/* ----------- */

unint tokenize(struct tok_state* tok, struct token* token) {
    int32_t* cp = &tok->uc.cp;
    unint* nread = &tok->uc.nread;
    (void)nread; // Unused for now

    unint blankline = 0;

    const char *p_start = NULL;
    const char *p_end = NULL;
nextline:
    tok->start = NULL;
    tok->starting_col_offset = -1;
    blankline = 0;

    // If: At Begining Of Line
    if(tok->atbol) {
        tok->atbol = 0;
        DBG(DO_LEXER_INDENTATION_DBG, "Reading Indentation\n");
        // Updates tok->pendin and errors
        if(get_indentation(tok, &blankline) == FAIL) return MAKE_TOKEN(ERRORTOKEN);

    }

    tok->start = tok->uc.curr;
    tok->starting_col_offset = tok->col_offset;

    // Need to generate indent/dedent tokens?
    if(tok->pendin != 0) DBG(DO_LEXER_INDENTATION_DBG, "Reading Indentation complete\n\n\n");
    while (tok->pendin != 0) return MAKE_TOKEN(generate_indent_dedent_token(tok, token)); // Updates tok->pendin
    
_loop:
    if (0) goto _loop; // Remove Compiler warning about unused label
    tok->start = NULL;

    /* Peek ahead at the next character */
    DBG(DO_LEXER_PEEK_DEBUG, "Peeking start\n");
    next_cp(tok);
    backup_cp(tok);
    DBG(DO_LEXER_PEEK_DEBUG, "Peeking complete\n\n\n");

    // Skip spaces
    DBG(DO_LEXER_SPACE_DEBUG, "Space skiping start\n");
    do {
        next_cp(tok);
    } while (*cp == ' ' || *cp == '\t' || *cp == '\014');
    DBG(DO_LEXER_SPACE_DEBUG, "Space skiping complete\n\n\n");

    DBG(DO_LEXER_USED_CHARACTER_DBG, "Using '%s' for token detection\n\n\n", CP_TO_ENCODING(*cp));

    // Python checks if tok->cur == NULL to see if there was an error, we check if tok->done is not E_OK
    // We don't need it for now
    tok->start = tok->uc.curr == NULL ? NULL : tok->uc.curr - tok->uc.nread;
    tok->starting_col_offset = tok->col_offset - 1;

    // Comments (#)
    if(*cp == '#') {
        while(*cp != EOF && !(*cp == '\n' || *cp == '\r')) 
            next_cp(tok);

        backup_cp(tok); /* don't eat the newline or EOF */
        p_start = tok->start;
        p_end = tok->uc.curr;
        return MAKE_TOKEN(COMMENT);
    }

    // Comments (// & /)
    if(*cp == '/') {
        next_cp(tok);

        if(*cp == '/') { // Comment
            while(!(*cp == '\n' || *cp == '\r') && *cp != EOF) 
                next_cp(tok);

            backup_cp(tok); /* don't eat the newline or EOF */
            p_start = tok->start;
            p_end = tok->uc.curr;
            return MAKE_TOKEN(COMMENT);

        // Multi-line comment
        } else if(*cp == '*') {
            int32_t prev = 0;

            tok->first_lineno = tok->lineno;
            tok->multi_line_start = tok->line_start;

            while(true) {
                next_cp(tok);
                // Errors
                if(tok->done == E_DECODE)
                    return MAKE_TOKEN(ERRORTOKEN); // break; in python (string)

                // Unexpected ending
                if(*cp == EOF) {
                    tok->uc.curr = (char *)tok->start;
                    tok->uc.curr++;
                    tok->line_start = tok->multi_line_start;
                    unint start = tok->lineno;
                    tok->lineno = tok->first_lineno;

                    _Tokenizer_syntaxerror(tok, "Unterminated multi-line comment (detected at line %d)", start);
                    return MAKE_TOKEN(ERRORTOKEN);
                }

                // New line
                if(*cp == '\n')
                    tok->atbol = 1;

                if (prev == '*' && *cp == '/') {
                    next_cp(tok);

                    // No need to check for errors as '\n' is on the same line as "*/" so next_cp already check for malformed characters on that line
                    
                    // Exit?
                    if(*cp == '\n')
                        break;

                    _Tokenizer_syntaxerror(tok, "Multi-line comments must end on a new line ('\\n')");
                    return MAKE_TOKEN(ERRORTOKEN);
                }
                prev = *cp;
            }
            backup_cp(tok); /* don't eat the newline or EOF */

            p_start = tok->start;
            p_end = tok->uc.curr;
            return MAKE_TOKEN(COMMENT);
        }
        return MAKE_TOKEN(SLASH);
    }

    // EOF or unicode errors
    if(*cp == EOF) {
        DBG(DO_LEXER_EOF_DBG, "Could be EOF\n");
        if(tok->level) return MAKE_TOKEN(ERRORTOKEN);
        if (tok->done == E_EOF) {
            DBG(DO_LEXER_EOF_DBG, "It is EOF\n");
            return MAKE_TOKEN(ENDMARKER);
        }

        return MAKE_TOKEN(ERRORTOKEN);
    }

    // Identifier
    // All ASCII validation is done with `is_potential_identifier_start` and `is_potential_identifier_char`
    // Unicode validation is done with XID_start and XID_continue

    if(*cp == '@') next_cp(tok);
    if(is_potential_identifier_start(*cp)) {

        // ASCII validation is done on the while condition but Unicode validation isnt so do it here
        if(!ISASCII(*cp) && is_identifier_start(*cp) == FAIL) {
            (is_printable_cp(*cp) == SUCCESS) ?
                _Tokenizer_syntaxerror(tok, "invalid character '%s' (U+%04X)", CP_TO_ENCODING(*cp), *cp) :
                _Tokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", *cp);
                
            return MAKE_TOKEN(ERRORTOKEN);
        }

        // Read the identifier
        while(is_potential_identifier_char(*cp)) {
            // Unicode validation is not done on the while condition so it is done here
            if(!ISASCII(*cp) && is_identifier_continue(*cp) == FAIL) {
                (is_printable_cp(*cp) == SUCCESS) ?
                    _Tokenizer_syntaxerror(tok, "invalid character '%s' (U+%04X)", CP_TO_ENCODING(*cp), *cp) :
                    _Tokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", *cp);

                return MAKE_TOKEN(ERRORTOKEN);
            }   
        
            next_cp(tok);
        }
        backup_cp(tok);

        // Verify identifier name
        p_start = tok->start;
        p_end = tok->uc.curr;

        return MAKE_TOKEN(NAME);
    }

    // Carriage Return
    if (*cp == '\r') next_cp(tok);

    // Fallthrough or just a new line
    if (*cp == '\n') {
        if (blankline || tok->level > 0) goto nextline;

        p_start = tok->start;
        p_end = tok->uc.curr - tok->uc.nread;
        return MAKE_TOKEN(NEWLINE);
    }

    // Period or number starting with period?
    if(*cp == '.') {
        next_cp(tok);

        if(ISDIGIT(*cp)) goto fraction;
        else backup_cp(tok);

        p_start = tok->start;
        p_end = tok->uc.curr;
        return MAKE_TOKEN(DOT);
    }

    // Digits / numbers
    if(ISDIGIT(*cp)) {
        if(*cp == '0') {
            // Hex, octal, bin
            next_cp(tok);

            // Hexadecimal number
            if (*cp == 'x' || *cp == 'X') {
                next_cp(tok);

                do {
                    // Ignore underscores
                    if(*cp == '_') next_cp(tok);

                    // If it'ts not a hex digit
                    if(!ISXDIGIT(*cp)) {
                        backup_cp(tok);
                        return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "invalid hexadecimal literal"));
                    }
                    do {
                        next_cp(tok);
                    }while(ISXDIGIT(*cp));

                }while(*cp == '_');

                // Verify the end of the number
                if(verify_end_of_number(tok, "hexadecimal") == FAIL) return MAKE_TOKEN(ERRORTOKEN);
            }
            
            // Octal number
            else if (*cp == 'o' || *cp == 'O') {
                next_cp(tok);

                do {
                    // Ignore underscores
                    if(*cp == '_') next_cp(tok);

                    // If it'ts not an octal digit
                    if(!ISODIGIT(*cp)) {
                        if(ISDIGIT(*cp)) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "invalid digit '%s' in octal literal", CP_TO_ENCODING(*cp)));
                        else {
                            backup_cp(tok);
                            return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "invalid octal literal"));
                        }
                    }

                    do {
                        next_cp(tok);
                    }while(ISODIGIT(*cp));

                }while(*cp == '_');

                // Verify if it was a digit but not in octal range
                if(ISDIGIT(*cp)) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "Invalid digit '%s' in octal literal", CP_TO_ENCODING(*cp)));

                // Verify the end of the number
                if(verify_end_of_number(tok, "octal") == FAIL) return MAKE_TOKEN(ERRORTOKEN);
            }
        
            // Binary number
            else if(*cp == 'b' || *cp == 'B') {
                next_cp(tok);

                do {
                    // Ignore underscores
                    if(*cp == '_') next_cp(tok);

                    // If it'ts not a binary digit
                    if(!ISBDIGIT(*cp)) {
                        if(ISDIGIT(*cp)) _Tokenizer_syntaxerror(tok, "invalid digit '%s' in binary literal", CP_TO_ENCODING(*cp));
                        else {
                            backup_cp(tok);
                            _Tokenizer_syntaxerror(tok, "invalid binary literal");
                        }
                    }

                    do {
                        next_cp(tok);
                    }while(ISODIGIT(*cp));

                }while(*cp == '_');

                // Verify if it was a digit but not in binary range
                if(ISDIGIT(*cp)) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "Invalid digit '%s' in binary literal", CP_TO_ENCODING(*cp)));

                // Verify the end of the number
                if(verify_end_of_number(tok, "binary") == FAIL) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "invalid binary literal"));
            }

            // No suport for old-style so just read the number as a decimal
            else { backup_cp(tok); goto decimal; }
 
        }
        // Decimal or Float / Double
        else {
decimal:
            // Integer part
            if(tok_decimal_tail(tok) == FAIL) return MAKE_TOKEN(ERRORTOKEN);

            if(*cp == '.') {
                next_cp(tok);
fraction:
                // Decimal part
                if(ISDIGIT(*cp)) {
                    if(tok_decimal_tail(tok) == FAIL) return MAKE_TOKEN(ERRORTOKEN);
                }
            }

            // Exponent
            if(*cp == 'e' || *cp == 'E') {
                next_cp(tok);
                
                // Sign
                if(*cp == '+' || *cp == '-') {
                    next_cp(tok);
                    if(!ISDIGIT(*cp)) {
                        backup_cp(tok);
                        return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "invalid decimal literal"));
                    }

                // Digits
                } else if(!ISDIGIT(*cp)) {
                    // Verify end of number
                    if(verify_end_of_number(tok, "decimal") == FAIL) return MAKE_TOKEN(ERRORTOKEN);
                    backup_cp(tok);

                    p_start = tok->start;
                    p_end = tok->uc.curr;
                    return MAKE_TOKEN(NUMBER);
                }

                // Verify
                if(tok_decimal_tail(tok) == FAIL) return MAKE_TOKEN(ERRORTOKEN);
            }
            
            // Verify end of number
            else if(verify_end_of_number(tok, "decimal") == FAIL) return MAKE_TOKEN(ERRORTOKEN);

        }
        backup_cp(tok);

        p_start = tok->start;
        p_end = tok->uc.curr;
        return MAKE_TOKEN(NUMBER);
    }
    
    // Strings
    if(*cp == '\'' || *cp == '"') {
        int32_t quote = *cp;
        unint len = 0;

        while(true) {
            next_cp(tok);

            // No need to do the checks that python does because those checks are for triple quote strings that allow new lines that are not checked immediately for UTF8 errors when reading the '"'
            if(*cp == EOF || *cp == '\n') {

                tok->uc.curr = (char *)tok->start;
                tok->uc.curr++;

                if(quote == '\'' && len == 1) _Tokenizer_syntaxerror(tok, "unterminated character literal (detected at line %d)", tok->lineno);
                else if(quote == '\'') _Tokenizer_syntaxerror(tok, "unterminated multi-character literal (detected at line %d)", tok->lineno);
                else _Tokenizer_syntaxerror(tok, "unterminated string literal (detected at line %d)", tok->lineno);

                if (*cp != '\n') tok->done = E_EOLS;
                return MAKE_TOKEN(ERRORTOKEN);
            }

            // Exit?
            if(*cp == quote) {
                p_start = tok->start;
                p_end = tok->uc.curr;
                return MAKE_TOKEN(STRING);
            }
            else {
                if(*cp == '\\') {
                    next_cp(tok);
                    if (*cp == '\r') next_cp(tok);
                }
            }
            ++len;
        }
    }

    // Check for two-character token
    // Save
    {
        int32_t cp1 = *cp; 
        next_cp(tok); int32_t cp2 = *cp; 

        unint current_token = _Token_TwoChars(cp1, cp2);
        if (current_token != OP) {
            next_cp(tok); int32_t cp3 = *cp; 
            unint current_token3 = _Token_ThreeChars(cp1, cp2, cp3);

            if (current_token3 != OP) current_token = current_token3;
            else backup_cp(tok);

            p_start = tok->start;
            p_end = tok->uc.curr;

            return MAKE_TOKEN(current_token);
        }
        backup_cp(tok);
    }
    // Python has c saved but we can't just save the current cp, we also need to save nread
    // So, go back one more and read to get the info
    backup_cp(tok);
    next_cp(tok);

    // Parentheses
    switch(*cp) {
        case '(':
        case '[':
            if(tok->level >= MAX_PARENTHESES_LEVEL) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "too many nested parentheses"));
            tok->parenstack[tok->level] = *cp;
            tok->parenlinenostack[tok->level] = tok->lineno;
            tok->parencolstack[tok->level] = tok->col_offset; // in python: (int)(tok->start - tok->line_start);
            tok->level++;
            break;
        case ')':
        case ']':
            if(!tok->level) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "unmatched '%s'", CP_TO_ENCODING(*cp)));

            if(tok->level > 0) {
                tok->level--;
                int32_t opening = tok->parenstack[tok->level];

                if(!(
                    (opening == '(' && *cp == ')') ||
                    (opening == '[' && *cp == ']')
                )) {
                    unsigned char buf1[CP_ENCODING_BUF];
                    unsigned char buf2[CP_ENCODING_BUF];
                    if (tok->parenlinenostack[tok->level] != tok->lineno) 
                        return MAKE_TOKEN(_Tokenizer_syntaxerror(tok,
                                "closing parenthesis '%s' does not match "
                                "opening parenthesis '%s' on line %d",
                                CP_TO_ENCODING_BUF(*cp, buf1), CP_TO_ENCODING_BUF(opening, buf2), tok->parenlinenostack[tok->level]));
                    else
                        return MAKE_TOKEN(_Tokenizer_syntaxerror(tok,
                                "closing parenthesis '%s' does not match "
                                "opening parenthesis '%s'",
                                CP_TO_ENCODING_BUF(*cp, buf1), CP_TO_ENCODING_BUF(opening, buf2)));
                }

            }
            break;
        default:
            break;
    }

    // Non printable codepoints
    if(is_printable_cp(*cp) == FAIL) return MAKE_TOKEN(_Tokenizer_syntaxerror(tok, "invalid non-printable character U+%04X", *cp));
    
    p_start = tok->start;
    p_end = tok->uc.curr;
    return MAKE_TOKEN(_Token_OneChar(*cp));

}