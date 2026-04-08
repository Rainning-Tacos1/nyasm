#ifndef PARSER_H
#define PARSER_H

#include "types.h"
#include "config.h"
#include "lexer.h"

struct Parser {
    struct tok_state* tok;
    struct token* tokens; // Linked list
    struct token* last_token;
};

struct Parser* _Parser_New(struct tok_state* tok);
void* _run_parser(struct Parser* p);

#endif