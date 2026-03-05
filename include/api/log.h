#ifndef API_LOG_H
#define API_LOG_H

#include <stdio.h>
#include "config.h"

// Public Logging API

// Choose your log function or create an implementation
#define LOG(__format, ...) printf(__format, ##__VA_ARGS__)


#endif