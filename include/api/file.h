#ifndef API_FILE_H
#define API_FILE_H

// API exposed by the implementation
#include "../../src/file/api.h"

typedef struct stat _stat;
#define __S_ISREG(mode) _S_ISREG(mode)

// Public File API
#define LOAD_FILE(path, len) load_file(path, len)
#define STAT_ERR_MSG(errnum) stat_error_message(errnum)
#define STAT(__file, __buf) stat(__file, __buf)

#define OUT_FILE_OPEN(path) out_file_open(path)
#define OUT_FILE_CLOSE() out_file_close()
#define OUT_FILE_WRITE_BYTE(byte) out_file_write_byte(byte)

#endif