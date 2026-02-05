#ifndef API_DEBUG_H
#define API_DEBUG_H

#include <stdio.h>
#include "config.h"
#include "api/log.h"

// Public Debug API

// Choose your debug function or create an implementation
#ifdef DEBUG
#define DBG(__format, ...) LOG(__format, ##__VA_ARGS__)
#else
#define DBG(...)
#endif

#endif