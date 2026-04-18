#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <inttypes.h>

#define SUCCESS 0
#define FAIL 1

#ifndef ARCH
// Default architecture
#define ARCH 64
#endif

#if ARCH == 32
typedef int32_t nint;
typedef uint32_t unint;
typedef unint nbool;
#define _N PRId32
#define _UN PRIu32

// General Purpose
#define NINT_MAX INT32_MAX
#define NINT_MIN INT32_MIN

#define UNINT_MAX UINT32_MAX

#else
typedef int64_t nint;
typedef uint64_t unint;
typedef unint nbool;
#define _N PRId64
#define _UN PRIu64

// General Purpose
#define NINT_MAX INT64_MAX
#define NINT_MIN INT64_MIN

#define UNINT_MAX UINT64_MAX
#endif

#endif