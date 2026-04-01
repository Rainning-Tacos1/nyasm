#ifndef UNICODE_H
#define UNICODE_H

#include "types.h"
#include "config.h"
#include "./utf8/utf8proc.h"

// Errors provided by unicode implementation
#define UNICODE_OK 0
#define UNICODE_ERR_CODEPOINT -1

#define __UNICODE_CATEGORY_CN UTF8PROC_CATEGORY_CN /**< Other, not assigned */
#define __UNICODE_CATEGORY_LU UTF8PROC_CATEGORY_LU /**< Letter, uppercase */
#define __UNICODE_CATEGORY_LL UTF8PROC_CATEGORY_LL /**< Letter, lowercase */
#define __UNICODE_CATEGORY_LT UTF8PROC_CATEGORY_LT /**< Letter, titlecase */
#define __UNICODE_CATEGORY_LM UTF8PROC_CATEGORY_LM /**< Letter, modifier */
#define __UNICODE_CATEGORY_LO UTF8PROC_CATEGORY_LO /**< Letter, other */
#define __UNICODE_CATEGORY_MN UTF8PROC_CATEGORY_MN /**< Mark, nonspacing */
#define __UNICODE_CATEGORY_MC UTF8PROC_CATEGORY_MC /**< Mark, spacing combining */
#define __UNICODE_CATEGORY_ME UTF8PROC_CATEGORY_ME /**< Mark, enclosing */
#define __UNICODE_CATEGORY_ND UTF8PROC_CATEGORY_ND /**< Number, decimal digit */
#define __UNICODE_CATEGORY_NL UTF8PROC_CATEGORY_NL /**< Number, letter */
#define __UNICODE_CATEGORY_NO UTF8PROC_CATEGORY_NO /**< Number, other */
#define __UNICODE_CATEGORY_PC UTF8PROC_CATEGORY_PC /**< Punctuation, connector */
#define __UNICODE_CATEGORY_PD UTF8PROC_CATEGORY_PD /**< Punctuation, dash */
#define __UNICODE_CATEGORY_PS UTF8PROC_CATEGORY_PS /**< Punctuation, open */
#define __UNICODE_CATEGORY_PE UTF8PROC_CATEGORY_PE /**< Punctuation, close */
#define __UNICODE_CATEGORY_PI UTF8PROC_CATEGORY_PI /**< Punctuation, initial quote */
#define __UNICODE_CATEGORY_PF UTF8PROC_CATEGORY_PF /**< Punctuation, final quote */
#define __UNICODE_CATEGORY_PO UTF8PROC_CATEGORY_PO /**< Punctuation, other */
#define __UNICODE_CATEGORY_SM UTF8PROC_CATEGORY_SM /**< Symbol, math */
#define __UNICODE_CATEGORY_SC UTF8PROC_CATEGORY_SC /**< Symbol, currency */
#define __UNICODE_CATEGORY_SK UTF8PROC_CATEGORY_SK /**< Symbol, modifier */
#define __UNICODE_CATEGORY_SO UTF8PROC_CATEGORY_SO /**< Symbol, other */
#define __UNICODE_CATEGORY_ZS UTF8PROC_CATEGORY_ZS /**< Separator, space */
#define __UNICODE_CATEGORY_ZL UTF8PROC_CATEGORY_ZL /**< Separator, line */
#define __UNICODE_CATEGORY_ZP UTF8PROC_CATEGORY_ZP /**< Separator, paragraph */
#define __UNICODE_CATEGORY_CC UTF8PROC_CATEGORY_CC /**< Other, control */
#define __UNICODE_CATEGORY_CF UTF8PROC_CATEGORY_CF /**< Other, format */
#define __UNICODE_CATEGORY_CS UTF8PROC_CATEGORY_CS /**< Other, surrogate */
#define __UNICODE_CATEGORY_CO UTF8PROC_CATEGORY_CO /**< Other, private use */

struct unicode {
    char* buf;
    char* curr;
    char* end;

    unint nread;

    unint err; // Unicode parser error;

    int32_t cp;
};

nbool read_codepoint(struct unicode* uc);

nbool backup_codepoint(struct unicode* uc);

void unicode_init(struct unicode* uc);

unint unicode_cat(int32_t cp);

unint codepoint_width(int32_t cp);

#endif