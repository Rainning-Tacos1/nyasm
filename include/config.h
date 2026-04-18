#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

// Undefine to compile without memory traces and dbg stuff
#define DEBUG

#define DO_MEM_DBG 1
#define DO_UC_DBG 0
#define DO_LEXER_TOKEN_DBG 1
#define DO_LEXER_CHAR_DBG 0
#define DO_LEXER_CHAR_VERIFICATION_LOOKAHEAD_DBG 0
#define DO_LEXER_INDENTATION_DBG 0
#define DO_LEXER_PEEK_DEBUG 0
#define DO_LEXER_SPACE_DEBUG 0
#define DO_LEXER_USED_CHARACTER_DBG 0
#define DO_LEXER_TOKEN_FILL_DBG 0
#define DO_LEXER_EOF_DBG 0

#define ERROR_TYPE_MESSAGE "SyntaxError"
#define ERROR_TYPE_MEMORY "MemoryError"
#define ERROR_TYPE_EXPRESSION "ExpressionError"
#define ERROR_TYPE_TYPE "TypeError"
#define ERROR_TYPE_OVERFLOW "OverflowError"

#define MAX_PARENTHESES_LEVEL 100

#define MEM_ALIGN 8

// Indentation related settings
#define MAX_INDENT 64
#define TAB_SIZE 8
#define ALT_TAB_SIZE 1

// Initial size of the allocated buffer for the token's / strings's codepoints
#define TOKEN_CP_BUFFER_INIT_SIZE 32

#define SIZEOF_IMPLICIT_NEWLINE 1

#endif