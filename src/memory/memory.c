// Public API
#include "config.h"
#include "types.h"
#include "api/debug.h"

#include "memory.h"

#include <stdlib.h>

struct mem_t memory;

// Memory initialization implementation
void* mem_init(nint size) {
    memory.size = 0;
    memory.last_alloc = NULL;
    memory.malloc = memory.end = memory.curr = memory.start = malloc(size);
    if(memory.malloc != NULL) {
        memory.curr = memory.start = (char*)NEXT_ALIGNED(memory.malloc, MEM_ALIGN);
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
    struct mem_alloc_t* alloc = (struct mem_alloc_t*)NEXT_ALIGNED(memory.curr, MEM_ALIGN);
    char* chunk_start = (char*)NEXT_ALIGNED((uintptr_t)alloc + sizeof(struct mem_alloc_t), MEM_ALIGN);
    char* alloc_end = chunk_start + size;

    // Out of memory?
    if(memory.end <= alloc_end) return NULL;

    #ifdef DEBUG
        alloc->dbg.size = size;
        alloc->dbg.talign = (uintptr_t)alloc_end - (uintptr_t)memory.curr - size - sizeof(struct mem_alloc_t);
        alloc->dbg.tag = tag;
    #endif

    // Update current memory
    memory.curr = alloc_end;
    
    // Update Doubly Linked List
    alloc->start=memory.curr;
    alloc->next = NULL;
    alloc->data = chunk_start;

    if(!memory.last_alloc) {
        alloc->prev = NULL;
    } else {
        memory.last_alloc->next = alloc;
        alloc->prev = memory.last_alloc;
    }
    memory.last_alloc = alloc;

    return alloc->data;
}

void* mem_get_last() {
    return memory.last_alloc->data;
}

void mem_free_last() {
    if(!memory.last_alloc) return;

    if(!memory.last_alloc->prev) {
        memory.last_alloc = NULL;
        return;
    }

    // Unlink
    memory.last_alloc = memory.last_alloc->prev;
    memory.last_alloc->next = NULL;

    // Deallocate the data
    memory.curr = memory.last_alloc->start;
}

void* mem_resize_last(unint new_size) {
    struct mem_alloc_t* last = memory.last_alloc;
    if(last == NULL) return NULL; // memory not allocated yet

    char* end = last->data + new_size;
    if(memory.end <= end) return NULL;

    #ifdef DEBUG
        last->dbg.size = new_size;
    #endif

    memory.curr = end;
    return last->data;
}

// Memory deinitialization implementation
void mem_deinit() {
    free(memory.malloc);
    memory.malloc = memory.curr = memory.start = memory.end = NULL;
    memory.last_alloc = NULL;
    memory.size = 0;
    return;
}

#ifdef DEBUG
void mem_dbg() {
    struct mem_alloc_t* curr = (struct mem_alloc_t*)memory.start;

    if(!memory.last_alloc) {
        DBG(DO_MEM_DBG, "[MEM_ALLOC]: No memory allocated\n");
        return; // No memory allocated yet
    }
    while (curr) {
        DBG(DO_MEM_DBG, "[MEM_ALLOC]: tag: %-20.20s | size: %-10"_UN" | data: %p | prev: %p | curr: %p | next: %p | talign: %"_UN"\n", curr->dbg.tag, curr->dbg.size, curr->data, curr->prev, curr, curr->next, curr->dbg.talign);
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

    unint dbg = 0;    // Debug
    unint align = 0;  // Alignment
    unint data = 0;   // Data
    unint all = 0;    // Allocators
    unint ovh = 0;    // Overhead
    
    if (memory.last_alloc) {
        struct mem_alloc_t *curr = (struct mem_alloc_t*)memory.start;
    
        while (curr) {
            dbg += sizeof(struct mem_dbg_t);
            align += curr->dbg.talign;
            data += curr->dbg.size;
            all += sizeof(struct mem_alloc_t) - sizeof(struct mem_dbg_t);
            curr = curr->next;
        }
    }

    ovh = dbg+all+align;

    float dbg_full = (float)dbg * 100.0f / (float)total;
    float dbg_alloc = (float)dbg * 100.0f / used;

    float all_full = (float)all * 100.0f / (float)total;
    float all_alloc = (float)all * 100.0f / used;

    float dat_full = (float)data * 100.0f / (float)total;
    float dat_alloc = (float)data * 100.0f / used;

    float aln_full = (float)align * 100.0f / (float)total;
    float aln_alloc = (float)align * 100.0f / used;

    float ovh_full = (float)ovh * 100.0f / (float)total;
    float ovh_alloc = (float)ovh * 100.0f / used;

    // Stats
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(mem): %10"_UN" (bytes) |  Used: %10"_UN" (bytes) | Free: %10"_UN" (bytes) |\n", total, used, free);
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(mem): %10.6f (%%)     |  Used: %10.6f (%%)     | Free: %10.6f (%%)     |\n", 100.f, p_used, p_free);
    DBG(DO_MEM_DBG, "\n");
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(all): %10"_UN" (bytes) |\n", all);
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(all): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", all_full, all_alloc);
    DBG(DO_MEM_DBG, "\n");
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(dbg): %10"_UN" (bytes) |\n", dbg);
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(dbg): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", dbg_full, dbg_alloc);
    DBG(DO_MEM_DBG, "\n");
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(aln): %10"_UN" (bytes) |\n", align);
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(aln): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", aln_full, aln_alloc);
    DBG(DO_MEM_DBG, "\n");
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(dat): %10"_UN" (bytes) |\n", data);
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(dat): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", dat_full, dat_alloc);
    DBG(DO_MEM_DBG, "\n");
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(ovh): %10"_UN" (bytes) |\n", ovh);
    DBG(DO_MEM_DBG, "[MEM_STAT]: Total(ovh): %10.6f (%%)     | Alloc: %10.6f (%%)     |\n", ovh_full, ovh_alloc);
}
#endif