#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "config.h"
#include "memory/memory.h"

// Memory API

// MEM_STATS - If debug is enabled, show memory stats
#ifdef DEBUG
#define MEM_STATS() mem_stats()
#else
#define MEM_STATS()
#endif

// MEM_SIZE - Returns the memory size in bytes
#define MEM_SIZE() mem_size()

// MEM_FREE - Returns the number of free bytes in memory
#define MEM_FREE() mem_free()

// MEM_DBG - If debug is enabled, show a log of memory allocations
#ifdef DEBUG
#define MEM_DBG() mem_dbg()
#else
#define MEM_DBG()
#endif

// MEM_DEINIT - Deinit all memory
#define MEM_DEINIT() mem_deinit()

// MEM_INIT - Init memory
#define MEM_INIT(size) mem_init((size))

// MEM_ALLOC - Allocate n bytes of memory
#define __MEM_ALLOC(_1,_2,NAME,...) NAME
#ifdef DEBUG
    #define MEM_ALLOC(...) \
        __MEM_ALLOC(__VA_ARGS__, MEM_ALLOC_2, MEM_ALLOC_1)(__VA_ARGS__)

    #define MEM_ALLOC_1(size) mem_alloc(size, NULL)
    #define MEM_ALLOC_2(size, extra) mem_alloc(size, extra)
#else
    #define MEM_ALLOC(size, ...) mem_alloc(size)
#endif

#endif