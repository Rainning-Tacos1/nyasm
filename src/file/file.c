#include "core/config.h"
#include "core/debug.h"
#include "types.h"

#include "core/memory.h"
#include "core/debug.h"

#include <stdio.h>
#include <stdlib.h>

void* load_file(char* path) {
    FILE* fp = fopen(path, "r");
    if(!fp) return NULL;

    // Get the file size
    fseek(fp, 0, SEEK_END);
    unint size = ftell(fp);
    rewind(fp);

    // Load the file into memory
    void* buf = MEM_ALLOC(size, "File");
    if(!buf) return NULL;

    fread(buf, 1, size, fp);
    fclose(fp);
}