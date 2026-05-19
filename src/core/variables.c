#include <locale.h>
#include <errno.h>

#include "api/memory.h"
#include "api/debug.h"


#include "parser.h"
#include "variables.h"
#include "ast.h"

#include "ctype.h"
#include "types.h"
#include "token.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

// Strings
struct Value* new_string(struct Parser* p, struct token* _token) {
    struct Value* string = (struct Value*)MEM_ALLOC(sizeof(struct Value), "string value");
    if(string == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the string");
        return NULL;
    }

    string->val.string.str = _token->cps+1; // Skip the quote
    string->val.string.len = _token->len-2; // Remove the quotes

    string->type = VALUE_STR;
    return string;
}


// Arrays
struct Value* new_array(struct Parser* p, struct token* _token) {
    struct Value* array = (struct Value*)MEM_ALLOC(sizeof(struct Value), "array value");
    if(array == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the array");
        return NULL;
    }

    array->val.arr.head = NULL;
    array->val.arr.tail = NULL;
    array->val.arr.len = 0;

    array->type = VALUE_ARRAY;
    return array;
}

unint append_array(struct Parser* p, struct token* _token, struct Value* arr, struct Value* val) {
    if(arr->type != VALUE_ARRAY) return FAIL;

    // Array Element
    struct ArrayElement* arr_el = (struct ArrayElement*)MEM_ALLOC(sizeof(struct ArrayElement), "array element value");
    if(arr_el == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the array element");
        return FAIL;
    }

    // 1st time
    if(arr->val.arr.head == NULL) arr->val.arr.head = arr_el;

    // Link
    if(arr->val.arr.tail != NULL) arr->val.arr.tail->next = arr_el;
    arr->val.arr.tail = arr_el;
    
    // Value
    arr->val.arr.tail->next = NULL;
    arr->val.arr.tail->this = *val;

    // Update Len
    arr->val.arr.len += 1;

    return SUCCESS;
}

unint __errno;

#define ABS_NINT_MIN (0-(unint)(NINT_MIN))

/* Static overflow check values for bases 2 through 36.
 * smallmax[base] is the largest unsigned long i such that
 * i * base doesn't overflow unsigned long.
 */
static const unint smallmax[] = {
    0, /* bases 0 and 1 are invalid */
    0,
    UNINT_MAX / 2,
    UNINT_MAX / 3,
    UNINT_MAX / 4,
    UNINT_MAX / 5,
    UNINT_MAX / 6,
    UNINT_MAX / 7,
    UNINT_MAX / 8,
    UNINT_MAX / 9,
    UNINT_MAX / 10,
    UNINT_MAX / 11,
    UNINT_MAX / 12,
    UNINT_MAX / 13,
    UNINT_MAX / 14,
    UNINT_MAX / 15,
    UNINT_MAX / 16,
    UNINT_MAX / 17,
    UNINT_MAX / 18,
    UNINT_MAX / 19,
    UNINT_MAX / 20,
    UNINT_MAX / 21,
    UNINT_MAX / 22,
    UNINT_MAX / 23,
    UNINT_MAX / 24,
    UNINT_MAX / 25,
    UNINT_MAX / 26,
    UNINT_MAX / 27,
    UNINT_MAX / 28,
    UNINT_MAX / 29,
    UNINT_MAX / 30,
    UNINT_MAX / 31,
    UNINT_MAX / 32,
    UNINT_MAX / 33,
    UNINT_MAX / 34,
    UNINT_MAX / 35,
    UNINT_MAX / 36,
};

#if ARCH == 32
static const nint digitlimit[] = {
    0,  0, 32, 20, 16, 13, 12, 11, 10, 10,  /*  0 -  9 */
    9,  9,  8,  8,  8,  8,  8,  7,  7,  7,  /* 10 - 19 */
    7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  /* 20 - 29 */
    6,  6,  6,  6,  6,  6,  6};             /* 30 - 36 */
#elif ARCH == 64
/* [int(math.floor(math.log(2**64, i))) for i in range(2, 37)] */
static const nint digitlimit[] = {
         0,   0, 64, 40, 32, 27, 24, 22, 21, 20,  /*  0 -  9 */
    19,  18, 17, 17, 16, 16, 16, 15, 15, 15,  /* 10 - 19 */
    14,  14, 14, 14, 13, 13, 13, 13, 13, 13,  /* 20 - 29 */
    13,  12, 12, 12, 12, 12, 12};             /* 30 - 36 */
#else
#  error "Need table for ARCH"
#endif

unsigned char _PyLong_DigitValue[256] = {
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  37, 37, 37, 37, 37, 37,
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,
    37, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
};

