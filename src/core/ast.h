#ifndef AST_H
#define AST_H

#include "lexer.h"
#include "types.h"

struct Parser;
struct Value;

enum AST_types {
    INCLUDE_NODE,
    BINOP_NODE,
    LITERAL_NODE,
    VAR_NODE,
    ASSIGN_VAR_NODE,
    STATEMENTS_NODE,
    MODULE_NODE,
};

struct AstInclude {
    struct Ast_node* path;
};

// An AstBinOp with token type MINUS and right == NULL is the same as negation: -a
struct AstBinOp {
    unint op; // Uses the token type
    struct Ast_node* left;
    struct Ast_node* right;

    struct token* op_token;
};

struct AstLiteral {
    struct Value* value;
};

// Used to hold variable names/pure text
struct AstIdentifier {
    int32_t* cps;
    unint len; 
};

struct Variable {
    struct AstIdentifier var;
    struct Value val;
    struct Variable* next;
};

struct AstAssignVariable {
    struct AstIdentifier* name;
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

        // Math & Numbers
        struct AstBinOp binop;
        struct AstLiteral literal;

        // Variable
        struct Variable var;

        // Ast
        struct Statements statements;
        struct AstModule ast;
    } node;

};

unint new_ast(struct Parser* p);

struct Ast_node* new_ast_string(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_number(struct Parser* p, struct token* _token, unint is_neg);
struct Ast_node* new_ast_binop(struct Parser* p, struct token* op_token, unint op, struct Ast_node* left, struct Ast_node* right);
struct Ast_node* new_ast_variable(struct Parser* p, struct token* _token, struct Variable* var);
struct Ast_node* new_ast_array(struct Parser* p, struct token* _token, struct Value* var);

void dbg_ast(struct Ast_node* ast);

#endif