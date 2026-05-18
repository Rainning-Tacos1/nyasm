#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#include "lexer.h"
#include "parser.h"
#include "variables.h"
#include "ast.h"

#include "types.h"
#include "token.h"


struct Ast_node* new_ast_node() {
    return (struct Ast_node*)MEM_ALLOC(sizeof(struct Ast_node), "AST node");
}

struct Ast_node* new_ast(struct Parser* p) {
    struct Ast_node* ast = (struct Ast_node*)MEM_ALLOC(sizeof(struct Ast_node), "AST root");
    if(ast == NULL) { 
        memory_error(p, "No available memory for AST's root node");  
        return NULL;
    }

    ast->type = STATEMENTS_NODE;
    ast->node.statements.head = NULL;
    ast->node.statements.tail = NULL;

    return ast;
}

struct Ast_node* new_ast_string(struct Parser* p, struct token* _token) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST string");
        return NULL;
    }

    node->type = LITERAL_NODE;
    node->node.literal.value = new_string(p, _token);

    return (node->node.literal.value != NULL) ? node : NULL;
}

struct Ast_node* new_ast_number(struct Parser* p, struct token* _token, unint is_neg) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST number");
        return NULL;
    }

    node->type = LITERAL_NODE;
    node->node.literal.value = new_number(p, _token, is_neg);

    return (node->node.literal.value != NULL) ? node : NULL;   
}


struct Ast_node* new_ast_binop(struct Parser* p, struct token* op_token, unint op, struct Ast_node* left, struct Ast_node* right) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        _error_from_token(p, op_token, ERROR_TYPE_MEMORY, "no available memory for the AST binop");
        return NULL;
    }

    node->type = BINOP_NODE;

    node->node.binop.op_token = op_token;
    node->node.binop.op = op;
    node->node.binop.left = left;
    node->node.binop.right = right;

    return node;
}

struct Ast_node* new_ast_variable(struct Parser* p, struct token* _token, struct Variable* var) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST variable");
        return NULL;
    }
    
    node->type = VAR_NODE;

    node->node.var = *var;
    return node;
}

struct Ast_node* new_ast_array(struct Parser* p, struct token* _token, struct Value* var) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST array");
        return NULL;
    }
    
    node->type = LITERAL_NODE;

    node->node.literal.value = var;
    return node;
}

struct AstStatementsNode* insert_ast_statement_node(struct Parser* p, struct AstStatements* ast, struct Ast_node* stmt_ast) {
    struct AstStatementsNode* node = (struct AstStatementsNode*)MEM_ALLOC(sizeof(struct AstStatementsNode), "Statement node");
    if(node == NULL) {
        memory_error(p, "No available memory for AST statement node");  
        return NULL;
    }

    node->ast = stmt_ast;
    node->next = NULL;

    // 1st time
    if(ast->head == NULL) ast->head = node;

    if(ast->tail != NULL) ast->tail->next = node;
    ast->tail = node;

    return ast->tail;
}

struct Ast_node* new_ast_if(struct Parser* p, struct Ast_node* cond) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        memory_error(p, "No available memory for AST if node");  
        return NULL;
    }

    node->type = IF_NODE;

    node->node._if._if.cond = cond;
    node->node._if._if.statements.head = NULL;
    node->node._if._if.statements.tail = NULL;
    return node;
}

struct Ast_node* new_ast_assign_variable(struct Parser* p, struct AstIdentifier* ident, struct Ast_node* expr) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        memory_error(p, "No available memory for AST assignment node");  
        return NULL;
    }

    node->type = ASSIGN_VAR_NODE;
    node->node.var_assign.expr = expr;
    node->node.var_assign.name = *ident;

    return node;
}

struct Ast_node* new_ast_assign_variable_idx(struct Parser* p, struct AstIdentifier* ident, struct Ast_node* idx, struct Ast_node* expr) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        memory_error(p, "No available memory for AST assignment+idx node");  
        return NULL;
    }

    node->type = ASSIGN_VAR_IDX_NODE;
    node->node.var_idx_assign.expr = expr;
    node->node.var_idx_assign.idx = idx;
    node->node.var_idx_assign.name = *ident;

    return node;
}

struct Ast_node* new_ast_assign_append_array(struct Parser* p, struct AstIdentifier* ident, struct Ast_node* expr) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) {
        memory_error(p, "No available memory for AST append assignment node");  
        return NULL;
    }

    node->type = ASSIGN_APPEND_ARRAY_NODE;
    node->node.var_array_append_assign.expr = expr;
    node->node.var_array_append_assign.name = *ident;

    return node;
}


