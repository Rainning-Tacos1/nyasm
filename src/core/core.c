
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

nbool assemble(char* path) {

    // File
    unint len = 0;
    char* file = LOAD_FILE(path, &len);

    // Make room for a potential implicit new line
    if(file != NULL) file = MEM_RESIZE_LAST(len + SIZEOF_IMPLICIT_NEWLINE);

    if(!file) {
        ERROR("Error loading input file (%s) into memory\n", path);
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
    DBG(1, "Start!\n");

    struct Ast_node* ast = _run_parser(p);
    if(ast == NULL) return FAIL;

    DBG(1, "End!\n");
    dbg_ast(ast);
    DBG(1, "Done DBG!\n");
    
    
    if(eval_ast(p, ast) == FAIL) return FAIL;

    DBG(1, "PRINTING VARTABLES\n");
    for(struct Variable* var = p->variables; var; var = var->next) {
        for(unint i=0; i<var->var_name->len; ++i) LOG_CP(var->var_name->cps[i]);
        LOG(" = ");
        print_value(&var->val);
    }

    return SUCCESS;
}