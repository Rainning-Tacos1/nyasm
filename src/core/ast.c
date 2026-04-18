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

unint new_ast(struct Parser* p) {
    struct Ast_node* ast = (struct Ast_node*)MEM_ALLOC(sizeof(struct Ast_node), "AST root");
    p->ast = ast;

    if(ast == NULL) return FAIL;

    ast->type = MODULE_NODE;
    ast->node.ast.statements = NULL;

    return SUCCESS;
}

struct Ast_node* new_ast_string(int32_t *cps, unint len) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) return NULL;

    node->type = LITERAL_NODE;
    node->node.literal.value = new_string(cps, len);

    return (node->node.literal.value != NULL) ? node : NULL;
}

struct Ast_node* new_ast_number(int32_t *cps, unint len) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) return NULL;

    node->type = LITERAL_NODE;
    node->node.literal.value = new_number(cps, len);

    return (node->node.literal.value != NULL) ? node : NULL;   
}


struct Ast_node* new_ast_binop(unint op, struct Ast_node* left, struct Ast_node* right, struct token* op_token) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) return NULL;

    node->type = BINOP_NODE;

    node->node.binop.op_token = op_token;
    node->node.binop.op = op;
    node->node.binop.left = left;
    node->node.binop.right = right;

    return node;
}

struct Ast_node* new_ast_variable(struct Variable* var) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) return NULL;
    
    node->type = VAR_NODE;

    node->node.var = *var;
    return node;
}

struct Ast_node* new_ast_array(struct Value* var) {
    struct Ast_node* node = new_ast_node();
    if(node == NULL) return NULL;
    
    node->type = LITERAL_NODE;

    node->node.literal.value = var;
    return node;
}

void dbg_ast_recur(struct Ast_node* ast, unint level) {
    for(unint i=0; i<level; ++i) LOG("\t");
    LOG("[");
    switch(ast->type) {
        case BINOP_NODE: { LOG("BINOP_NODE"); break; }
        case LITERAL_NODE: { LOG("LITERAL_NODE"); break; }
        case VAR_NODE: { LOG("VARIABLE_NODE"); break; }
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

                case DOUBLESTAR:   { LOG("**"); break; }

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
            for(unint i=0; i<ast->node.var.var.len; ++i) DBG_CP(1, ast->node.var.var.cps[i]);
            LOG("\n");
            break;
        default: { LOG("INVALID_NODE"); break; }
    }
}

void dbg_ast(struct Ast_node* ast) {
    if(ast == NULL) { LOG("AST is <NULL>\n"); return; }
    dbg_ast_recur(ast, 0);
}

