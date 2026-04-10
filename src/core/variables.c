#include "variables.h"
#include "types.h"

#include "api/memory.h"

#define NULL ((void*)0)

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

    // Link
    if(arr->val.arr.tail != NULL) arr->val.arr.tail->next = arr_el;
    arr->val.arr.tail = arr_el;
    
    // Value
    arr->val.arr.tail->this = val;

    // Update Len
    arr->val.arr.len += 1;

    return SUCCESS;
}

// Integers / Doubles
struct Value* new_number(int32_t* cps, unint len) {
    struct Value* number = (struct Value*)MEM_ALLOC(sizeof(struct Value), "number value");
    if(number == NULL) return NULL;

    // Parse the number
    number->val.number = 0xc0ffe; // For now
    number->type = VALUE_INT;
    
    return number;
}
