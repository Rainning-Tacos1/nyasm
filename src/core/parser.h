#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "config.h"

struct tok_state;
struct token;
struct Ast_node;
struct Variable;
struct Macro;

struct MacroTrace {
    struct Macro* macro;
    unint lineno;
    struct MacroTrace* parent;
};

struct Parser {
    struct tok_state* tok;
    struct token* tokens; // Linked list

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
    unint expanded_macro_is_blank;

    unint macro_expansion_count;
};

struct Parser* _Parser_New(struct tok_state* tok);
void* _run_parser(struct Parser* p);

void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...);

#endif