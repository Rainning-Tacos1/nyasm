#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "config.h"

struct tok_state;
struct token;
struct Ast_node;
struct Variable;
struct Macro;

struct MacroCallArgs {
    struct token* head;
    struct token* tail;
    struct token* terminator;
    struct MacroCallArgs* next;
};

struct MacroTrace {
    struct Macro* macro;
    const char* file;
    unint lineno;
    struct MacroTrace* parent;
};

struct Parser {
    struct tok_state* tok;

    struct token* head;
    struct token* last_token;
    struct token* peek;
    struct token* tail;

    struct Ast_node* ast;

    struct Variable* variables;
    struct Variable* variables_tail;

    struct Macro* macros;
    struct Macro* macros_tail;
    unint is_inside_macro_decl;

    unint macro_expansion_count;

    unint pending_dedents;

    unint macro_end_cursor;
    struct token* macro_ends[MAX_MACRO_EXPANSION_LIMIT];

    unint ctx_block_stack[MAX_CTX_BLOCK_LEVEL];
    unint ctx_block_cursor;
};

struct Parser* _Parser_New(struct tok_state* tok);
struct Ast_node* _run_parser(struct Parser* p);

void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...);
void memory_error(struct Parser* p, const char* format, ...);

#endif