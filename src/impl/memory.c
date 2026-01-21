#include "nyasm/memory.h"
#include "types.h"
#include <stdlib.h>

#define DEBUG 1

struct memory_t {
    void* start; // Points to the start of the allocated pool of memory
    void* end;
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

nbool memory_init(nint size) {
    memory.size = 0;
    memory.curr = memory.start = malloc(size);
    if(memory.start != NULL) { 
        memory.size = size;
        memory.end = memory.start + size;
        return SUCCESS;
    }
    return FAIL;
}

void* memory_alloc(nint size) {
    if(memory.end - memory.curr <= MEM_CHUNK_SIZE(size)) return NULL; // Cant allocate more memory

    void* curr = memory.curr;
    #if DEBUG
        // Update the last mem dbg trace
        if(last_dbg_trace) last_dbg_trace->next = curr;
        else last_dbg_trace = curr;

        // Set the debug info
        struct memory_dbg_t* last = curr;
        last->size = size;
        last->next = NULL;

        // Update the last dbg trace
        last_dbg_trace = last;

        // Update the current position to the start of the data
        curr += sizeof(struct memory_dbg_t);
        last->chunk = curr;

    #endif

    memory.curr += size;
    return curr;
}

// Maybe change to a bool for better compatiblity
void memory_destroy() {
    free(memory.start);
    return;
}

void memory_dbg() {
    struct memory_dbg_t* curr = memory.start;
    nbool exit = false; 
    if(!last_dbg_trace) return; // No memory allocated yet
    do {
        exit = (curr->next == NULL); 
        printf("[Allocated]: size: %d, data: %p next: %p\n", curr->size, curr->chunk, curr->next);
        curr = curr->next;
    } while(!exit);
}
