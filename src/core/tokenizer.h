#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "types.h"
#include "config.h"
#include "../unicode/api.h"

struct token {
    unint lineno, col_offset, end_lineno, end_col_offset;
    char *start, *end;
};

struct tok_state {
    struct unicode uc;
    unint nread;

    unint err;

    unint atbol;

    unint indent; 
    nint pendin;
    unint indstack[MAX_INDENT];
    unint altindstack[MAX_INDENT];
};

void tok_state_init(struct tok_state* tok);
nint tokenize(struct tok_state* tok, struct token* token);

#endif