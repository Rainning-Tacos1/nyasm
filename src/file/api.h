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

unint out_file_open(char* path);
void out_file_write_byte(unsigned char byte);
void out_file_close();
#endif