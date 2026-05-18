#ifndef AST_H
#define AST_H

#include "lexer.h"
#include "types.h"

struct Parser;
struct Value;

enum AST_types {
    // Expression
    BINOP_NODE,
    LITERAL_NODE,
    VAR_NODE,

    STATEMENTS_NODE,
    IF_NODE,

    ASSIGN_VAR_NODE,
    ASSIGN_VAR_IDX_NODE,
    ASSIGN_APPEND_ARRAY_NODE,
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
    struct AstIdentifier name;
    struct Ast_node* expr;
};

struct AstAssignVariableIdx {
    struct AstIdentifier name;
    struct Ast_node* idx;
    struct Ast_node* expr;
};

struct AstAppendArray {
    struct AstIdentifier name;
    struct Ast_node* expr;
};

struct AstStatementsNode {
    struct Ast_node* ast;
    struct AstStatementsNode* next;
};

struct AstStatements {
    struct AstStatementsNode* head;
    struct AstStatementsNode* tail;
};

struct CondBlock {
    struct Ast_node* cond;
    struct AstStatements statements;
};

struct AstIf {
    struct Ast_node* cond;
    struct AstStatements statements;
    // elif branches
    struct CondBlock _if;
};

// Macros

struct MacroArg {
    unint is_variadic;
    struct AstIdentifier arg_name;

    struct MacroArg* next;
};

struct Macro {
    struct AstIdentifier name;
    struct MacroArg* args;
    struct MacroArg* args_tail;
    unint arg_len;

    struct token* tokens;
    unint tok_len;
    unint dedents_needed;
    struct Macro* next;
};

struct Ast_node {
    unint type;

    union {

        // Math & Numbers
        struct AstBinOp binop;
        struct AstLiteral literal;

        // Variable
        struct Variable var;

        // Ast
        struct AstStatements statements;

        struct AstIf _if;

        // Assignments
        struct AstAssignVariable var_assign;
        struct AstAssignVariableIdx var_idx_assign;
        struct AstAppendArray var_array_append_assign;

    } node;

};

struct Ast_node* new_ast(struct Parser* p);

struct Ast_node* new_ast_node();

struct Ast_node* new_ast_string(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_number(struct Parser* p, struct token* _token, unint is_neg);
struct Ast_node* new_ast_binop(struct Parser* p, struct token* op_token, unint op, struct Ast_node* left, struct Ast_node* right);
struct Ast_node* new_ast_variable(struct Parser* p, struct token* _token, struct Variable* var);
struct Ast_node* new_ast_array(struct Parser* p, struct token* _token, struct Value* var);

struct AstStatementsNode* insert_ast_statement_node(struct Parser* p, struct AstStatements* ast, struct Ast_node* stmt_ast);

struct Ast_node* new_ast_if(struct Parser* p, struct Ast_node* cond);

struct Ast_node* new_ast_assign_variable(struct Parser* p, struct AstIdentifier* ident, struct Ast_node* expr);
struct Ast_node* new_ast_assign_variable_idx(struct Parser* p, struct AstIdentifier* ident, struct Ast_node* idx, struct Ast_node* expr);
struct Ast_node* new_ast_assign_append_array(struct Parser* p, struct AstIdentifier* ident, struct Ast_node* expr);

void dbg_ast(struct Ast_node* ast);

#endif