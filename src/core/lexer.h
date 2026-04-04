#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "types.h"
#include "config.h"
#include "../unicode/api.h"

#ifndef EOF
    #define EOF -1
#endif

#define DO_FAIL(stmt) \
    do {              \
        stmt;         \
        return FAIL;  \
    } while(0)

#define DO_SUCCESS(stmt) \
    do {                 \
        stmt;            \
        return SUCCESS;  \
    } while(0)

#define DO_ERRTOKEN(stmt) ((stmt), ERRORTOKEN)

struct token {
    unint level;
    unint lineno, end_lineno;
    nint col_offset, end_col_offset;
    const char *start, *end;
    int32_t* cps;
    unint len;
};

struct tok_state {
    // Unicode related
    struct unicode uc;

    const char* inp;

    // () [] {} Parentheses nesting level
    unint level;

    // File related
    const char* source;

    // Implicte new line
    unint implicit_newline;
    // Needed to restore the codepoint before the implicit new line as the codepoint is overwritten with a \n 
    int32_t implicit_newline_save_cp;
    unint implicit_newline_save_nread;

    // State
    unint atbol;
    unint done;
    
    // Line/Column related
    const char* line_start;
    const char* multi_line_start;
    nint first_lineno;
    nint lineno;
    nint col_offset;
    nint starting_col_offset;

    // Parentheses related
    int32_t parenstack[MAX_PARENTHESES_LEVEL];
    nint parenlinenostack[MAX_PARENTHESES_LEVEL];
    nint parencolstack[MAX_PARENTHESES_LEVEL];

    // Token related
    const char* start;
    const char* end;

    // Token cp buffer related
    unint token_cp_buffer_idx;
    unint token_cp_buffer_size;

    // Indentation related
    unint indent; 
    nint pendin;
    unint indstack[MAX_INDENT];
    unint altindstack[MAX_INDENT];

};

void tok_state_init(struct tok_state* tok);
unint tokenize(struct tok_state* tok, struct token* token);
#endif