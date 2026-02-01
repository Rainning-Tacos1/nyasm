#ifndef CORE_LOG_H
#define CORE_LOG_H

#include <stdio.h>
#include "config.h"

// Choose your debug function or create an implementation

#define LOG(__format, ...) printf(__format, ##__VA_ARGS__)


#endif