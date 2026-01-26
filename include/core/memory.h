#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include "config.h"
#include "memory/memory.h"

// Memory API

// MEM_DBG
#ifdef DEBUG
#define MEM_DBG() mem_dbg()
#else
#define MEM_DBG()
#endif

// MEM_DEINIT
#define MEM_DEINIT() mem_deinit()

// MEM_INIT
#define MEM_INIT(size) mem_init((size))

// MEM_ALLOC
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