#ifndef NYASM_FILE
#define NYASM_FILE

#include "types.h"
#include "config.h"

// Expose implementation API

char* load_file(char* path, unint* _size);

#endif