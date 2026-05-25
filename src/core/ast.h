#ifndef AST_H
#define AST_H

#include "asm_lang.h"
#include "lexer.h"
#include "types.h"

struct Parser;
struct Value;

enum AST_types {
    // Expression
    BINOP_NODE,
    LITERAL_NODE,
    VAR_NODE,
    LEN_NODE,
    DOLLAR_NODE,

    STATEMENTS_NODE,

    IF_NODE,       // Done
    WHILE_NODE,    // Done
    REPEAT_NODE,   // Done
    
    BREAK_NODE,    // Done
    CONTINUE_NODE, // Done
    RETURN_NODE,
    FUN_NODE,

    ERROR_NODE,
    WARN_NODE,
    ASSERT_NODE,

    BYTE_NODE,
    WORD_NODE,
    DWORD_NODE,
    QWORD_NODE,
    FLOAT_NODE,
    DOUBLE_NODE,
    PTR_NODE,

    VOID_RETURN_TYPE,

    SAVEB_NODE,
    SAVEW_NODE,
    SAVEDW_NODE,
    SAVEQ_NODE,
    SAVEF_NODE,
    SAVED_NODE,
    SAVEP_NODE,

    LABEL_NODE,

    STRUCT_DECL_NODE, // Done
    STRUCT_VAR_NODE,
    
    IMPORT_NODE, // Plan on removing
    DEL_NODE,    // Done
    STRING_NODE,
    CODE_NODE,
    ALIGN_NODE,

    INSTRUCTION_NODE,
    FUNC_CALL_NODE,

    ASSIGN_VAR_NODE,           // Done
    ASSIGN_APPEND_ARRAY_NODE,  // Done
};

enum BlockContext {
    CTX_GLOBAL,
    CTX_IF,
    CTX_LOOP,
    CTX_FUNC,
};

// An AstBinOp with token type MINUS and right == NULL is the same as negation: -a
struct AstBinOp {
    unint op; // Uses the token type
    struct Ast_node* left;
    struct Ast_node* right;

    // For indexation
    struct token* _s;
    struct token* _e;

    struct token* op_token;
};

struct AstLiteral {
    struct Value* value;
};

struct AstLen {
    struct Ast_node* expr;

    struct token* _s;
    struct token* _e;  
};

// Used to hold variable names/pure text
struct AstIdentifier {
    int32_t* cps;
    unint len; 
};

struct AstAssignVariable {
    struct token* name;
    struct token* ass_token;
    struct Ast_node* idx;
    struct Ast_node* expr;
    unint ass_type;
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
    struct token* _s; // For error reporting
    struct token* _e; // For error reporting
};

// Import
struct AstImport {
    // May change for an expression later
    struct token* import;
};

// del
struct AstDel {
    struct token* ident;
};

struct AstReturn {
    struct token* ident;
};

struct AstSpace {
    struct Ast_node* align_start_expr;
    struct Ast_node* len_expr;
    struct Ast_node* align_per_el_expr;
    unint type;
    
    /* For @byte, ... */ 
    struct Ast_node* value;
    struct token* _s; 
    struct token* _e;

    // When inside functions
    struct token* name;
};

struct AstLabel {
    struct token* name;
};

struct StructDeclField {
    struct token* name;

    unint type;

    // Array
    struct Ast_node* len_expr;
    struct Ast_node* align_per_el_expr;

    struct Ast_node* align_start_expr;

    struct StructDeclField* next;
};

struct AstStructDecl {
    struct token* struct_name;
    struct StructDeclField* head;
    struct StructDeclField* tail;
};

struct StructAssignField {
    struct token* field_name;

    struct Ast_node* value;
    struct StructAssignField* head;
    struct StructAssignField* tail;

    struct StructAssignField* next;
};

struct AstStructVar {
    struct token* struct_name;
    struct token* var_name;
    struct StructAssignField* head;
    struct StructAssignField* tail;
};

struct FuncDeclArg {
    unint type;
    struct token* arg_name;

    struct FuncDeclArg* next;
};

struct AstFuncDecl {
    struct token* calling_conv;
    struct token* func_name;

    struct FuncDeclArg* head;
    struct FuncDeclArg* tail;

    unint return_type;
    struct AstStatements statements;
};

struct AstString {
    struct Ast_node* expr;
    struct token* _s; // For error reporting
    struct token* _e; // For error reporting    

    struct Ast_node* align_start_expr;
};

// Function calls
struct FuncCallArg {
    struct Ast_node* arg_expr;

    struct token* _s; // For error reporting
    struct token* _e; // For error reporting   

    struct FuncCallArg* next;
};


struct AstFuncCall {
    struct token* func_call;

    struct FuncCallArg* args_head;
    struct FuncCallArg* args_tail;
};

struct InstructionArg {
    struct token* _s;
    struct token* _e;

    struct InstructionArg* next;
};

struct AstInstruction {
    struct token* name;

    struct InstructionArg* args_head;
    struct InstructionArg* args_tail;
};

struct AstCode {
    struct asm_lang_t* lang;
};

struct AstAlign {
    struct Ast_node* expr;
    struct token* _s;
    struct token* _e;
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
    unint block_ctx;

    union {

        // Math & Numbers
        struct AstBinOp binop;
        struct AstLiteral literal;

        // Variable
        struct token* var;

        // Len
        struct AstLen len_expr;