unint _strtoul(int32_t** ptr, int32_t* str, unint len, unint base) {
    nint c;
    nint ovlimit;
    unint result = 0;

    /* skip leading white space */
    while (len && ISSPACE(*str)) {
        ++str;
        --len;
    }

    if (len == 0) {
        if (ptr) *ptr = (int32_t*)str;
        return 0;
    }

    switch(base) {
        case 0:
            if (len && *str == '0') {
                ++str; --len;

                if (len && (*str == 'x' || *str == 'X')) {
                    /* must have at least one valid hex digit */
                    if (len < 2 ||
                        !ISASCII(str[1]) ||
                        _PyLong_DigitValue[CHARMASK(str[1])] >= 16) {
                        if (ptr) *ptr = (int32_t*)str;
                        return 0;
                    }
                    ++str; --len;
                    base = 16;
                } else if (len && (*str == 'o' || *str == 'O')) {
                    if (len < 2 ||
                        !ISASCII(str[1]) ||
                        _PyLong_DigitValue[CHARMASK(str[1])] >= 8) {
                        if (ptr) *ptr = (int32_t*)str;
                        return 0;
                    }
                    ++str; --len;
                    base = 8;
                } else if (len && (*str == 'b' || *str == 'B')) {
                    if (len < 2 ||
                        !ISASCII(str[1]) ||
                        _PyLong_DigitValue[CHARMASK(str[1])] >= 2) {
                        if (ptr) *ptr = (int32_t*)str;
                        return 0;
                    }
                    ++str; --len;
                    base = 2;
                } else {
                    /* skip all zeroes */
                    while (len && *str == '0') {
                        ++str;
                        --len;
                    }
                    if (ptr) *ptr = (int32_t*)str;
                    return 0;
                }
            } else {
                base = 10;
            }
            break;

        case 16:
            if (len && *str == '0') {
                ++str; --len;
                if (len && (*str == 'x' || *str == 'X')) {
                    if (len < 2 ||
                        !ISASCII(str[1]) ||
                        _PyLong_DigitValue[CHARMASK(str[1])] >= 16) {
                        if (ptr) *ptr = (int32_t*)str;
                        return 0;
                    }
                    ++str; --len;
                }
            }
            break;

        case 8:
            if (len && *str == '0') {
                ++str; --len;
                if (len && (*str == 'o' || *str == 'O')) {
                    if (len < 2 ||
                        !ISASCII(str[1]) ||
                        _PyLong_DigitValue[CHARMASK(str[1])] >= 8) {
                        if (ptr) *ptr = (int32_t*)str;
                        return 0;
                    }
                    ++str; --len;
                }
            }
            break;

        case 2:
            if (len && *str == '0') {
                ++str; --len;
                if (len && (*str == 'b' || *str == 'B')) {
                    if (len < 2 ||
                        !ISASCII(str[1]) ||
                        _PyLong_DigitValue[CHARMASK(str[1])] >= 2) {
                        if (ptr) *ptr = (int32_t*)str;
                        return 0;
                    }
                    ++str; --len;
                }
            }
            break;
    }

    /* catch invalid bases */
    if (base < 2 || base > 36) {
        if (ptr) *ptr = (int32_t*)str;
        return 0;
    }

    /* skip leading zeroes */
    while (len && *str == '0') {
        ++str;
        --len;
    }

    ovlimit = digitlimit[base];

    /* main conversion loop */
    while (len &&
           ISASCII(*str) &&
           ((c = _PyLong_DigitValue[CHARMASK(*str)]) < base)) {

        if (ovlimit > 0) {
            result = result * base + c;
        } else {
            unint temp_result;

            if (ovlimit < 0)
                goto overflowed;

            if (result > smallmax[base])
                goto overflowed;

            result *= base;

            temp_result = result + c;
            if (temp_result < result)
                goto overflowed;

            result = temp_result;
        }

        ++str;
        --len;
        --ovlimit;
    }

    if (ptr) *ptr = (int32_t*)str;
    return result;

overflowed:
    if (ptr) {
        while (len &&
               ISASCII(*str) &&
               _PyLong_DigitValue[CHARMASK(*str)] < base) {
            ++str;
            --len;
        }
        *ptr = (int32_t*)str;
    }
    __errno = ERANGE;
    return (unint)-1;
}

