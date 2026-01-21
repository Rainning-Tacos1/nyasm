#include "nyasm/memory.h"
#include "types.h"

// External functions
extern nbool memory_init(nint memory_size);
extern void* memory_alloc(nint size);
extern void memory_destroy();
extern void memory_dbg();

// Memory interface
struct memory_interface_t nyasm_memory = {
    .init = memory_init,
    .alloc = memory_alloc,
    .destroy = memory_destroy,
    .dbg = memory_dbg,
};