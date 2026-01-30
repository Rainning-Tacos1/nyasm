#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <stddef.h>

#include "core/memory.h"


// Memory alignment
#ifndef MEM_ALIGN
#define MEM_ALIGN 1
#endif

#define IS_POWER_OF_2(x) ((x) && (((x) & ((x) - 1)) == 0))

#define REQUIRE_POWER_OF_2(x) \
    _Static_assert(IS_POWER_OF_2(x), "Value must be a power of 2")


#define NEXT_ALIGNED(addr, alignment) \
    ((uintptr_t)(((uintptr_t)(addr) + (uintptr_t)(alignment) - 1) & \
                  ~((uintptr_t)(alignment) - 1)))

// Enforce alignment
REQUIRE_POWER_OF_2(MEM_ALIGN);


// Memory
struct memory_t {
    char* start; // Points to the start of the allocated pool of memory
    char* end;
    char* curr;  // Points to free memory
    unint size;  // Size of the allocated pool
};

#ifdef DEBUG
struct memory_dbg_t {
    char* chunk;
    unint talign;
    unint size;
    struct memory_dbg_t* next;
    char* tag;
} ;
#endif

#ifdef DEBUG
    #define MEM_CHUNK_SIZE(size1) ((size1) + (sizeof(struct memory_dbg_t)))
#else
    #define MEM_CHUNK_SIZE(size1) (size1)
#endif

#endif