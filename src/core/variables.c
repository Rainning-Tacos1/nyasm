#include "api/memory.h"
#include "api/debug.h"

#include "parser.h"
#include "variables.h"
#include "ast.h"


#include "types.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

// Strings
struct Value* new_string(int32_t* cps, unint len) {
    struct Value* string = (struct Value*)MEM_ALLOC(sizeof(struct Value), "string value");
    if(string == NULL) return NULL;

    string->val.string.str = cps;
    string->val.string.len = len;

    string->type = VALUE_STR;
    return string;
}

// Characters
struct Value* new_character(int32_t cp) {
    struct Value* character = (struct Value*)MEM_ALLOC(sizeof(struct Value), "character value");
    if(character == NULL) return NULL;

    character->val.character = cp;

    character->type = VALUE_CHARACTER;
    return character;
}

// Arrays
struct Value* new_array() {
    struct Value* array = (struct Value*)MEM_ALLOC(sizeof(struct Value), "array value");
    if(array == NULL) return NULL;

    array->val.arr.head = NULL;
    array->val.arr.tail = NULL;
    array->val.arr.len = 0;

    array->type = VALUE_ARRAY;
    return array;
}

unint append_array(struct Value* arr, struct Value* val) {
    if(arr->type != VALUE_ARRAY) return FAIL;

    // Array Element
    struct ArrayElement* arr_el = (struct ArrayElement*)MEM_ALLOC(sizeof(struct ArrayElement), "array element value");
    if(arr_el == NULL) return FAIL;

    // 1st time
    if(arr->val.arr.head == NULL) arr->val.arr.head = arr_el;

    // Link
    if(arr->val.arr.tail != NULL) arr->val.arr.tail->next = arr_el;
    arr->val.arr.tail = arr_el;
    
    // Value
    arr->val.arr.tail->this = *val;

    // Update Len
    arr->val.arr.len += 1;

    return SUCCESS;
}

// Integers / Doubles
struct Value* new_number(int32_t* cps, unint len) {
    struct Value* number = (struct Value*)MEM_ALLOC(sizeof(struct Value), "number value");
    if(number == NULL) return NULL;

    // Parse the number
    // May also be float
    // May be binary
    // May be decimal
    number->val.number = 0xc0ffe; // For now
    number->type = VALUE_INT;
    
    return number;
}

struct Variable* new_variable(struct Parser* p, int32_t* cps, unint len, struct Value* val) {
    struct Variable* var = (struct Variable*)MEM_ALLOC(sizeof(struct Variable), "new variable");
    if(var == NULL) return NULL;

    // Fill
    var->var.len = len;
    var->var.cps = cps;
    var->next = NULL;

    // Copy
    var->val = *val;

    // First Variable
    if(p->variables == NULL) p->variables = var;

    // Link the new variable
    if(p->variables_tail != NULL) p->variables_tail->next = var;
    p->variables_tail = var;

    return var;
}

struct Variable* get_variable(struct Parser* p, int32_t* cps, unint len) {
    if(p == NULL || p->variables == NULL) return NULL;

    for(struct Variable* var = p->variables; var != NULL; var = var->next) {
        if(var->var.len != len) continue;

        unint i;
        for(i = 0; i < len; ++i) if(var->var.cps[i] != cps[i]) break;
        
        // Full match
        if(i == len) return var;
    }
    return NULL;
}

unint is_variable_declared(struct Parser* p, int32_t* cps, unint len) {
    return (get_variable(p, cps, len) == NULL) ? FAIL : SUCCESS;
}

void print_value_recur(struct Value* val, unint level) {
    for(unint i=0; i<level; ++i) LOG("\t");
    switch(val->type) {
        case VALUE_INT: 
            LOG("Number: %d", val->val.number);
            break;
        case VALUE_DOUBLE:
            LOG("Float: %f", val->val.flt);
            break;
        case VALUE_STR:
            LOG("Strong: '");
            for(unint i=0; i<val->val.string.len; ++i) LOG_CP(val->val.string.str[i]);
            LOG("'");
            break;
        case VALUE_ARRAY:
            LOG("[\n");
            struct ArrayElement* el = val->val.arr.head;
            while(el) {
                print_value_recur(&el->this, level+1);
                el = el->next;
            }
            for(unint i=0; i<level; ++i) LOG("\t");
            LOG("]");
            break;
        default:
            LOG("Invalid variable type");
    }
    LOG("\n");
    return;
}

void print_value(struct Value* val) {
    print_value_recur(val, 0);
}

// Get Variable number