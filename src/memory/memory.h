#ifndef _MEMORY_H
#define _MEMORY_H

// Public API
#include "api/memory.h"
#include "types.h"

#include <stddef.h>



#define IS_POWER_OF_2(x) ((x) && (((x) & ((x) - 1)) == 0))

#define REQUIRE_POWER_OF_2(x) \
    _Static_assert(IS_POWER_OF_2(x), "Value must be a power of 2")


#define NEXT_ALIGNED(addr, alignment) \
    ((uintptr_t)(((uintptr_t)(addr) + (uintptr_t)(alignment) - 1) & \
                  ~((uintptr_t)(alignment) - 1)))

// Enforce alignment
REQUIRE_POWER_OF_2(MEM_ALIGN);


// Memory
struct mem_t {
    char* malloc;                  // Points to the start of the allocated pool
    char* start;                   // Points to the start of the first aligned pool of memory
    char* end;                     // Points to the end of memory
    char* curr;                    // Points to free memory
    unint size;                    // Size of the allocated pool
    struct mem_alloc_t* last_alloc; // Last allocated block;
};

#ifdef DEBUG
struct mem_dbg_t {
    unint talign;
    unint size;
    char* tag;
};
#endif

// Intrusive doubly linked list
struct mem_alloc_t {
    #ifdef DEBUG
        struct mem_dbg_t dbg;  // Debug info
    #endif
    struct mem_alloc_t* prev;  // Points to the previous allocation
    struct mem_alloc_t* next;  // Points to the next allocation
    char* start;               // Points to the unaligned allocated chunk of data
    char* data;                // Points to the aligned allocated chunk of data
};


#ifdef DEBUG
    #define MEM_CHUNK_SIZE(size1) ((size1) + (sizeof(struct memory_dbg_t)))
#else
    #define MEM_CHUNK_SIZE(size1) (size1)
#endif

#endif