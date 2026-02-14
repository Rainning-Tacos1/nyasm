// Public API
#include "api/debug.h"
#include "api/memory.h"
#include "types.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

char* load_file(char* path, unint* _size) {
    FILE* fp = fopen(path, "r");
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