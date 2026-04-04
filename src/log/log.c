#include "api/unicode.h"
#include "types.h"

#include <stdarg.h>
#include <stdio.h>

/*
  Prints a formated string
*/
void _printf(const char* __format, ...) {
    va_list vargs;
    va_start(vargs, __format);
    vprintf(__format, vargs);
    va_end(vargs);
}

void _vprintf(const char* __fmt, va_list __arg) {
    vprintf(__fmt, __arg);
}

/*
  Prints a unicode code point in the native encoding
*/
void print_cp(int32_t cp) {
  printf("%s", CP_TO_ENCODING(cp));
}