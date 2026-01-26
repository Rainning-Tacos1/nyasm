#include "memory/memory.h"
#include "memory.h"
#include "types.h"
#include <stdlib.h>
#include <stdio.h>

struct memory_t memory;

#ifdef DEBUG
struct memory_dbg_t* last_dbg_trace = NULL;
#endif

// Memory initialization implementation
nbool mem_init(nint size) {
    memory.size = 0;
    memory.end = memory.curr = memory.start = malloc(size);
    if(memory.start != NULL) { 
        memory.size = size;
        memory.end += size;
        return SUCCESS;
    }
    return FAIL;
}

// Memory allocation implementation
void* mem_alloc(
#ifdef DEBUG
    unint size, char* tag
#else
    unint size
#endif 
){
    if(memory.end - memory.curr <= MEM_CHUNK_SIZE(size)) return NULL; // Can't allocate more memory

    void* curr = memory.curr;
    #ifdef DEBUG
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

    memory.curr += MEM_CHUNK_SIZE(size);
    return curr;
}

// Memory deinitialization implementation
void mem_deinit() {
    free(memory.start);
    return;
}

void mem_dbg() {
    struct memory_dbg_t* curr = memory.start;
    nbool exit = false; 
    if(!last_dbg_trace) return; // No memory allocated yet
    do {
        exit = (curr->next == NULL); 
        printf("[Allocated]: size: %d, data: %p next: %p\n", curr->size, curr->chunk, curr->next);
        curr = curr->next;
    } while(!exit);
}
