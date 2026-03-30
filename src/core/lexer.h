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

struct token {
    unint lineno, col_offset, end_lineno, end_col_offset;
    int32_t* cps;
};

struct tok_state {
    // Unicode related
    struct unicode uc;

    // File related
    unint atbol;
    unint lineno;
    unint col_offset;
    char* source;

    // Column related
    unint starting_col_offset;

    // Token related
    char* start;
    char* end;

    // Error related
    unint done;

    // Token cp buffer related
    unint token_cp_buffer_idx;
    unint token_cp_buffer_size;

    // Indentation related
    unint indent; 
    nint pendin;
    unint indstack[MAX_INDENT];
    unint altindstack[MAX_INDENT];

    // May change place
    unsigned char encoded[MAX_ENCODING_SIZE];
};

void tok_state_init(struct tok_state* tok);
unint tokenize(struct tok_state* tok, struct token* token);

#endif