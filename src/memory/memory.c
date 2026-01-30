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
    // Align the address
    char* header = (char*)NEXT_ALIGNED(memory.curr, MEM_ALIGN);

    #ifdef DEBUG
        char* data_start = (uintptr_t)header + sizeof(struct memory_dbg_t);
        char* data = (char*)NEXT_ALIGNED(data_start, MEM_ALIGN);
        unint talign = (header - memory.curr) + (data - data_start); // Calculate the total alignement done
    #else
        char* data = header;
    #endif

    if(memory.end <= data + size) return NULL; // Can't allocate more memory
    
    #ifdef DEBUG
        // Set the debug info
        struct memory_dbg_t* this = header;
    
        // Update the last mem dbg trace
        if(last_dbg_trace) last_dbg_trace->next = this;

        this->size = size;
        this->next = NULL;
        this->talign = talign;

        // Store the tag
        this->tag = tag;

        // Update the last dbg trace
        last_dbg_trace = this;

        // Update the current position to the start of the data
        this->chunk = data;
    #endif

    // set the data
    memory.curr = (uintptr_t)data + size;
    return data;
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
        DBG("[MEM_ALLOC]: tag: %-20.20s | size: %-10d | data: %p | curr: %p | next: %p | talign: %d\n", curr->tag, curr->size, curr->chunk, curr, curr->next, curr->talign);
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
    DBG("[MEM_STAT]:           Total: %10d (bytes) |           Used: %10d (bytes) |            Free: %10d (bytes) |\n", total, used, free);
    DBG("[MEM_STAT]:           Total: %10d (%%)     |           Used: %10.6f (%%)     |            Free: %10.6f (%%)     |\n", 100, p_used, p_free);

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
    DBG("[MEM_STAT]: Overhead(total): %10d (bytes) | Overhead(full): %10.6f (%%)     | Overhead(alloc): %10.6f (%%)     |\n", overhead, oh_full, oh_alloc);

}
#endif