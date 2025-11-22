#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#if ARCH == 32
typedef int32_t nint;
typedef uint32_t nuint;
#else
typedef int64_t nint;
typedef uint64_t nuint;
#endif

#endif