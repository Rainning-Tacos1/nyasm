#include "types.h"
#include "config.h"

#include "api/log.h"

#include <stdint.h>

// UFT8 implementation
#include "../../utf8/utf8proc.h"

/*
  Reads a graphme and normalizes it
*/
unint read_grapheme(char** _buf, unint len, uint32_t out[MAX_GRAPHEME_SIZE]) {
    /*
      Iterates thorugh each code point of a grapheme and pushes it into `out`.
      Normalizes the codepoints with `utf8proc_normalize_utf32`
      Sets _buf to the start of the next grapheme
      Returns the number of code points after normalization
    */
    char* buf = *_buf;
    utf8proc_int32_t prev_cp = 0, state = 0;
    utf8proc_ssize_t pos = 0, grapheme_start = 0, cp_count = 0;
    nbool last_grapheme = false;

    while(pos <= len) {
        utf8proc_int32_t cp = 0;
        utf8proc_ssize_t size = utf8proc_iterate(buf+pos, len-pos, &cp);
        if(size < 0) return -1; // Error parsing code point

        if(prev_cp && utf8proc_grapheme_break_stateful(prev_cp, cp, &state)) {
            LOG("  \"");
            for (utf8proc_ssize_t i=grapheme_start; i < pos; i++) {
                LOG("%c", buf[i]);
            }
            LOG("\" (%zd cps) ", cp_count);

            for(utf8proc_ssize_t i=0; i<cp_count; i++) {
                LOG("%04x ", out[i]);
            }
            LOG(" -> ");

            // Grapheme normalization
            utf8proc_ssize_t length = utf8proc_normalize_utf32(out, cp_count, UTF8PROC_COMPOSE);

            if(length < 0) return -2; // Error normalizing grapheme

            LOG(" (%zd cps) ", length);

            for (utf8proc_ssize_t i=0; i < length; i++) {
                LOG("%04x ", out[i]);
            }
            LOG("\n");
            *_buf += pos;
            return length;
            //grapheme_start = pos;
            //cp_count = 0;
        }

        if(size == 0) break; // Trying to read past the last code point
        if(cp_count > MAX_GRAPHEME_SIZE) break; // Break if cp_count > MAX_GRAPHEME_SIZE
        pos += size;
        prev_cp = cp;
        out[cp_count++] = cp; // Push the code point onto the stack
    }
    return 0;
}