#include "types.h"
#include "lexer.h"

#include "api/unicode.h"
#include "api/debug.h"

#include "err.h"
#include "token.h"

void tok_state_init(struct tok_state* tok) {
    for(unint i=0; i<MAX_INDENT; ++i) tok->altindstack[i] = tok->indstack[i] = 0;
    tok->col_offset = tok->lineno = tok->pendin = tok->indent = 0;
    tok->err = E_OK;
    tok->atbol = 1;
    tok->source = NULL;
    UNICODE_INIT(&tok->uc);
}

unint lexer_error(struct tok_state* tok, void* msg) {
    LOG("%s:%d %s\n", tok->source, tok->lineno, msg);
    LOG("Column: %d\n", tok->col_offset);
    return ERRORTOKEN;
}

unint make_token(struct tok_state* tok, unint token_type, struct token* token) {
    // For now
    token->lineno = tok->lineno;
    token->col_offset = tok->col_offset;
    return token_type;
}

/*
  Tries to read the next grapheme
  Updates tok fields:
    nread, err, cps

  In case of error:
    you can perform "normal" character checks as they will never trigger
*/
void next_grapheme(struct tok_state* tok) {
    int32_t* cps = tok->uc.cps;
    int32_t curr = *cps;
    if(READ_GRAPHEME(&tok->uc) == SUCCESS) {
        DBG(DO_LEXER_CHAR_DBG, "READ '%c', %d\n", (char)*cps, *cps);
        ++tok->col_offset;
        if(curr != '\n' && *cps == EOF) { // add a 'fake' new line at the end
            DBG(DO_LEXER_CHAR_DBG, "ADDING 'FAKE' NEW LINE\n");
            *cps = '\n';
            tok->uc.nread = 1;
            tok->uc.err = UNICODE_OK;
        }
    }
    return;
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

    int32_t* cps = tok->uc.cps;
    unint* nread = &tok->uc.nread;

    for(;;) {
        next_grapheme(tok);
        if(*nread != 1) break;

        if(*cps == ' ') { ++col; ++altcol; }
        else if(*cps == '\t') {
            col = (col / TAB_SIZE + 1) * TAB_SIZE;
            altcol = (altcol / ALT_TAB_SIZE + 1) * ALT_TAB_SIZE;
        }
        // No formfeed
        // Handle multiline
        else break;
    }
    // Blank lines
    if(*nread == 1 && *cps == '\n') blankline = 1;
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

unint tokenize(struct tok_state* tok, struct token* token) {
    int32_t* cps = tok->uc.cps;
    unint* nread = &tok->uc.nread;

    // If: At Begining Of Line
    if(tok->atbol) {
        tok->atbol = 0;
        // Updates tok->pendin and errorrs
        if(get_indentation(tok) == FAIL) return lexer_error(tok, "Indentation error");

    }

    // Need to generate indent/dedent tokens?
    while (tok->pendin != 0) return generate_indent_dedent_token(tok, token); // Updates tok->pendin
    for(;;) {
        switch(*cps) {
            // Use python lexing for new lines
            case '\r':
                next_grapheme(tok);
            case '\n':
                if (*cps != '\n') break; // Check because of \r fallthrough. The same is done in python but with ifs

                ++tok->lineno;
                tok->atbol = 1;

                return NEWLINE;

            case ' ': case '\f': case '\t': case '\v': /* spaces */
                next_grapheme(tok);
                break;

            case '"':
                DBG(DO_LEXER_TOKEN_DBG, "\"");
                do { next_grapheme(tok); DBG(DO_LEXER_TOKEN_DBG, "%c", (char)*cps); } while(*cps != '"');
                next_grapheme(tok);
                DBG(DO_LEXER_TOKEN_DBG, " ");
                return 70; // String
            // Numbers - dec, hex, bin, oct
            // Negative numbers?
            case '0': case '1': case '2': case '3': case '4': // From Lua
            case '5': case '6': case '7': case '8': case '9':
                // From python
                if(*cps == '0') {
                    /* Hex, octal or binary -- maybe. */
                    next_grapheme(tok);
                    if(*cps == 'x' || *cps == 'X') {
                        /* Hex */
                        next_grapheme(tok);
                        do {
                            if(*cps == '_') next_grapheme(tok);
    
                            if (!((*cps >= '0' && *cps <= '9') ||
                                (*cps >= 'a' && *cps <= 'f') ||
                                (*cps >= 'A' && *cps <= 'F'))) {
                                tok->col_offset--;
                                return lexer_error(tok, "Invalid hexadecimal literal");
                            }
                            do {
                                next_grapheme(tok);
                            } while((*cps >= '0' && *cps <= '9') ||
                                (*cps >= 'a' && *cps <= 'f') ||
                                (*cps >= 'A' && *cps <= 'F'));
                        } while (*cps == '_');
                        if(is_identifier_continue(*cps) == SUCCESS) return lexer_error(tok, "Invalid hexadecimal literal!");
                    }
                }
                return NUMBER;
            case '.':
                next_grapheme(tok);
                return DOT;
            case '=':
                next_grapheme(tok);
                return *cps == '=' ? EQEQUAL : EQUAL;
            case '#':
                while(!(*cps == '\n' || *cps == '\r') && *cps != EOF) next_grapheme(tok);
                return COMMENT;
            case '/':
                next_grapheme(tok);
                if(*cps == '/') { // Comment
                    while(!(*cps == '\n' || *cps == '\r') && *cps != EOF) next_grapheme(tok);
                    return COMMENT;
                } else if(*cps == '*') { // Multi line comment
                    while (*cps != EOF) {
                        if (*cps == '*') {
                            next_grapheme(tok);
                            if (*cps == '/') {
                                next_grapheme(tok);
                                break;
                            }
                        } else next_grapheme(tok);
                    }
                    return COMMENT;
                }
                return SLASH;
            case EOF:
                if (tok->uc.err == UNICODE_OK) return ENDMARKER;
                return lexer_error(tok, "Unicode error");
            case '@':
                next_grapheme(tok);
                DBG(DO_LEXER_TOKEN_DBG, "@");
            default:
                if(is_identifier_start(*cps) == FAIL) { DBG(1, "%c is not a valid identidfier", (char)*cps); return lexer_error(tok, "Invalid Syntax"); }

                do {
                    if(UNICODE_TO_ENCODING(cps, *nread, tok->encoded, MAX_ENCODING_SIZE) != SUCCESS) return lexer_error(tok, "Error encoding code point to encoding");
                    DBG(DO_LEXER_TOKEN_DBG, "%s", tok->encoded);
                    next_grapheme(tok);
                } while(is_identifier_continue(*cps) == SUCCESS);
                DBG(DO_LEXER_TOKEN_DBG, " ");
                return NAME;
        }
    }
}