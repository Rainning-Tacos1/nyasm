#include "nyasm/memory.h"
#include "types.h"

// External functions
extern nbool nyasm_memory_init(nint memory_size);
extern void* nyasm_memory_alloc(nint size);
extern void nyasm_memory_destroy();

// Memory interface
struct memory_interface_t nyasm_memory = {
    .init = nyasm_memory_init,
    .alloc = nyasm_memory_alloc,
    .destroy = nyasm_memory_destroy,
};