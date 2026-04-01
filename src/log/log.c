#include "../unicode/utf8/utf8proc.h"
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
    char encoding_buffer[4];
    unint n = utf8proc_encode_char(cp, (utf8proc_uint8_t*)encoding_buffer);
    for(unint i=0; i<n; ++i) printf("%c", encoding_buffer[i]);
}