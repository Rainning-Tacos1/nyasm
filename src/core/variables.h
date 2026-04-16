#ifndef VARIABLES_H
#define VARIABLES_H

#include "types.h"

struct AstIdentifier;
struct Variable;
struct ArrayElement;


enum ValueTypes {
    VALUE_INT,
    VALUE_STR,
    VALUE_CHARACTER,
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
        int32_t character;
        double flt;
        struct Array arr;
    } val;
};

struct ArrayElement {
    struct Value this;
    struct ArrayElement* next;
};

struct Value* new_string(int32_t* cps, unint len);
struct Value* new_number(int32_t* cps, unint len);
struct Value* new_character(int32_t cp);

struct Value* new_array();
unint append_array(struct Value* arr, struct Value* val);

struct Variable* new_variable(struct Parser* p, int32_t* cps, unint len, struct Value* val);
struct Variable* get_variable(struct Parser* p, int32_t* cps, unint len);
unint is_variable_declared(struct Parser* p, int32_t* cps, unint len);

void print_value(struct Value* val);
#endif