#ifndef CORE_DEBUG_H
#define CORE_DEBUG_H

#include <stdio.h>
#include "config.h"

// Choose your debug function or create an implementation
#ifdef DEBUG
#define DBG(__format, ...) printf(__format, ##__VA_ARGS__)
#else
#define DBG()
#endif

#endif