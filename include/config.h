#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

// Undefine to compile without memory traces and dbg stuff
#define DEBUG

#define DO_MEM_DBG 1
#define DO_UC_DBG 0
#define DO_LEXER_TOKEN_DBG 1
#define DO_LEXER_CHAR_DBG 0
#define DO_LEXER_CHAR_VERIFICATION_LOOKAHEAD_DBG 0


#define MEM_ALIGN 8

// Indentation related settings
#define MAX_INDENT 64
#define TAB_SIZE 8
#define ALT_TAB_SIZE 1

// Max number of bytes to encode a grapheme in an encoding format
#define MAX_ENCODING_SIZE 64

// Initial size of the allocated buffer for the token's codepoints
#define TOKEN_CP_BUFFER_INIT_SIZE 32

#endif