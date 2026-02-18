#ifndef NYASM_MEMORY
#define NYASM_MEMORY

// Public API
#include "types.h"
#include "config.h"

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
char* mem_init(
    nint size
);

// Total memory deinitialization
void mem_deinit();

// Memory debug
void mem_dbg();

// Amount of free memory
unint mem_free();

// Size of the memory
unint mem_size();

void mem_stats();

#endif