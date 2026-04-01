#ifndef API_DEBUG_H
#define API_DEBUG_H

#include <stdio.h>
#include "config.h"
#include "api/log.h"

// Public Debug API

// Choose your debug function or create an implementation
#ifdef DEBUG

#define DBG(DEBUG, fmt, ...) DBG_SELECT(DEBUG)(fmt, ##__VA_ARGS__)

#define DBG_SELECT(x) DBG_SELECT_IMPL(x)
#define DBG_SELECT_IMPL(x) DBG_IMPL_##x

#define DBG_IMPL_1(fmt, ...) LOG(fmt, ##__VA_ARGS__)
#define DBG_IMPL_0(fmt, ...) ((void)0)
#else
#define DBG(...)
#endif

// Debug a cp
#ifdef DEBUG

#define DBG_CP(DEBUG, cp) DBG_CP_SELECT(DEBUG)(cp)

#define DBG_CP_SELECT(x) DBG_CP_SELECT_IMPL(x)
#define DBG_CP_SELECT_IMPL(x) DBG_CP_IMPL_##x

#define DBG_CP_IMPL_1(cp) LOG_CP(cp)
#define DBG_CP_IMPL_0(cp) ((void)0)
#else
#define DBG_CP(...)
#endif

#endif