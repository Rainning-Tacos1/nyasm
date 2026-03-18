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

/*
  Initializes the buffer used to store the cp's of tokens
  Returns:
    NULL : Memory error
    X: memory of the allocated buffer
*/
int32_t* token_cp_buffer_init(struct tok_state* tok) {
    tok->token_cp_buffer_idx = 0;
    tok->token_cp_buffer_size = TOKEN_CP_BUFFER_INIT_SIZE;
    return (int32_t*)MEM_ALLOC(TOKEN_CP_BUFFER_INIT_SIZE, "token");
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
        tok->token_cp_buffer_size = tok->token_cp_buffer_size + tok->token_cp_buffer_size >> 1; // 1.5x growth

        mem = (int32_t*)MEM_RESIZE_LAST(tok->token_cp_buffer_size);
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
    return (int32_t*)MEM_RESIZE_LAST(tok->token_cp_buffer_idx);
}

void tok_state_init(struct tok_state* tok) {
    for(unint i=0; i<MAX_INDENT; ++i) tok->altindstack[i] = tok->indstack[i] = 0;
    tok->col_offset = tok->pendin = tok->indent = 0;
    tok->atbol = tok->lineno = 1;
    tok->source = tok->start = tok->end = NULL;
    UNICODE_INIT(&tok->uc);
}

unint lexer_error(struct tok_state* tok, void* msg, ...) {
    va_list args;
    va_start(args, msg);

    LOG("%s:%d ", tok->source, tok->lineno);
    VLOG(msg, args);
    LOG("\nColumn: %d\n", tok->col_offset);
    return ERRORTOKEN;
}

unint make_token(struct tok_state* tok, unint token_type, struct token* token) {
    // For now
    token->lineno = tok->lineno;
    token->col_offset = tok->col_offset;
    return token_type;
}


/*
  Tries to read the next code point
*/
void next_cp(struct tok_state* tok) {
    int32_t* cp = &tok->uc.cp;
    int32_t curr = *cp;
    if(READ_CODEPOINT(&tok->uc) != SUCCESS) return;

    DBG(DO_LEXER_CHAR_DBG, "READ '%c', %d\n", (char)*cp, *cp);
    ++tok->col_offset;

    // add a 'fake' new line at the end
    if(curr != '\n' && *cp == EOF) {
        *cp = '\n';
        tok->uc.nread = 1;
        tok->uc.err = UNICODE_OK;
    }
}

/*
  Parse indentation and update tokenizer state
  Returns:
    0 : Success
    1 : Indentation error
*/
nbool get_indentation(struct tok_state* tok) {
    unint col = 0;
    unint altcol = 0;
    unint blankline = 0;

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
        else break;
    }
    // Blank lines
    if(*cp == '\n') blankline = 1;
    // col = cont_line_col ? cont_line_col : col;
    // altcol = cont_line_col ? cont_line_col : altcol;
    if(!blankline) {
        col = col;
        altcol = altcol;

        if(col == tok->indstack[tok->indent]) {
            if (altcol != tok->altindstack[tok->indent]) DO_FAIL(lexer_error(tok, "E_TABSPACE"));
        } else if(col > tok->indstack[tok->indent]) {
            if (tok->indent+1 >= MAX_INDENT) DO_FAIL(lexer_error(tok, "E_TOODEEP"));
            if (altcol <= tok->altindstack[tok->indent]) DO_FAIL(lexer_error(tok, "E_TABSPACE"));
            tok->pendin++;
            tok->indstack[++tok->indent] = col;
            tok->altindstack[tok->indent] = altcol;
        } else {
            while (tok->indent > 0 &&
                col < tok->indstack[tok->indent]) {
                tok->pendin--;
                tok->indent--;
            }
            if (col != tok->indstack[tok->indent]) DO_FAIL(lexer_error(tok, "E_DEDENT"));
            if (altcol != tok->altindstack[tok->indent]) DO_FAIL(lexer_error(tok, "E_TABSPACE"));
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
        return make_token(tok, DEDENT, token);
    }
    else {
        tok->pendin--;
        return make_token(tok, INDENT, token);
    }
}

void increment_line_number(struct tok_state* tok) {
    ++tok->lineno;
    tok->col_offset = 0;
    tok->atbol = 1;
}

unint verify_end_of_number(struct tok_state* tok, int32_t cp, char* kind) {
    if(cp < 128 && is_potential_identifier_char(cp)) return lexer_error(tok, "Invalid literal");
}

