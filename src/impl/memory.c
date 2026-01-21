#include "nyasm/memory.h"
#include "types.h"
#include <stdlib.h>

#define DEBUG 1

struct memory_t {
    void* start; // Points to the start of the allocated pool of memory
    void* curr;  // Points to free memory
    unint size;  // Size of the allocated pool
};

#if DEBUG
struct memory_dbg_t {
    void* chunk;
    unint size;
    struct memory_dbg_t* next;
};
struct memory_dbg_t* last_dbg_trace = NULL;
#endif

#if DEBUG
    #define MEM_CHUNK_SIZE(size1) ((size1) + (sizeof(struct memory_dbg_t)))
#else
    #define MEM_CHUNK_SIZE(size1) (size1)
#endif

struct memory_t memory;

nbool nyasm_memory_init(nint size) {
    memory.size = 0;
    memory.curr = memory.start = malloc(size);
    if(memory.start != NULL) { 
        memory.size = size;
        return SUCCESS;
    }
    return FAIL;
}

void* nyasm_memory_alloc(nint size) {
    if(memory.curr - memory.start >= MEM_CHUNK_SIZE(size)) return NULL; // Cant allocate more memory

    #if DEBUG
        // Update the last mem dbg trace
        if(last_dbg_trace) last_dbg_trace->next = memory.curr;
        else last_dbg_trace = memory.curr;

        // Set the debug info
        struct memory_dbg_t* last = memory.curr;
        last->size = size;
        last->chunk = memory.curr + sizeof(struct memory_dbg_t);
        last->next = NULL;
    #endif

    void* tmp = memory.curr;
    memory.curr += MEM_CHUNK_SIZE(size);
    return tmp;
}

// Maybe change to a bool for better compatiblity
void nyasm_memory_destroy() {
    free(memory.start);
    return;
}