        // Ast
        struct AstStatements statements;

        struct AstIf _if;
        struct AstWhile _while;
        struct AstRepeat _repeat;

        // Import
        struct AstImport import;

        // String
        struct AstString string;

        // Return
        struct AstReturn _return;

        // Fun
        struct AstFuncDecl fun_decl;

        // Function call
        struct AstFuncCall func_call;

        // Instruction
        struct AstInstruction instruction;

        // @byte, @word, @dword, @qword, @float, @double
        // @saveb, @savew, @savedw, @saveq, @savef, @saved
        struct AstSpace space;

        struct AstLabel label;

        struct AstStructDecl struct_decl;
        struct AstStructVar struct_var;

        struct AstDel del;

        // Assignments
        struct AstAssignVariable var_assign;

        struct AstAlign align;

        struct AstCode code;

        // Error, warn, assert, include
        struct AstError error;
        struct AstWarn warn;
        struct AstAssert assert;
        struct AstInclude include;

    } node;

};

struct Ast_node* new_ast(struct Parser* p);

struct Ast_node* new_ast_node(struct Parser* p);

struct Ast_node* new_ast_string(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_number(struct Parser* p, struct token* _token, unint is_neg);
struct Ast_node* new_ast_binop(struct Parser* p, struct token* op_token, unint op, struct Ast_node* left, struct Ast_node* right);
struct Ast_node* new_ast_variable(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_array(struct Parser* p, struct token* _token, struct Value* var);
struct Ast_node* new_ast_dollar(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_len(struct Parser* p, struct token* _token, struct token* _s, struct token* _e, struct Ast_node* len);

struct AstStatementsNode* insert_ast_statement_node(struct Parser* p, struct AstStatements* ast, struct Ast_node* stmt_ast);

struct Ast_node* new_ast_if(struct Parser* p, struct token* _token, struct Ast_node* cond);
unint insert_elif(struct Parser* p, struct token* _token, struct Ast_node* _if, struct Ast_node* cond);
unint insert_else(struct Parser* p, struct token* _token, struct Ast_node* _if);

struct Ast_node* new_ast_while(struct Parser* p, struct token* _token, struct Ast_node* cond);
struct Ast_node* new_ast_repeat(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e);
struct Ast_node* new_ast_break(struct Parser* p, struct token* _token);
struct Ast_node* new_ast_continue(struct Parser* p, struct token* _token);

struct Ast_node* new_ast_import(struct Parser* p, struct token* _token, struct token* import_token);

struct Ast_node* new_ast_at_string(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e, struct Ast_node* align_start_expr);

struct Ast_node* new_ast_del(struct Parser* p, struct token* _token, struct token* ident);

struct Ast_node* new_ast_return(struct Parser* p, struct token* _token, struct token* ident);

struct Ast_node* new_ast_space(struct Parser* p, struct token* _token, struct Ast_node* align_start_expr, struct Ast_node* len_expr, struct Ast_node* align_per_el_expr, unint type, /* Extra*/ struct Ast_node* value, struct token* _s, struct token* _e, struct token* name);

struct Ast_node* new_ast_label(struct Parser* p, struct token* _token, struct token* name);

struct Ast_node* new_ast_struct_decl(struct Parser* p, struct token* _token, struct token* struct_name);
unint insert_struct_field(struct Parser* p, struct token* _token, struct AstStructDecl* struct_decl, struct token* name, struct token* struct_name, unint type, struct Ast_node* len_expr, struct Ast_node* align_per_el_expr, struct Ast_node* align_start_expr);

struct Ast_node* new_ast_struct_var(struct Parser* p, struct token* _token, struct token* struct_name, struct token* struct_var_name);
unint insert_struct_field_assignment(struct Parser* p, struct token* _token, struct StructAssignField** head, struct StructAssignField** tail, struct token* field_name, struct Ast_node* value);

struct Ast_node* new_ast_fun_decl(struct Parser* p, struct token* _token, struct token* calling_conv, struct token* func_name, unint return_type);
unint insert_fun_decl_arg(struct Parser* p, struct token* _token, struct AstFuncDecl* func_decl, struct token* arg_name, unint type);

struct Ast_node* new_ast_instruction(struct Parser* p, struct token* _token, struct token* ident);
unint insert_instruction_arg(struct Parser* p, struct token* _token, struct AstInstruction* instruction, struct token* _s, struct token* _e);

struct Ast_node* new_ast_function_call(struct Parser* p, struct token* _token, struct token* ident);
unint insert_func_call_arg(struct Parser* p, struct token* _token, struct AstFuncCall* func_call, struct Ast_node* arg_expr, unint is_p, struct token* _s, struct token* _e);

struct Ast_node* new_ast_code(struct Parser* p, struct token* _token, struct asm_lang_t* lang);

struct Ast_node* new_ast_align(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e);

struct Ast_node* new_ast_assign_variable(struct Parser* p, struct token* ident, struct token* ass_tok, struct Ast_node* idx, struct Ast_node* expr, unint ass_type, unint type);

struct Ast_node* new_ast_assert(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e);
struct Ast_node* new_ast_warn(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e);
struct Ast_node* new_ast_error(struct Parser* p, struct token* _token,  struct Ast_node* expr, struct token* _s, struct token* _e);

void dbg_ast(struct Ast_node* ast);

#endif