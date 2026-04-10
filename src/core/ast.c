#include "api/memory.h"
#include "api/debug.h"

#include "types.h"
#include "ast.h"
#include "parser.h"
#include "lexer.h"
#include "variables.h"

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

