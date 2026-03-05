#include "types.h"
#include "config.h"

#include "api/debug.h"

#include <stdint.h>

// UFT8 implementation
#include "./utf8/utf8proc.h"

#include "api.h"
#include "unicode.h"

/*
  Returns the unicode category of a code point
*/
unint unicode_cat(int32_t cp) { return (unint)utf8proc_category((int32_t)cp); }

// Helper function
const char* utf8proc_category_to_string(int32_t cp) {
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
  Init unicode structure with blank data
*/
void unicode_init(struct unicode* uc) {
    uc->buf = NULL;
    for(unint i=0; i<MAX_GRAPHEME_SIZE; ++i) uc->cps[i] = 0;
    uc->curr = NULL;
    uc->end = NULL;
}

/*
  Converts an array of unicode codepoints into a encoding format(UTF-8)
  Returns:
    0: If successfull
    1: If it failed
*/
nbool unicode_to_encoding(int32_t* cps, unint cp_len, char* out, unint len) {
    unint out_i = 0;

    for(unint i=0; i<cp_len; ++i) {
        unint n = utf8proc_encode_char(cps[i], out+out_i); // 0 if it failed
        out_i += n;
        if(out_i >= len || !n) return FAIL; // >= so that we have one more for '\0'; 
    }
    out[out_i] = '\0';
    return SUCCESS;
}

/*
  Reads a graphme and normalizes it.
  Returns:
    0      : Success
        sets nread to the number of code points after normalization
    1      : Error
*/
nbool read_grapheme(struct unicode* uc, unint* nread) {
    /*
      Iterates thorugh each code point of a grapheme and pushes it into `out`.
      Normalizes the codepoints with `utf8proc_normalize_utf32`
      Sets ur->curr to the start of the next grapheme
      Returns the number of code points after normalization
    */

    char* buf = uc->curr;
    int32_t* out = uc->cps;

    utf8proc_ssize_t len = uc->end - buf;
    utf8proc_int32_t prev_cp = 0, state = 0;
    utf8proc_ssize_t pos = 0, grapheme_start = 0, cp_count = 0;

    while(true) {
        utf8proc_int32_t cp = 0;
        utf8proc_ssize_t size = utf8proc_iterate((utf8proc_uint8_t*)(buf+pos), len, &cp);

        if(size < 0) UNICODE_RETURN(uc, UNICODE_ERR_CODEPOINT); // Error parsing code point
        if(size == 0 && cp_count != 0) UNICODE_RETURN(uc, UNICODE_ERR_GRAPHEME); // Buffer ended before the grapheme ended
        if(size == 0) UNICODE_RETURN(uc, UNICODE_ERR_EOF); // Already at the end of the buffer
        
        // If the condition passes, a return is assured
        if(prev_cp && utf8proc_grapheme_break_stateful(prev_cp, cp, &state)) {
            DBG(DO_UC_DBG, "  \"");
            for (utf8proc_ssize_t i=grapheme_start; i < pos; i++) {
                DBG(DO_UC_DBG, "%c", buf[i]);
            }
            DBG(DO_UC_DBG, "\" (%zd cps) ", cp_count);
            
            for(utf8proc_ssize_t i=0; i<cp_count; i++) {
                DBG(DO_UC_DBG, "%04x ", out[i]);
            }
            DBG(DO_UC_DBG, " -> ");
            
            // Grapheme normalization
            utf8proc_ssize_t length = utf8proc_normalize_utf32(out, cp_count, UTF8PROC_COMPOSE);
            
            if(length < 0) UNICODE_RETURN(uc, UNICODE_ERR_NORMALIZE); // Error normalizing grapheme
            
            DBG(DO_UC_DBG, " (%zd cps) ", length);
            
            for (utf8proc_ssize_t i=0; i < length; i++) {
                DBG(DO_UC_DBG, "%04x ", out[i]);
            }
            DBG(DO_UC_DBG, "cat: %s \n", utf8proc_category_to_string(*out));
            uc->curr += pos;

            *nread = length;
            UNICODE_RETURN(uc, UNICODE_OK);
        }
        
        if(cp_count > MAX_GRAPHEME_SIZE-1) UNICODE_RETURN(uc, UNICODE_ERR_TOO_SMALL); // Cant store more code point onto the buffer
        pos += size;
        prev_cp = cp;
        out[cp_count++] = cp; // Push the code point onto the stack
    }
}