nint _strtol(int32_t** ptr, int32_t* str, unint len, unint base, unint is_neg) {

    int32_t sign;
    unint uresult;
    nint result;

    /* skip leading white space */
    while (len && ISSPACE(*str)) {
        ++str;
        --len;
    }

    sign = *str;
    if (sign == '+') str++;

    uresult = _strtoul(ptr, str, len, base);

    if(uresult <= (unint)NINT_MAX) {
        result = (nint)uresult;
        if(is_neg) result = -result;
    }
    else if(is_neg && uresult == ABS_NINT_MIN) result = NINT_MIN;
    else {
        __errno = ERANGE;
        result = NINT_MAX;
    }
    return result;
}

unint _strtod(struct Parser* parser, struct token* _token, unint is_neg, double* val) {
    if(_token->type != NUMBER) return FAIL; // Not a number token

    int32_t* s = _token->cps;
    unint orig_len = _token->len;

    int32_t* p;
    char *dup, *end;
    int32_t prev;

    // skip if there are no underscores

    // Dupe the string
    dup = MEM_ALLOC(orig_len * sizeof(int32_t) + 1 + 1); // +1 for a possible "-" sign and +1 for the null term
    if(dup == NULL) {
        _error_from_token(parser, _token, ERROR_TYPE_MEMORY, "no available memory to parse the float");
        return FAIL;
    }

    end = dup;

    if(is_neg) *end++ = '-';

    prev = '\0'; // Can be anything that wont cause a parsing error

    p = s;
    for(unint i=0; i<orig_len; ++i, ++p) {
        if(*p == '_') {
            /* Underscores are only allowed after digits. */
            if (!(prev >= '0' && prev <= '9')) goto error;
        }
        else {
            *end++ = (char)*p; // Direct conversion to ASCII is allowed here
            /* Underscores are only allowed before digits. */
            if (prev == '_' && !(*p >= '0' && *p <= '9')) goto error;
        }
        prev = *p;
    }

    /* Underscores are not allowed at the end. */
    if (prev == '_') goto error;
    *end = '\0';

    char* endptr;

    setlocale(LC_NUMERIC, "C");
    double x = strtod(dup, &endptr);

    if (endptr == dup) goto error;
    else if (*endptr != '\0') goto error;
    else if (errno == ERANGE) {
        _error_from_token(parser, _token, ERROR_TYPE_OVERFLOW, "decimal overflow");
        goto fail;
    }

    *val = x;

    MEM_FREE_LAST();
    return SUCCESS;

error:
    _error_from_token(parser, _token, ERROR_TYPE_MESSAGE, "(parser) invalid float literal");
fail:
    MEM_FREE_LAST();
    return FAIL;
}


// Integers / Doubles
struct Value* new_number(struct Parser* p, struct token* _token, unint is_neg) {
    struct Value* number = (struct Value*)MEM_ALLOC(sizeof(struct Value), "number value");
    if(number == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the number");
        return NULL;
    }

    __errno = 0;
    int32_t* endptr;
    nint num = _strtol(&endptr, _token->cps, _token->len, 0, is_neg);
    if((endptr - _token->cps) == _token->len) {
        if(__errno == ERANGE) {
            _error_from_token(p, _token, ERROR_TYPE_OVERFLOW, "number overflow");
            return NULL;
        }

        // Good integer
        number->type = VALUE_INT;
        number->val.number = num;
    
        return number;
    }

    // Try to parse as float
    if(_strtod(p, _token, is_neg, &number->val.flt) == FAIL) return NULL;

    number->type = VALUE_DOUBLE;
    
    return number;
}

struct Variable* new_variable(struct Parser* p, struct token* _token, struct Value* val) {
    struct Variable* var = (struct Variable*)MEM_ALLOC(sizeof(struct Variable), "new variable");
    if(var == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for variable");
        return NULL;
    }

    // Fill
    var->var_name = _token;
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

struct Variable* get_variable(struct Parser* p, struct token* _token) {
    if(p == NULL || p->variables == NULL) return NULL;

    for(struct Variable* var = p->variables; var != NULL; var = var->next) {
        if(var->var_name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(var->var_name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return var;
    }
    return NULL;
}

unint is_variable_declared(struct Parser* p, struct token* _token) {
    return (get_variable(p, _token) == NULL) ? FAIL : SUCCESS;
}

void print_value_recur(struct Value* val, unint level) {
    for(unint i=0; i<level; ++i) LOG("\t");
    switch(val->type) {
        case VALUE_INT:
            LOG("Number: %lld", val->val.number);
            break;
        case VALUE_DOUBLE:
            LOG("Float: %f", val->val.flt);
            break;
        case VALUE_STR:
            LOG("String: '");
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
            LOG("Invalid variable type: %d\n", val->type);
    }
    LOG("\n");
    return;
}

void print_value(struct Value* val) {
    print_value_recur(val, 0);
}

// Get Variable number