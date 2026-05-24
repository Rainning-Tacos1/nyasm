#ifndef EVAL_H
#define EVAL_H

#include <stdarg.h>
#include "parser.h"
#include "variables.h"
#include "ast.h"
#include "types.h"

struct Variable {
    struct token* var_name;
    struct Value val;
    struct Variable* next;
};


struct Variable* new_variable(struct Parser* p, struct token* _token, struct Value* val);
struct Variable* get_variable(struct Parser* p, struct token* _token);
unint is_variable_declared(struct Parser* p, struct token* _token);

unint eval_ast(struct Parser* p, struct Ast_node* ast);

#endif