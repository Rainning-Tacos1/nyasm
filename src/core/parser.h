#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "config.h"

struct tok_state;
struct token;
struct Ast_node;
struct Variable;

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
};

struct Parser* _Parser_New(struct tok_state* tok);
void* _run_parser(struct Parser* p);

#endif