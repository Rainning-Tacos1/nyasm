#ifndef API_DEBUG_H
#define API_DEBUG_H

#include <stdio.h>
#include "config.h"
#include "api/log.h"

// Public Debug API

// Choose your debug function or create an implementation
#ifdef DEBUG
//#define DBG(DEBUG, __format, ...) LOG(__format, ##__VA_ARGS__)
#define DBG(DEBUG, fmt, ...) DBG_SELECT(DEBUG)(fmt, ##__VA_ARGS__)

#define DBG_SELECT(x) DBG_SELECT_IMPL(x)
#define DBG_SELECT_IMPL(x) DBG_IMPL_##x

#define DBG_IMPL_1(fmt, ...) LOG(fmt, ##__VA_ARGS__)
#define DBG_IMPL_0(fmt, ...) ((void)0)
#else
#define DBG(...)
#endif

#endif