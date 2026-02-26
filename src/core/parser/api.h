#ifndef PARSER_H
#define PARSER_H

#include "types.h"

nint parse_indent(char** file, char* end, int32_t* cps, unint* indent, unint* indstack, unint* altindstack);
unint skip_until_next_line(char** file, char* end, int32_t* cps);
#endif