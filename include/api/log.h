#ifndef API_LOG_H
#define API_LOG_H

#include "config.h"

// Public Logging API

// API exposed by the implementation
#include <stdio.h>
#include "../../src/log/api.h"

// Choose your log function or create an implementation
#define LOG(__format, ...) _printf(__format, ##__VA_ARGS__)
#define ERROR(__format, ...) _error(__format, ##__VA_ARGS__)
#define VLOG(__format, __arg) _vprintf((const char*)(__format), __arg)
#define LOG_CP(cp) print_cp(cp)

#endif