#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <stddef.h>

#include "core/memory.h"

struct memory_t {
    void* start; // Points to the start of the allocated pool of memory
    void* end;
    void* curr;  // Points to free memory
    unint size;  // Size of the allocated pool
};

#ifdef DEBUG
struct memory_dbg_t {
    void* chunk;
    unint size;
    struct memory_dbg_t* next;
    char* tag;
};
#endif

#ifdef DEBUG
    #define MEM_CHUNK_SIZE(size1) ((size1) + (sizeof(struct memory_dbg_t)))
#else
    #define MEM_CHUNK_SIZE(size1) (size1)
#endif

#endif