/*
  Tells if a code point is a valid identifier start
  Allow: characters and '_'
*/
unint is_identifier_start(int32_t cp) {
    unint cat = UNICODE_CAT(cp);

    if (
        cp == '_' ||

        cat == UTF8PROC_CATEGORY_LU ||
        cat == UTF8PROC_CATEGORY_LL ||
        cat == UTF8PROC_CATEGORY_LT ||
        cat == UTF8PROC_CATEGORY_LM ||
        cat == UTF8PROC_CATEGORY_LO
    ) {
        return SUCCESS;
    }
    else {
        return FAIL;
    }
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

unint verify_identifier(struct tok_state* tok) {
    
}

unint tokenize(struct tok_state* tok, struct token* token) {
    int32_t* cp = &tok->uc.cp;
    unint* nread = &tok->uc.nread;

    int32_t* token_cp_buffer = NULL;

    // If: At Begining Of Line
    if(tok->atbol) {
        tok->atbol = 0;
        // Updates tok->pendin and errors
        if(get_indentation(tok) == FAIL) return lexer_error(tok, "Indentation error");

    }
    // Need to generate indent/dedent tokens?
    while (tok->pendin != 0) return generate_indent_dedent_token(tok, token); // Updates tok->pendin

_loop:
    tok->start = NULL;

    // Skip spaces
    // Change a little bc we dont have tok_backup()
    while (*cp == ' ' || *cp == '\t' || *cp == '\014') next_cp(tok);

    // Comments (#)
    if(*cp == '#') {
        while(!(*cp == '\n' || *cp == '\r') && *cp != EOF) 
            next_cp(tok); // EOF & Error safe.

        increment_line_number(tok);
        return COMMENT;
    }

    // Comments (// & /)
    if(*cp == '/') {
        next_cp(tok);

        if(*cp == '/') { // Comment
            while(!(*cp == '\n' || *cp == '\r') && *cp != EOF) 
                next_cp(tok); // EOF & Error safe.
            
            increment_line_number(tok);
            return COMMENT;

        // Multi-line comment
        } else if(*cp == '*') {
            int32_t prev = 0;

            while (*cp != EOF) {
                if (*cp == '\n')
                    increment_line_number(tok);
            
                if (prev == '*' && *cp == '/')
                    break;
            
                prev = *cp;
                next_cp(tok);
            }

            return COMMENT;
        }
        return SLASH;
    }

    // EOF or unicode errors
    if(*cp == EOF) {
        if (tok->uc.err == UNICODE_OK) return ENDMARKER;
        return lexer_error(tok, "Unicode error");
    }

    // Identifier
    // All ASCII validation is done with `is_potential_identifier_start` and `is_potential_identifier_char`
    // Unicode validation is done with XID_start and XID_continue
    unint is_at_identifier = 0;
    if(*cp == '@') {
        is_at_identifier=1;
        next_cp(tok);
    }
    if(is_potential_identifier_start(*cp)) {

        // ASCII validation is done on the while condition but Unicode validation isnt so do it here
        if(!ISASCII(*cp) && is_identifier_start(*cp) == FAIL) return lexer_error(tok, "BAD");

        // Init the cp buffer
        token_cp_buffer = token_cp_buffer_init(tok);
        if(token_cp_buffer == NULL) return lexer_error(tok, "Could not allocate initial buffer for identifier");

        // If it's an '@' identifier, push it
        if(is_at_identifier) {
            token_push_cp(tok, (int32_t)'@');
            if(token_cp_buffer == NULL) return lexer_error(tok, "Could not allocate space for identifier");
        }

        // Read the identifier
        while(is_potential_identifier_char(*cp)) {
            // Unicode validation is not done on the while condition so it is done here
            if(!ISASCII(*cp) && is_identifier_continue(*cp) == FAIL) return lexer_error(tok, "BAD2");      

            // Push the codepoint
            // This also serves as the "push" of the identifier start
            token_cp_buffer = token_push_cp(tok, *cp);
            if(token_cp_buffer == NULL) return lexer_error(tok, "Could not allocate space for identifier");
        
            next_cp(tok);
        }

        token_cp_buffer = token_trim_cp_buffer(tok);
        if(token_cp_buffer == NULL) return lexer_error(tok, "Could not trim left space for identifier");

        // Test for now
        for(unint i=0; i<tok->token_cp_buffer_size; ++i) DBG(1, "%c", (char)token_cp_buffer[i]);
        return NAME;
    }

    // Carriage Return
    if (*cp == '\r') next_cp(tok);

    // Fallthrough or just a new line
    if (*cp == '\n') {
        // Increment line number
        increment_line_number(tok);

        return NEWLINE;
    }

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
                    if(!ISXDIGIT(*cp)) return lexer_error(tok, "Invalid hexadecimal literal");

                    do {
                        next_cp(tok);
                    }while(ISXDIGIT(*cp));

                }while(*cp == '_');

                // Verify the end of the number
                if(ISASCII(*cp) && is_potential_identifier_char(*cp)) return lexer_error(tok, "111Invalid hexadecimal literal");
            }
            
            // Octal number
            else if (*cp == 'o' || *cp == 'O') {
                next_cp(tok);

                do {
                    // Ignore underscores
                    if(*cp == '_') next_cp(tok);

                    // If it'ts not an octal digit
                    if(!ISODIGIT(*cp)) {
                        return ISDIGIT(*cp) ? 
                            lexer_error(tok, "Invalid digit '%c' in octal literal", (char)*cp) :
                            lexer_error(tok, "Invalid octal literal");
                    }

                    do {
                        next_cp(tok);
                    }while(ISODIGIT(*cp));

                }while(*cp == '_');

                // Verify if it was a digit but not in octal range
                if(ISDIGIT(*cp)) return lexer_error(tok, "Invalid digit '%c' in octal literal", (char)*cp);

                // Verify the end of the number
                if(ISASCII(*cp) && is_potential_identifier_char(*cp)) return lexer_error(tok, "Invalid octal literal");
            }
        }
        return NUMBER;
    }
    DBG(1, "GOT TO THE END\n");
    return lexer_error(tok, "Invalid character (U+%04X)", *cp);
}