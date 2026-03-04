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
    char *start, *end;
};

struct tok_state {
    struct unicode uc;
    unint nread;

    unint err;

    unint atbol;
    unint lineno;

    unint indent; 
    nint pendin;
    unint indstack[MAX_INDENT];
    unint altindstack[MAX_INDENT];
};

void tok_state_init(struct tok_state* tok);
nint tokenize(struct tok_state* tok, struct token* token);

#endif