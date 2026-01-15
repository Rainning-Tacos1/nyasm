#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>

#define SUCCESS true
#define FAIL false

#ifndef ARCH
// Default architecture
#define ARCH 64
#endif

#if ARCH == 32
typedef int32_t nint;
typedef uint32_t unint;

// General Purpose
#define NINT_MAX INT32_MAX
#define UNINT_MAX UINT32_MAX

#else
typedef int64_t nint;
typedef uint64_t unint;

// General Purpose
#define NINT_MAX INT64_MAX
#define UNINT_MAX UINT64_MAX
#endif

#endif