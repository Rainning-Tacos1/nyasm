#ifndef AST_H
#define AST_H

#include "types.h"
#include "variables.h"

struct Parser; 

enum AST_types {
    INCLUDE_NODE,
    BINOP_NODE,
    LITERAL_NODE,
    ASSIGN_VAR_NODE,
    DECLARE_VAR_NODE,
    STATEMENTS_NODE,
    MODULE_NODE,
};

struct AstInclude {
    struct Ast_node* path;
};

enum BinOps {
    OP_ADD,
    OP_SUB,
    OP_DIV,
    OP_MUL,
    OP_NEG,
};

struct AstBinOp {
    unint op;
    struct Ast_node* left;
    struct Ast_node* right;
};

struct AstLiteral {
    struct Value* value;
};

// Used to hold variable names/pure text
struct AstIdentifier {
    int32_t* str;
    unint len; 
};

struct AstAssignVariable {
    struct AstIdentifier name;
    struct Ast_node* value;
};

struct AstDeclareVariable {
    struct AstIdentifier name;
    struct Ast_node* value;
};

struct Statements {
    struct Ast_node* ast;
    struct Statements* next;
};

struct AstModule {
    struct Statements* statements;
};

struct Ast_node {
    unint type;

    union{
        struct AstInclude include;

        struct AstAssignVariable var_assign;
        struct AstDeclareVariable var_declare;

        // Math & Numbers
        struct AstBinOp op;
        struct AstLiteral literal;

        // Ast
        struct Statements statements;
        struct AstModule ast;
    } node;

};

unint new_ast(struct Parser* p);
struct Ast_node* new_ast_string(int32_t *cps, unint len);
struct Ast_node* new_ast_number(int32_t *cps, unint len);

#endif