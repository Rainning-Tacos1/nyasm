#include "types.h"
#include "config.h"

#include "api/log.h"

#include <stdint.h>

// UFT8 implementation
#include "../../utf8/utf8proc.h"

// Helper function
const char* utf8proc_category_to_string(utf8proc_category_t cp) {
    switch (utf8proc_category(cp)) {
        case UTF8PROC_CATEGORY_CN: return "Other, not assigned (CN)";
        case UTF8PROC_CATEGORY_LU: return "Letter, uppercase (LU)";
        case UTF8PROC_CATEGORY_LL: return "Letter, lowercase (LL)";
        case UTF8PROC_CATEGORY_LT: return "Letter, titlecase (LT)";
        case UTF8PROC_CATEGORY_LM: return "Letter, modifier (LM)";
        case UTF8PROC_CATEGORY_LO: return "Letter, other (LO)";
        case UTF8PROC_CATEGORY_MN: return "Mark, nonspacing (MN)";
        case UTF8PROC_CATEGORY_MC: return "Mark, spacing combining (MC)";
        case UTF8PROC_CATEGORY_ME: return "Mark, enclosing (ME)";
        case UTF8PROC_CATEGORY_ND: return "Number, decimal digit (ND)";
        case UTF8PROC_CATEGORY_NL: return "Number, letter (NL)";
        case UTF8PROC_CATEGORY_NO: return "Number, other (NO)";
        case UTF8PROC_CATEGORY_PC: return "Punctuation, connector (PC)";
        case UTF8PROC_CATEGORY_PD: return "Punctuation, dash (PD)";
        case UTF8PROC_CATEGORY_PS: return "Punctuation, open (PS)";
        case UTF8PROC_CATEGORY_PE: return "Punctuation, close (PE)";
        case UTF8PROC_CATEGORY_PI: return "Punctuation, initial quote (PI)";
        case UTF8PROC_CATEGORY_PF: return "Punctuation, final quote (PF)";
        case UTF8PROC_CATEGORY_PO: return "Punctuation, other (PO)";
        case UTF8PROC_CATEGORY_SM: return "Symbol, math (SM)";
        case UTF8PROC_CATEGORY_SC: return "Symbol, currency (SC)";
        case UTF8PROC_CATEGORY_SK: return "Symbol, modifier (SK)";
        case UTF8PROC_CATEGORY_SO: return "Symbol, other (SO)";
        case UTF8PROC_CATEGORY_ZS: return "Separator, space (ZS)";
        case UTF8PROC_CATEGORY_ZL: return "Separator, line (ZL)";
        case UTF8PROC_CATEGORY_ZP: return "Separator, paragraph (ZP)";
        case UTF8PROC_CATEGORY_CC: return "Other, control (CC)";
        case UTF8PROC_CATEGORY_CF: return "Other, format (CF)";
        case UTF8PROC_CATEGORY_CS: return "Other, surrogate (CS)";
        case UTF8PROC_CATEGORY_CO: return "Other, private use (CO)";
        default: return "Unknown category";
    }
}


/*
  Reads a graphme and normalizes it.
  Returns:
    0 .. n : Number of code points after normalization
    -1     : Error parsing code point
    -2     : Error parsing grapheme
    -3     : Error normalizing grapheme
    -4     : Out buffer too small
*/
nint read_grapheme(char** _buf, char* end, int32_t* out) {
    /*
      Iterates thorugh each code point of a grapheme and pushes it into `out`.
      Normalizes the codepoints with `utf8proc_normalize_utf32`
      Sets _buf to the start of the next grapheme
      Returns the number of code points after normalization
    */

    char* buf = *_buf;
    utf8proc_ssize_t len = end - buf;
    utf8proc_int32_t prev_cp = 0, state = 0;
    utf8proc_ssize_t pos = 0, grapheme_start = 0, cp_count = 0;

    while(true) { // After reading the grapheme update len. WE READ A GRAPHEME SO LEN MUST CHANGE
        utf8proc_int32_t cp = 0;
        utf8proc_ssize_t size = utf8proc_iterate((utf8proc_uint8_t*)(buf+pos), len, &cp);
        LOG("POS: %zd LEN: %zd SIZE: %zd CP: %d COUNT: %zd\n", pos, len, size, cp, cp_count);

        if(size < 0) return -1; // Error parsing code point
        if(size == 0 && cp_count != 0) return -2; // Buffer ended before the grapheme ended
        if(size == 0) return 0; // Already at the end of the buffer
        
        // If the condition passes, a return is assured
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
            
            if(length < 0) return -3; // Error normalizing grapheme
            
            LOG(" (%zd cps) ", length);
            
            for (utf8proc_ssize_t i=0; i < length; i++) {
                LOG("%04x ", out[i]);
            }
            LOG("cat: %s \n", utf8proc_category_to_string(*out));
            *_buf += pos;

            return length;
            //grapheme_start = pos;
            //cp_count = 0;
        }
        
        if(cp_count > MAX_GRAPHEME_SIZE-1) return -4; // Cant store more code point onto the buffer
        pos += size;
        prev_cp = cp;
        out[cp_count++] = cp; // Push the code point onto the stack
    }
}