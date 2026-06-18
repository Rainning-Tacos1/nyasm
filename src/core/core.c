
#include "config.h"
#include "types.h"

#include "api/unicode.h"
#include "api/file.h"
#include "api/log.h"
#include "api/memory.h"
#include "api/debug.h"

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "variables.h"
#include "ast.h"
#include "err.h"
#include "eval.h"

nbool assemble(char* path, char* out_path) {

    // File
    unint len = 0;
    char* file = LOAD_FILE(path, &len);

    // Make room for a potential implicit new line
    if(file != NULL) file = MEM_RESIZE_LAST(len + SIZEOF_IMPLICIT_NEWLINE);

    if(!file) {
        ERROR("Error loading input file (%s) into memory\n", path);
        return FAIL;
    }

    if(OUT_FILE_OPEN(out_path) == FAIL) {
        ERROR("Error accessing output file (%s)\n", out_path);
        return FAIL; 
    }

    // Tokenizer
    struct tok_state* tok = _Tokenizer_tok_new();
    if(tok == NULL) {
        LOG("Error allocating space for the tokenizer\n");
        return FAIL;
    }

    tok->uc.curr = tok->uc.buf = file;
    tok->inp = tok->uc.curr; // Trigger an underflow/verification
    tok->uc.end = file+len; // There is still space for an implicit newline
    tok->source = path;

    // Parser
    struct Parser* p = _Parser_New(tok);
    if(p == NULL) {
        LOG("Error allocating space for the parser\n");
        return FAIL;
    }
    DBG(DO_RUNTIME_DEBUG, "Start!\n");

    struct Ast_node* ast = _run_parser(p);
    if(ast == NULL) return FAIL;

    DBG(DO_RUNTIME_DEBUG, "End!\n");
    #ifdef DEBUG
        dbg_ast(ast);
    #endif
    DBG(DO_RUNTIME_DEBUG, "Done DBG!\n");
    
    unint result = eval_ast(p, ast);
    if(result != EVAL_OK) {
        DBG(DO_RUNTIME_DEBUG, "EVALUATION FAILED. %d\n", result);
        return FAIL;
    }
    DBG(DO_RUNTIME_DEBUG, "PRINTING VARIABLES\n");
    for(struct Variable* var = p->variables; var; var = var->next) {
        for(unint i=0; i<var->var_name->len; ++i) DBG_CP(DO_RUNTIME_DEBUG, var->var_name->cps[i]);
        DBG(DO_RUNTIME_DEBUG, " = ");
        #ifdef DEBUG
            print_value(&var->val);
        #endif
    }
    DBG(DO_RUNTIME_DEBUG, "PRINTING LABELS\n");
    #ifdef DEBUG
        print_labels(p->global_label_decl, p->global_label_decl_tail, 0);
    #endif
    DBG(DO_RUNTIME_DEBUG, "#######################################################################\n");

    p->addr = 0;
    p->last_pass = 1;

    // Clear vars, structs decl
    p->variables = p->variables_tail = NULL;
    p->struct_decl = p->struct_decl_tail = NULL;

    result = eval_ast(p, ast);
    if(result != EVAL_OK) {
        DBG(DO_RUNTIME_DEBUG, "EVALUATION FAILED. %d\n", result);
        return FAIL;
    }
    DBG(DO_RUNTIME_DEBUG, "PRINTING VARIABLES\n");
    for(struct Variable* var = p->variables; var; var = var->next) {
        for(unint i=0; i<var->var_name->len; ++i) DBG_CP(DO_RUNTIME_DEBUG, var->var_name->cps[i]);
        DBG(DO_RUNTIME_DEBUG, " = ");
        #ifdef DEBUG
            print_value(&var->val);
        #endif
    }
    DBG(DO_RUNTIME_DEBUG, "PRINTING LABELS\n");
    #ifdef DEBUG
        print_labels(p->global_label_decl, p->global_label_decl_tail, 0);
    #endif

    OUT_FILE_CLOSE();
    return SUCCESS;
}