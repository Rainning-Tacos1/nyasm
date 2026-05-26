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
  Backup one code point
  You cannot backup from EOF as the previous number of bytes read(nread) is replaced with 0
  Author:  Chat-GPT + Human
  Returns:
    0      : Success
    1      : Error
*/
nbool backup_codepoint(struct unicode* uc) {
    if (uc->err != UNICODE_OK || uc->curr <= uc->buf || uc->cp == -1 || uc->nread == 0)
        return FAIL;

    const unsigned char* p = (const unsigned char*)uc->curr - 1;

    while (p > (const unsigned char*)uc->buf &&
           ((*p & 0xC0) == 0x80))
    {
        p--;
    }

    uc->curr = (const char*)p;
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

    const char* buf = uc->curr;
    utf8proc_ssize_t len = uc->end - buf;

    utf8proc_ssize_t size = utf8proc_iterate((utf8proc_uint8_t*)(buf), len, cp);
    if(size < 0) UNICODE_FAIL(uc, UNICODE_ERR_CODEPOINT); // Error parsing code point

    *nread = size;
    uc->curr += size;

    if(size == 0) {
        DBG(DO_UC_DBG, "\tUnicode: \"EOF\" cat: EOF\n");
        *cp = EOF;
        UNICODE_SUCCESS(uc);
    }

    // Debug
    DBG(DO_UC_DBG, "\tUnicode: \"");

    if(IS_VALID_CP(*cp)) DBG_CP(DO_UC_DBG, *cp);
    else DBG(DO_UC_DBG, "ERR");

    DBG(DO_UC_DBG, "\" (U+%04X) cat: ", *cp);

    if(IS_VALID_CP(*cp)) DBG(DO_UC_DBG, "%s", utf8proc_category_to_string(*cp));
    else DBG(DO_UC_DBG, "ERR");

    DBG(DO_UC_DBG, "\n");

    UNICODE_SUCCESS(uc);
}

/*
  Returns the width of a code point
*/
unint codepoint_width(int32_t cp) {
    return (unint)utf8proc_charwidth(cp);
}

/*
  Converts a unicode codepoint into a encoding format(UTF-8)
  Returns:
    a pointer to the encoded codepoint
*/
unsigned char* codepoint_to_encoding(int32_t cp) {
    static unsigned char __unicode_to_encoding[__CP_ENCODING_BUF];
    return codepoint_to_encoding_buf(cp, __unicode_to_encoding);
}

unint codepoint_to_encoding_buf_get_len(int32_t cp, unsigned char* buf) {
    unint len = utf8proc_codepoint_valid(cp) ?
        utf8proc_encode_char(cp, buf) :
        utf8proc_encode_char(0xFFFD, buf);
    buf[len] = '\0';
    return len+1;
}

unsigned char* codepoint_to_encoding_buf(int32_t cp, unsigned char* buf) {
    codepoint_to_encoding_buf_get_len(cp, buf);
    return buf;
}

unint write_implicit_newline(char* buf) {
    *buf = '\n';
    return SIZEOF_IMPLICIT_NEWLINE;
}

nint normalize_codepoints(int32_t* cps, unint len) {
    return utf8proc_normalize_utf32(cps, len, UTF8PROC_COMPOSE);
}