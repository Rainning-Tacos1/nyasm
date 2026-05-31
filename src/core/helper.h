#ifndef HELPPER_H
#define HELPPER_H

#include "lexer.h"
#include "types.h"

struct TokenStream {
    struct token* start;
    struct token* end;

    struct token* read;
    struct token* peek;
};

enum VariableParsing {
    VP_SUCCESS,
    VP_UNRESOLVED_LABEL,
    VP_NONE,
    VP_FAIL
};

unint compare_identifiers_cp_array(struct token* ident1, int32_t* cmp);

void tks_init(struct TokenStream* tks, struct token* start, struct token* end);
struct token* tks_read(struct TokenStream* tks);
struct token* tks_peek(struct TokenStream* tks);
void tks_reset_peek(struct TokenStream* tks);

unint parse_potential_variable(struct Parser* p, struct TokenStream* tks, nint* addr);
unint parse_potential_register(struct TokenStream* tks, int32_t** registers, unint* idx);

nint unresolved_label(struct Parser* p, struct token* _token);

#endif