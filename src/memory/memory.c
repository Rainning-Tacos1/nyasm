// Public API
#include "config.h"
#include "types.h"
#include "api/debug.h"

#include "memory.h"

#include <stdlib.h>

struct memory_t memory;

#ifdef DEBUG
struct memory_dbg_t* last_dbg_trace = NULL;
#endif

// Memory initialization implementation
char* mem_init(nint size) {
    memory.size = 0;
    memory.end = memory.curr = memory.start = malloc(size);
    if(memory.start != NULL) { 
        memory.size = size;
        memory.end += size;
    }
    return memory.start;
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
        char* data_start = header + sizeof(struct memory_dbg_t);
        char* data = (char*)NEXT_ALIGNED(data_start, MEM_ALIGN);
        unint talign = (header - memory.curr) + (data - data_start); // Calculate the total alignement done
    #else
        char* data = header;
    #endif

    if(memory.end <= data + size) return NULL; // Can't allocate more memory
    
    #ifdef DEBUG
        // Set the debug info
        struct memory_dbg_t* this = (struct memory_dbg_t*)header;
    
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
    memory.curr = data + size;
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
    struct memory_dbg_t* curr = (struct memory_dbg_t*)NEXT_ALIGNED(memory.start, MEM_ALIGN);

    if(!last_dbg_trace) return; // No memory allocated yet
    while (curr) {
        DBG("[MEM_ALLOC]: tag: %-20.20s | size: %-10"_UN" | data: %p | curr: %p | next: %p | talign: %"_UN"\n", curr->tag, curr->size, curr->chunk, curr, curr->next, curr->talign);
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
    // mem
    unint total = mem_size();
    unint free = mem_free();
    unint used = (total - free);

    float p_used = (float)used*100.0f / (float)total;
    float p_free = (float)free*100.0f / (float)total;

    // dbg & aln & ovh
    unint header = 0;
    unint align = 0;
    unint data = 0;
    
    if (last_dbg_trace) {
        struct memory_dbg_t *curr = (struct memory_dbg_t*)NEXT_ALIGNED(memory.start, MEM_ALIGN);
    
        while (curr) {
            header += sizeof(struct memory_dbg_t);
            align += curr->talign;
            data += curr->size;
            curr = curr->next;
        }
    }

    float dbg_full = (float)header * 100.0f / (float)total;
    float dbg_alloc = (float)header * 100.0f / used;

    float dat_full = (float)data * 100.0f / (float)total;
    float dat_alloc = (float)data * 100.0f / used;

    float aln_full = (float)align * 100.0f / (float)total;
    float aln_alloc = (float)align * 100.0f / used;

    float ovh_full = (float)(header+align) * 100.0f / (float)total;
    float ovh_alloc = (float)(header+align) * 100.0f / used;

    // Stats
    DBG("[MEM_STAT]: Total(mem): %10"_UN" (bytes) |  Used: %10"_UN" (bytes) | Free: %10"_UN" (bytes) |\n", total, used, free);
    DBG("[MEM_STAT]: Total(mem): %10.6f (%%)     |  Used: %10.6f (%%)     | Free: %10.6f (%%)     |\n", 100.f, p_used, p_free);
    DBG("\n");
    DBG("[MEM_STAT]: Total(dat): %10"_UN" (bytes) |\n", data);
    DBG("[MEM_STAT]: Total(dat): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", dat_full, dat_alloc);
    DBG("\n");
    DBG("[MEM_STAT]: Total(dbg): %10"_UN" (bytes) |\n", header);
    DBG("[MEM_STAT]: Total(dbg): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", dbg_full, dbg_alloc);
    DBG("\n");
    DBG("[MEM_STAT]: Total(aln): %10"_UN" (bytes) |\n", align);
    DBG("[MEM_STAT]: Total(aln): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", aln_full, aln_alloc);
    DBG("\n");
    DBG("[MEM_STAT]: Total(ovh): %10"_UN" (bytes) |\n", (header + align));
    DBG("[MEM_STAT]: Total(ovh): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", ovh_full, ovh_alloc);
}
#endif