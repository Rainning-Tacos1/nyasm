#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "types.h"

unint compare_identifiers(struct token* ident1, int32_t* cmp) {
    if (!ident1 || !cmp) return FAIL;

    int32_t* a = ident1->cps;
    unint alen = ident1->len;

    unint i = 0;

    while (1) {
        int32_t b = cmp[i];
        if (b == -1) break;

        if (i >= alen) return FAIL;

        if (a[i] != b) return FAIL;

        i++;
    }

    if (i != alen) return FAIL;

    return SUCCESS;
}

/*
Priority:
 - registers
 - func args
 - func vars
 - func structs
 - global structs 
 - func labels
 - labels
 - vars

*/

unint parse_potential_variable(struct Parser* p, struct token* start,  struct token* end, int32_t** registers) {
    return 0;
}