#ifndef MEMORY_H
#define MEMORY_H

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

// Get the last memory block allocated
void* mem_get_last();

// Memory deallocation
void mem_free_last();

// Memory initialization
void* mem_init(
    nint size
);

// Total memory deinitialization
void mem_deinit();

// Resize the last element
void* mem_resize_last(unint new_size);

// Memory debug
void mem_dbg();

// Amount of free memory
unint mem_free();

// Size of the memory
unint mem_size();

void mem_stats();

#endif