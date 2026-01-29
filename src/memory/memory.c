// Api exported to the assembler(core)
#include "memory/memory.h"

#include "core/config.h"
#include "core/debug.h"
#include "memory.h"
#include "types.h"

#include <stdlib.h>

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

    char* curr = memory.curr;
    #ifdef DEBUG
        // Update the last mem dbg trace
        if(last_dbg_trace) last_dbg_trace->next = curr;
        else last_dbg_trace = curr;

        // Set the debug info
        struct memory_dbg_t* last = curr;
        last->size = size;
        last->next = NULL;

        // Store the tag
        last->tag = tag;

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
    memory.curr = memory.start = memory.end = NULL;
    memory.size = 0;
    return;
}

#ifdef DEBUG
void mem_dbg() {
    struct memory_dbg_t* curr = memory.start;
    nbool exit = false; 
    if(!last_dbg_trace) return; // No memory allocated yet
    while (curr) {
        DBG("[MEM_ALLOC]: tag: %-20.20s | size: %-10d | data: %p\n", curr->tag, curr->size, curr->chunk);
        curr = curr->next;
    }
}
#endif

unint mem_free() {
    return memory.end - memory.curr;
}

unint mem_size() {
    return memory.size;
}

#ifdef DEBUG
void mem_stats() {
    unint total = mem_size();
    unint free = mem_free();
    unint used = (total - free);

    float p_used = (float)used*100.0f / (float)total;
    float p_free = (float)free*100.0f / (float)total;

    // Stats
    DBG("[MEM_STAT]:          Total: %10d (bytes) |            Used: %10d (bytes) | Free: %10d (bytes)\n", total, used, free);
    DBG("[MEM_STAT]:          Total: %10d (%%)     |            Used: %10.6f (%%)     | Free: %10.6f (%%)\n", 100, p_used, p_free);

    unint overhead = 0;
    
    if (last_dbg_trace) {
        struct memory_dbg_t *curr = memory.start;
    
        while (curr) {
            overhead += sizeof(struct memory_dbg_t);
            curr = curr->next;
        }
    }

    float oh_full = (float)overhead * 100.0f / (float)total;
    float oh_alloc = (float)overhead * 100.0f / used;
    
    // Debug stats
    DBG("[MEM_STAT]: Overhead(full): %10.6f (%%)     | Overhead(alloc): %10.6f (%%)     |\n", oh_full, oh_alloc);

}
#endif