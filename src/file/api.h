#ifndef NYASM_FILE
#define NYASM_FILE

#include <sys/stat.h>
#include <errno.h>

#include "types.h"
#include "config.h"

// Expose implementation API

#define _S_ISREG(mode) S_ISREG(mode)

char* load_file(char* path, unint* _size);
const char* stat_error_message(nint errnum);
#endif