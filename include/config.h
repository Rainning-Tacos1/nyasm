#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

// Undefine to compile without memory traces and dbg stuff
// #define DEBUG

#define DO_MEM_DBG 0

#define DO_UC_DBG 0

#define DO_LEXER_TOKEN_DBG 0
#define DO_LEXER_CHAR_DBG 0
#define DO_LEXER_CHAR_VERIFICATION_LOOKAHEAD_DBG 0
#define DO_LEXER_INDENTATION_DBG 0
#define DO_LEXER_PEEK_DEBUG 0
#define DO_LEXER_SPACE_DEBUG 0
#define DO_LEXER_USED_CHARACTER_DBG 0
#define DO_LEXER_TOKEN_FILL_DBG 0
#define DO_LEXER_EOF_DBG 0
#define DO_LEXER_IMPLICIT_NL_DBG 0

#define DO_PARSER_READ_TOKEN_DBG 0
#define DO_PARSER_PEEK_TOKEN_DBG 0

#define DO_MACRO_EXPANSION_DBG 0
#define DO_MACRO_NOT_EXPANDING_DBG 0
#define DO_MACRO_SANITY_CHECK_DBG 0
#define DO_MACRO_CALL_ARGS_DBG 0
#define DO_MACRO_TOTAL_TOKEN_COUNT_DBG 0
#define DO_MACRO_LAST_DEDENTS_DBG 0
#define DO_MACRO_END_MACRO_TOKEN_DBG 0
#define DO_MACRO_PENDING_DEDENTS_DBG 0
#define DO_MACRO_MACRO_END_DBG 0
#define DO_MACRO_RECOVER_TAIL_DBG 0

#define DO_PARSER_RADOM_STUFF_DBG 0

#define DO_EXPRESSION_EVAL_TYPE_CHECK_DBG 0

#define DO_RUNTIME_DEBUG 0

#define ERROR_TYPE_MESSAGE "SyntaxError"
#define ERROR_TYPE_MEMORY "MemoryError"
#define ERROR_TYPE_EXPRESSION "ExpressionError"
#define ERROR_TYPE_TYPE "TypeError"
#define ERROR_TYPE_OVERFLOW "OverflowError"
#define ERROR_TYPE_DIVISION_ERROR "ZeroDivisionError"
#define ERROR_TYPE_INDEX_ERROR "IndexError"
#define ERROR_TYPE_MACRO_LIMIT "MacroLimit"
#define ERROR_TYPE_ASSERT "AssertError"
#define ERROR_TYPE_PATH "PathError"
#define ERROR_TYPE_RUNTIME "RunTimeError"
#define ERROR_TYPE_NAME "NameError"

#define MAX_CTX_BLOCK_LEVEL 100

#define MACRO_EXPANSION_TRACE_START_LIMIT 3
#define MACRO_EXPANSION_TRACE_END_LIMIT 3

#define MAX_MACRO_EXPANSION_LIMIT 100

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