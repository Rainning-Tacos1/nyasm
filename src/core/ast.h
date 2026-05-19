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
    WHILE_NODE,
    REPEAT_NODE,
    BREAK_NODE,

    ERROR_NODE,
    WARN_NODE,
    ASSERT_NODE,
    INCLUDE_NODE,

    ASSIGN_VAR_NODE,
    ASSIGN_VAR_IDX_NODE,
    ASSIGN_APPEND_ARRAY_NODE,
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

// VAR_NODE

// Move to parser
struct Variable {
    struct token* var_name;
    struct Value val;
    struct Variable* next;
};


struct AstAssignVariable {
    struct token* name;
    struct Ast_node* expr;
};

struct AstAssignVariableIdx {
    struct token* name;
    struct Ast_node* idx;
    struct Ast_node* expr;
};

struct AstAppendArray {
    struct token* name;
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

// @warn, @error, @assert, @include
struct AstWarn {
    struct Ast_node* warn;
    struct token* _s; // For error reporting
    struct token* _e; // For error reporting
};

struct AstError {
    struct Ast_node* error;
    struct token* _s; // For error reporting
    struct token* _e; // For error reporting
};

struct AstAssert {
    struct Ast_node* assert;
    struct token* _s; // For error reporting
    struct token* _e; // For error reporting
};

struct AstInclude {
    struct Ast_node* include;
    struct token* last_token; // To append to the chain
    struct token* _s; // For error reporting
    struct token* _e; // For error reporting
};

// If
struct ElifCondBlock {
    struct Ast_node* cond;
    struct AstStatements statements;
    struct ElifCondBlock* next;
};

struct AstIf {
    struct Ast_node* cond;
    struct AstStatements statements;

    struct ElifCondBlock* _elifs_start;
    struct ElifCondBlock* _elifs_end;

    struct AstStatements* _else;
};

// While
struct AstWhile {
    struct Ast_node* cond;
    struct AstStatements statements;
};

// Repeat
struct AstRepeat {
    struct Ast_node* expr;
    unint iter;
    struct AstStatements statements;
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
        struct token* var;

        // Ast
        struct AstStatements statements;

        struct AstIf _if;
        struct AstWhile _while;
        struct AstRepeat _repeat;


        // Assignments
        struct AstAssignVariable var_assign;
        struct AstAssignVariableIdx var_idx_assign;
        struct AstAppendArray var_array_append_assign;

        // Error, warn, assert, include
        struct AstError error;
        struct AstWarn warn;
        struct AstAssert assert;
        struct AstInclude include;

    } node;

};

struct Ast_node* new_ast(struct Parser* p);

struct Ast_node* new_ast_node();

struct Ast_node* new_ast_string(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_number(struct Parser* p, struct token* _token, unint is_neg);
struct Ast_node* new_ast_binop(struct Parser* p, struct token* op_token, unint op, struct Ast_node* left, struct Ast_node* right);
struct Ast_node* new_ast_variable(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_array(struct Parser* p, struct token* _token, struct Value* var);

struct AstStatementsNode* insert_ast_statement_node(struct Parser* p, struct AstStatements* ast, struct Ast_node* stmt_ast);

struct Ast_node* new_ast_if(struct Parser* p, struct token* _token, struct Ast_node* cond);
unint insert_elif(struct Parser* p, struct token* _token, struct Ast_node* _if, struct Ast_node* cond);
unint insert_else(struct Parser* p, struct token* _token, struct Ast_node* _if);

struct Ast_node* new_ast_while(struct Parser* p, struct token* _token, struct Ast_node* cond);
struct Ast_node* new_ast_while(struct Parser* p, struct token* _token, struct Ast_node* cond);
struct Ast_node* new_ast_repeat(struct Parser* p, struct token* _token, struct Ast_node* expr);
struct Ast_node* new_ast_break(struct Parser* p, struct token* _token);

struct Ast_node* new_ast_assign_variable(struct Parser* p, struct token* ident, struct Ast_node* expr);
struct Ast_node* new_ast_assign_variable_idx(struct Parser* p, struct token* ident, struct Ast_node* idx, struct Ast_node* expr);
struct Ast_node* new_ast_assign_append_array(struct Parser* p, struct token* ident, struct Ast_node* expr);

struct Ast_node* new_ast_assert(struct Parser* p, struct Ast_node* expr, struct token* _s, struct token* _e);
struct Ast_node* new_ast_include(struct Parser* p, struct Ast_node* expr, struct token* _s, struct token* _e, struct token* last_token);
struct Ast_node* new_ast_warn(struct Parser* p, struct Ast_node* expr, struct token* _s, struct token* _e);
struct Ast_node* new_ast_error(struct Parser* p, struct Ast_node* expr, struct token* _s, struct token* _e);

void dbg_ast(struct Ast_node* ast);

#endif