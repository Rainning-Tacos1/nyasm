#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include "types.h"

void _printf(const char* __format, ...);
void _error(const char* __format, ...);
void _vprintf(const char* __fmt, va_list __arg);


void print_cp(int32_t cp);

#endif