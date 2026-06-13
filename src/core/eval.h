#ifndef EVAL_H
#define EVAL_H

#include <stdarg.h>
#include "parser.h"
#include "variables.h"
#include "ast.h"
#include "types.h"

/*
Priority:
 - registers
 - structs
 - labels
 - vars

*/

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
    nint addr;

    struct token* name;

    struct LabelDecl* deep_head; // Used for structs
    struct LabelDecl* deep_tail; // Used for structs

    nint len;     // for array fields inside structs, on a label value will be 0
    nint stride;  // for array fields inside structs

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
void print_labels(struct LabelDecl* head, struct LabelDecl* tail, unint level);
struct Variable* get_variable(struct Parser* p, struct token* _token);

#endif