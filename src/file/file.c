#include <sys/stat.h>
#include <errno.h>

// Public API
#include "api/debug.h"
#include "api/memory.h"
#include "types.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

FILE* out_file = NULL;

// Errno to message
const char* stat_error_message(nint errnum) {
    switch (errnum) {
        case ENOENT:    return "File does not exist";
        case EACCES:    return "Permission denied";
        case ENOTDIR:   return "A component of the path is not a directory";
        case ELOOP:     return "Too many symbolic links";
        case ENAMETOOLONG: return "File path is too long";
        case EFAULT:    return "Bad address";
        case EMFILE:    return "Too many files open in the process";
        case ENFILE:    return "Too many files open in the system";
        case EOVERFLOW: return "Value too large for defined data type";
        default:        return "An unknown error occurred";
    }
}

char* load_file(char* path, unint* _size) {
    FILE* fp = fopen(path, "rb");
    if(!fp) return NULL;

    // Get the file size
    fseek(fp, 0, SEEK_END);
    unint size = ftell(fp);
    rewind(fp);

    // Load the file into memory
    void* buf = MEM_ALLOC(size, "File");
    if(!buf) { *_size=0; return NULL; }

    fread(buf, 1, size, fp);
    fclose(fp);
    *_size = size;
    return buf;
}

unint out_file_open(char* path) {
    return ((out_file = fopen(path, "wb")) == NULL) ? FAIL : SUCCESS;
}

void out_file_close() {
    fclose(out_file);
    out_file = NULL;
}

void out_file_write_byte(unsigned char byte) {
    fputc((int)byte, out_file);
}