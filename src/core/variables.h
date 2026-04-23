#ifndef VARIABLES_H
#define VARIABLES_H

#include "types.h"

struct AstIdentifier;
struct Variable;
struct ArrayElement;


enum ValueTypes {
    VALUE_INT,
    VALUE_STR,
    VALUE_DOUBLE,
    VALUE_ARRAY
};

struct String {
    int32_t* str;
    unint len;
};

struct Array {
    struct ArrayElement* head;
    struct ArrayElement* tail;
    unint len;
};

struct Value {
    unint type;

    union {
        nint number;
        struct String string;
        double flt;
        struct Array arr;
    } val;
};

struct ArrayElement {
    struct Value this;
    struct ArrayElement* next;
};

struct Value* new_string(struct Parser* p, struct token* _token);
struct Value* new_number(struct Parser* p, struct token* _token, unint is_neg);

struct Value* new_array(struct Parser* p, struct token* _token);
unint append_array(struct Parser* p, struct token* _token, struct Value* arr, struct Value* val);

struct Variable* new_variable(struct Parser* p, int32_t* cps, unint len, struct Value* val);
struct Variable* get_variable(struct Parser* p, int32_t* cps, unint len);
unint is_variable_declared(struct Parser* p, int32_t* cps, unint len);

void print_value(struct Value* val);

#define	ERANGE 34	/* Result too large */

extern unint __errno;
unint _strtoul(int32_t** ptr, int32_t* str, unint len, unint base);
nint _strtol(int32_t** ptr, int32_t* str, unint len, unint base, unint is_neg);

unint _strtod(struct Parser* parser, struct token* _token, unint is_neg, double* val);

#endif