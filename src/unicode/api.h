#ifndef UNICODE_H
#define UNICODE_H

#include "types.h"
#include "config.h"
#include "./utf8/utf8proc.h"

// Errors provided by read_grapheme
#define UNICODE_OK 0
#define UNICODE_ERR_CODEPOINT -1
#define UNICODE_ERR_GRAPHEME -2
#define UNICODE_ERR_EOF -3
#define UNICODE_ERR_NORMALIZE -4
#define UNICODE_ERR_TOO_SMALL -5

#define __UNICODE_CATEGORY_CN 0 /**< Other, not assigned */
#define __UNICODE_CATEGORY_LU 1 /**< Letter, uppercase */
#define __UNICODE_CATEGORY_LL 2 /**< Letter, lowercase */
#define __UNICODE_CATEGORY_LT 3 /**< Letter, titlecase */
#define __UNICODE_CATEGORY_LM 4 /**< Letter, modifier */
#define __UNICODE_CATEGORY_LO 5 /**< Letter, other */
#define __UNICODE_CATEGORY_MN 6 /**< Mark, nonspacing */
#define __UNICODE_CATEGORY_MC 7 /**< Mark, spacing combining */
#define __UNICODE_CATEGORY_ME 8 /**< Mark, enclosing */
#define __UNICODE_CATEGORY_ND 9 /**< Number, decimal digit */
#define __UNICODE_CATEGORY_NL 10 /**< Number, letter */
#define __UNICODE_CATEGORY_NO 11 /**< Number, other */
#define __UNICODE_CATEGORY_PC 12 /**< Punctuation, connector */
#define __UNICODE_CATEGORY_PD 13 /**< Punctuation, dash */
#define __UNICODE_CATEGORY_PS 14 /**< Punctuation, open */
#define __UNICODE_CATEGORY_PE 15 /**< Punctuation, close */
#define __UNICODE_CATEGORY_PI 16 /**< Punctuation, initial quote */
#define __UNICODE_CATEGORY_PF 17 /**< Punctuation, final quote */
#define __UNICODE_CATEGORY_PO 18 /**< Punctuation, other */
#define __UNICODE_CATEGORY_SM 19 /**< Symbol, math */
#define __UNICODE_CATEGORY_SC 20 /**< Symbol, currency */
#define __UNICODE_CATEGORY_SK 21 /**< Symbol, modifier */
#define __UNICODE_CATEGORY_SO 22 /**< Symbol, other */
#define __UNICODE_CATEGORY_ZS 23 /**< Separator, space */
#define __UNICODE_CATEGORY_ZL 24 /**< Separator, line */
#define __UNICODE_CATEGORY_ZP 25 /**< Separator, paragraph */
#define __UNICODE_CATEGORY_CC 26 /**< Other, control */
#define __UNICODE_CATEGORY_CF 27 /**< Other, format */
#define __UNICODE_CATEGORY_CS 28 /**< Other, surrogate */
#define __UNICODE_CATEGORY_CO 29 /**< Other, private use */

struct unicode {
    char* buf;
    char* curr;
    char* end;

    unint err; // Unicode parser error;

    int32_t cps[MAX_GRAPHEME_SIZE];
};

nbool read_grapheme(struct unicode* uc, unint* nread);

void unicode_init(struct unicode* uc);

unint unicode_cat(int32_t cp);

#endif