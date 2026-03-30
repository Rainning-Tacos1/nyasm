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
    uc->buf = uc->curr = uc->end = NULL;
    uc->cp = EOF;
    uc->err = UNICODE_OK;
    uc->nread = 0;
}

/*
  Converts an array of unicode codepoints into a encoding format(UTF-8)
  Returns:
    0: If successfull
    1: If it failed
*/
nbool unicode_to_encoding(int32_t* cps, unint cp_len, unsigned char* out, unint len) {
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
  Backup one code point
  Author:  Chat-GPT
  Returns:
    0      : Success
    1      : Error
*/
nbool backup_cp(struct unicode* uc) {
    if (uc->err != UNICODE_OK || uc->nread == 0)
        return FAIL;

    // Step 1: go to start of current codepoint
    char* p = uc->curr - uc->nread;

    if (p < uc->buf) return FAIL; 

    // We assume that if the first condition doesn't fail, there must be a valid utf-8 sequence that wont make p go below uc->buf
    while (p > uc->buf && ((*p & 0xC0) == 0x80)) {
        p--;
    }

    uc->curr = p;
    return SUCCESS;
}

/*
  Reads a codepoint.
  Returns:
    0      : Success
    1      : Error
      sets error code in err
*/
nbool read_codepoint(struct unicode* uc) {
    unint* nread = &uc->nread;
    int32_t* cp = &uc->cp;

    char* buf = uc->curr;
    utf8proc_ssize_t len = uc->end - buf;

    utf8proc_ssize_t size = utf8proc_iterate((utf8proc_uint8_t*)(buf), len, cp);
    if(size < 0) UNICODE_FAIL(uc, UNICODE_ERR_CODEPOINT); // Error parsing code point

    *nread = size;
    uc->curr += size;

    if(size == 0) {
        DBG(DO_UC_DBG, "  \"EOF\" cat: EOF\n");
        *cp = EOF;
        UNICODE_SUCCESS(uc);
    }

    // Debug
    DBG(DO_UC_DBG, "  \"");

    nbool suc;
    unsigned char enc[5]; // Max UTF-8 character is 4 bytes + 1 '\0'
    if((suc = unicode_to_encoding(cp, 1, enc, sizeof(enc))) == FAIL) DBG(DO_UC_DBG, "ERR");
    else DBG(DO_UC_DBG, "%s", enc);

    DBG(DO_UC_DBG, "\" cat: ");
    if(suc == FAIL) DBG(DO_UC_DBG, "ERR");
    else DBG(DO_UC_DBG, "%s", utf8proc_category_to_string(*cp));
    DBG(DO_UC_DBG, "\n");

    UNICODE_SUCCESS(uc);
}