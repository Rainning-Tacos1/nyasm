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
    struct Variable* prev;
};

struct StructsDecl {
    struct AstStructDecl* ast_struct_decl;
    struct StructsDecl* next; 
};

/*
struct FuncDecl {
    struct AstFuncDecl* ast_func_decl;
    struct FuncDecl* next; 
};
*/

struct LabelDecl {
    struct AstLabel* label;
    nint addr;

    struct LabelDecl* next;
};

enum EvalTypes {
    EVAL_OK,
    EVAL_ERROR,
    EVAL_BREAK,
    EVAL_CONTINUE,
    // EVAL_RETURN
};

// Ast Evaluation

unint eval_ast(struct Parser* p, struct Ast_node* ast);

#endif