void dbg_ast_recur(struct Ast_node* ast, unint level) {
    for(unint i=0; i<level; ++i) LOG("\t");
    LOG("[");
    switch(ast->type) {
        case BINOP_NODE: { LOG("BINOP_NODE"); break; }
        case LITERAL_NODE: { LOG("LITERAL_NODE"); break; }
        case VAR_NODE: { LOG("VARIABLE_NODE"); break; }
        case STATEMENTS_NODE: { LOG("STATEMENTS_NODE"); break; }
        case IF_NODE: { LOG("IF_NODE"); break; }
        case ASSIGN_VAR_NODE: { LOG("ASSIGN_VAR_NODE"); break; }
        case ASSIGN_VAR_IDX_NODE: { LOG("ASSIGN_VAR_IDX_NODE"); break; }
        case ASSIGN_APPEND_ARRAY_NODE: { LOG("ASSIGN_APPEND_ARRAY_NODE"); break; }
        default: { LOG("INVALID_NODE"); break; }
    }
    LOG("] ");

    switch(ast->type) {
        case BINOP_NODE:
            LOG("Op: '");
            switch(ast->node.binop.op) {
                case NOTEQUAL:     { LOG("!="); break; }
                case GREATER:      { LOG(">"); break; }
                case GREATEREQUAL: { LOG(">="); break; }
                case EQEQUAL:      { LOG("=="); break; }
                case LESS:         { LOG("<"); break; }
                case LESSEQUAL:    { LOG("<="); break; }

                case VBAR:         { LOG("|"); break; }
                case CIRCUMFLEX:   { LOG("^"); break; }
                case AMPER:        { LOG("&"); break; }

                case LEFTSHIFT:    { LOG("<<"); break; }
                case RIGHTSHIFT:   { LOG(">>"); break; }

                case PLUS:         { LOG("+"); break; }
                case MINUS:        { LOG("-"); break; }

                case SLASH:        { LOG("/"); break; }
                case DOUBLESLASH:  { LOG("//"); break; }
                case PERCENT:      { LOG("%%"); break; }
                case STAR:         { LOG("*"); break; }

                case TILDE:        { LOG("~"); break; }
                case EXCLAMATION:  { LOG("!"); break; }

                case DOUBLEVBAR:   { LOG("||"); break; }
                case DOUBLEAMPER:  { LOG("&&"); break; }

                case LSQB:         { LOG("idx"); break; }
                case DOT:          { LOG("concat"); break; }

                default: { LOG("X"); break; }
            };
            LOG("'\n");
            dbg_ast_recur(ast->node.binop.left, level+1);
            // Can be NULL if type is MINUS or PLUS to make a negation or positive-ation
            if(ast->node.binop.right != NULL) dbg_ast_recur(ast->node.binop.right, level+1);
            else { 
                for(unint i=0; i<level+1; ++i) LOG("\t");
                LOG("NULL\n");
            }
            break;
        
        case LITERAL_NODE:
            LOG("[");
            switch(ast->node.literal.value->type) {
                case VALUE_INT:    { LOG("INTEGER"); break; }
                case VALUE_STR:    { LOG("STRING"); break; }
                case VALUE_DOUBLE: { LOG("DOUBLE"); break; }
                case VALUE_ARRAY:  { LOG("ARRAY"); break; }

                default:           { LOG("UNKNOWN"); break; }

            }
            LOG("]\n");
            break;
        
        case VAR_NODE:
            for(unint i=0; i<ast->node.var.var.len; ++i) LOG_CP(ast->node.var.var.cps[i]);
            LOG("\n");
            break;
        
        case STATEMENTS_NODE:
            LOG("\n");
            for(struct AstStatementsNode* statement = ast->node.statements.head; statement; statement = statement->next) {
                dbg_ast_recur(statement->ast, level+1);
                LOG("\n");
            }
            break;
        
        case IF_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[CONDITION]\n");

            dbg_ast_recur(ast->node._if._if.cond, level+2);

            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[STATEMENTS]\n");

            struct Ast_node stmts;
            stmts.type = STATEMENTS_NODE;
            stmts.node.statements = ast->node._if._if.statements;
            dbg_ast_recur(&stmts, level+2);

            break;
        
        case ASSIGN_VAR_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[VARIABLE] ");
            for(unint i=0; i<ast->node.var_assign.name.len; ++i) LOG_CP(ast->node.var_assign.name.cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[=]\n");
            dbg_ast_recur(ast->node.var_assign.expr, level+2);
            break;
        
        case ASSIGN_VAR_IDX_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[VARIABLE] ");
            for(unint i=0; i<ast->node.var_assign.name.len; ++i) LOG_CP(ast->node.var_assign.name.cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[IDX]\n");
            dbg_ast_recur(ast->node.var_idx_assign.idx, level+2);
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[=]\n");
            dbg_ast_recur(ast->node.var_idx_assign.expr, level+2);
            break;
        
        case ASSIGN_APPEND_ARRAY_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[VARIABLE] ");
            for(unint i=0; i<ast->node.var_array_append_assign.name.len; ++i) LOG_CP(ast->node.var_assign.name.cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[=]\n");
            dbg_ast_recur(ast->node.var_array_append_assign.expr, level+2);
            break;

        default: { LOG("INVALID_NODE"); break; }
    }
}

void dbg_ast(struct Ast_node* ast) {
    if(ast == NULL) { LOG("AST is <NULL>\n"); return; }
    dbg_ast_recur(ast, 0);
}

