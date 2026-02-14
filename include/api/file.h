#ifndef API_FILE_H
#define API_FILE_H

// API exposed by the implementation
#include "../src/file/api.h"

// Public File API
#define LOAD_FILE(path, len) load_file(path, len)
#endif