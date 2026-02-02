#ifndef NYASM_MEMORY
#define NYASM_MEMORY

#include "types.h"
#include "core/config.h"

// Expose implementation API

// Memory allocation
void* mem_alloc(
#ifdef DEBUG
    unint size, char* tag
#else
    unint size
#endif
);

// Memory initialization
nbool mem_init(
    nint size
);

// Total memory deinitialization
void mem_deinit();

// Memory debug
void mem_dbg();

// Amount of free memory
inline unint mem_free();

// Size of the memory
inline unint mem_size();

void mem_stats();

#endif