#include "nyasm/memory.h"
#include "types.h"
#include <stdlib.h>

#define PRE_ALLOCATE_MEMORY 0
#define ALLOCATE_AS_NEEDED 1

#define DEBUG

// define memory management strat
#define MEMORY_MANAGEMENT PRE_ALLOCATE_MEMORY

nbool nyasm_memory_init(nint memory_size) {
    #if (MEMORY_MANAGEMENT == PRE_ALLOCATE_MEMORY)
        return SUCCESS;
    #elif (MEMORY_MANAGEMENT == ALLOCATE_AS_NEEDED)
        return SUCCESS;
    #else
        #error "No memory management method selected"
    #endif
}

void* nyasm_memory_alloc(nint size) {
    #if (MEMORY_MANAGEMENT == PRE_ALLOCATE_MEMORY)
        return NULL;
    #elif (MEMORY_MANAGEMENT == ALLOCATE_AS_NEEDED)
        return NULL;
    #else
        #error "No memory management method selected"
    #endif
}