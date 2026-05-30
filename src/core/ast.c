#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#include "asm_lang.h"
#include "lexer.h"
#include "parser.h"
#include "variables.h"
#include "ast.h"

#include "types.h"
#include "token.h"

extern const char * const _Parser_TokenNames[];

struct Ast_node* new_ast_node(struct Parser* p) {
    struct Ast_node* node = (struct Ast_node*)MEM_ALLOC(sizeof(struct Ast_node), "AST node");
    if(node == NULL) return NULL;

    node->block_ctx = p->ctx_block_stack[p->ctx_block_cursor-1];

    return node;
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
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST string");
        return NULL;
    }

    node->type = LITERAL_NODE;
    node->node.literal.value = new_string(p, _token);

    return (node->node.literal.value != NULL) ? node : NULL;
}

struct Ast_node* new_ast_number(struct Parser* p, struct token* _token, unint is_neg) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST number");
        return NULL;
    }

    node->type = LITERAL_NODE;
    node->node.literal.value = new_number(p, _token, is_neg);

    return (node->node.literal.value != NULL) ? node : NULL;   
}

struct Ast_node* new_ast_dollar(struct Parser* p, struct token* _token) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST dollar node");
        return NULL;
    }

    node->type = DOLLAR_NODE;
    node->node.dollar = _token;
    return node;
}

struct Ast_node* new_ast_binop(struct Parser* p, struct token* op_token, unint op, struct Ast_node* left, struct Ast_node* right) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, op_token, ERROR_TYPE_MEMORY, "no available memory for the AST binop node");
        return NULL;
    }

    node->type = BINOP_NODE;

    node->node.binop.op_token = op_token;
    node->node.binop.op = op;
    node->node.binop.left = left;
    node->node.binop.right = right;

    return node;
}

struct Ast_node* new_ast_len(struct Parser* p, struct token* _token, struct token* _s, struct token* _e, struct Ast_node* len) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the @len AST node");
        return NULL;
    }

    node->type = LEN_NODE;
    node->node.len_expr.expr = len;
    node->node.len_expr._s = _s;
    node->node.len_expr._e = _e;

    return node;
}

struct Ast_node* new_ast_variable(struct Parser* p, struct token* _token) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for the AST variable node");
        return NULL;
    }
    
    node->type = VAR_NODE;

    node->node.var = _token;
    return node;
}

struct Ast_node* new_ast_array(struct Parser* p, struct token* _token, struct Value* var) {
    struct Ast_node* node = new_ast_node(p);
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

struct Ast_node* new_ast_if(struct Parser* p, struct token* _token, struct Ast_node* cond) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST if node");
        return NULL;
    }

    node->type = IF_NODE;

    node->node._if.cond = cond;
    node->node._if.statements.head = NULL;
    node->node._if.statements.tail = NULL;

    node->node._if._elifs_start = NULL;
    node->node._if._elifs_end = NULL;
    node->node._if._else = NULL;

    return node;
}

struct Ast_node* new_ast_while(struct Parser* p, struct token* _token, struct Ast_node* cond) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST while node");
        return NULL;
    }

    node->type = WHILE_NODE;

    node->node._while.cond = cond;
    node->node._while.statements.head = NULL;
    node->node._while.statements.tail = NULL;

    return node;
}

struct Ast_node* new_ast_repeat(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST repeat node");
        return NULL;
    }

    node->type = REPEAT_NODE;

    node->node._repeat.expr = expr;
    node->node._repeat.statements.head = NULL;
    node->node._repeat.statements.tail = NULL;
    node->node._repeat._s = _s;
    node->node._repeat._e = _e;

    return node;
}

struct Ast_node* new_ast_break(struct Parser* p, struct token* _token) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST break node");
        return NULL;
    }

    node->type = BREAK_NODE;
    return node;
}

struct Ast_node* new_ast_continue(struct Parser* p, struct token* _token) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST continue node");
        return NULL;
    }

    node->type = CONTINUE_NODE;
    return node;
}

unint insert_elif(struct Parser* p, struct token* _token, struct Ast_node* _if, struct Ast_node* cond) {
    struct ElifCondBlock* node = (struct ElifCondBlock*)MEM_ALLOC(sizeof(struct ElifCondBlock), "elif cond+block");
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST elif node"); 
        return FAIL;
    }

    // Link and append
    node->next = NULL;

    if(_if->node._if._elifs_start == NULL) _if->node._if._elifs_start = node;

    if(_if->node._if._elifs_end != NULL) _if->node._if._elifs_end->next = node;
    _if->node._if._elifs_end = node;

    return SUCCESS;
}

unint insert_else(struct Parser* p, struct token* _token, struct Ast_node* _if) {
    struct AstStatements* node = (struct AstStatements*)MEM_ALLOC(sizeof(struct AstStatements), "else block");
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST else node"); 
        return FAIL;
    }

    _if->node._if._else = node;
    return SUCCESS;
}

struct Ast_node* new_ast_assign_variable(struct Parser* p, struct token* ident, struct token* ass_tok, struct Ast_node* idx, struct Ast_node* expr, unint ass_type, unint type) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, ident, ERROR_TYPE_MEMORY, "no available memory for AST assignment+idx node");
        return NULL;
    }

    node->type = type;
    node->node.var_assign.ass_token = ass_tok;
    node->node.var_assign.name = ident;
    node->node.var_assign.expr = expr;
    node->node.var_assign.idx = idx;
    node->node.var_assign.ass_type = ass_type;

    return node;
}

// Warn, error, include, assert
struct Ast_node* new_ast_error(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST error node");
        return NULL;
    }

    node->type = ERROR_NODE;
    node->node.error.error = expr;
    node->node.error._s = _s;
    node->node.error._e = _e;

    return node;
}

struct Ast_node* new_ast_warn(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST warn node");
        return NULL;
    }

    node->type = WARN_NODE;
    node->node.warn.warn = expr;
    node->node.warn._s = _s;
    node->node.warn._e = _e;

    return node;
}
/*
struct Ast_node* new_ast_import(struct Parser* p, struct token* _token, struct token* import_token) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST import node");
        return NULL;
    }

    node->type = IMPORT_NODE;
    node->node.import.import = import_token;

    return node;
}
*/

struct Ast_node* new_ast_space(struct Parser* p, struct token* _token, struct Ast_node* align_start_expr, struct Ast_node* len_expr, struct Ast_node* align_per_el_expr, unint type, /* Extra*/ struct Ast_node* value, struct token* _s, struct token* _e/*, struct token* name */) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST space node");
        return NULL;
    }

    node->type = type;

    node->node.space.len_expr = len_expr;
    node->node.space.align_per_el_expr = align_per_el_expr;
    node->node.space.align_start_expr = align_start_expr;

    node->node.space.value = value;
    node->node.space._s = _s;
    node->node.space._e = _e;

    // node->node.space.name = name;
    node->node.space.space_ident = _token;

    return node;
}

struct Ast_node* new_ast_label(struct Parser* p, struct token* _token, struct token* name/*,  unint is_inside_func */) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST label node"); 
        return NULL;
    }

    node->type = LABEL_NODE;
    node->node.label.name = name;
    // node->node.label.is_inside_func = is_inside_func;

    return node;
}

struct Ast_node* new_ast_assert(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST assert node");
        return NULL;
    }

    node->type = ASSERT_NODE;
    node->node.assert.assert = expr;
    node->node.assert._s = _s;
    node->node.assert._e = _e;

    return node;
}

struct Ast_node* new_ast_struct_decl(struct Parser* p, struct token* _token, struct token* struct_name) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST struct declaration node");
        return NULL;
    }

    node->type = STRUCT_DECL_NODE;

    node->node.struct_decl.struct_name = struct_name;
    node->node.struct_decl.head = NULL;
    node->node.struct_decl.tail = NULL;

    return node;
}

unint insert_struct_field(struct Parser* p, struct token* _token, struct AstStructDecl* struct_decl, struct token* name, struct token* struct_name, unint type, struct Ast_node* len_expr, struct Ast_node* align_per_el_expr, struct Ast_node* align_start_expr) {
    struct StructDeclField* field = (struct StructDeclField*)MEM_ALLOC(sizeof(struct StructDeclField), "struct field");
    if(field == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST struct field node"); 
        return FAIL;
    }

    field->name = name;
    field->type = type;
    field->len_expr = len_expr;
    field->align_per_el_expr = align_per_el_expr;
    field->align_start_expr = align_start_expr;
    field->next = NULL;
    field->struct_name = struct_name;

    // Link
    if(struct_decl->head == NULL) struct_decl->head = field;

    if(struct_decl->tail != NULL) struct_decl->tail->next = field;
    struct_decl->tail = field;

    return SUCCESS;
}

struct Ast_node* new_ast_struct_var(struct Parser* p, struct token* _token, struct token* struct_name, struct token* struct_var_name) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST struct variable node");
        return NULL;
    }

    node->type = STRUCT_VAR_NODE;
    node->node.struct_var.struct_name = struct_name;
    node->node.struct_var.var_name = struct_var_name;
    node->node.struct_var.head = NULL;
    node->node.struct_var.tail = NULL;

    return node;
}

unint insert_struct_field_assignment(struct Parser* p, struct token* _token, struct StructAssignField** head, struct StructAssignField** tail, struct token* field_name, struct Ast_node* value, struct token* _s, struct token* _e) {
    struct StructAssignField* field = (struct StructAssignField*)MEM_ALLOC(sizeof(struct StructAssignField), "struct assignment field");
    if(field == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for struct assignment field"); 
        return FAIL;
    }

    field->field_name = field_name;
    field->value = value;
    field->head = NULL;
    field->tail = NULL;

    field->_s = _s;
    field->_e = _e;

    field->next = NULL;

    // Link
    if(*head == NULL) *head = field;

    if(*tail != NULL) (*tail)->next = field;
    *tail = field;

    return SUCCESS;
}

/*
struct Ast_node* new_ast_fun_decl(struct Parser* p, struct token* _token, struct token* calling_conv, struct token* func_name, unint return_type) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST function node");
        return NULL;
    }

    node->type = FUN_NODE;

    node->node.fun_decl.calling_conv = calling_conv;
    node->node.fun_decl.func_name = func_name;
    node->node.fun_decl.return_type = return_type;

    node->node.fun_decl.head = NULL;
    node->node.fun_decl.tail = NULL;

    return node;
}


unint insert_fun_decl_arg(struct Parser* p, struct token* _token, struct AstFuncDecl* func_decl, struct token* arg_name, unint type) {
    struct FuncDeclArg* arg = (struct FuncDeclArg*)MEM_ALLOC(sizeof(struct FuncDeclArg), "function declaration arg");
    if(arg == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for function declaration arg"); 
        return FAIL;
    }

    arg->arg_name = arg_name;
    arg->type = type;
    arg->next = NULL;

    // Link
    if(func_decl->head == NULL) func_decl->head = arg;

    if(func_decl->tail != NULL) func_decl->tail->next = arg;
    func_decl->tail = arg;

    return SUCCESS;
}

unint insert_func_call_arg(struct Parser* p, struct token* _token, struct AstFuncCall* func_call, struct Ast_node* arg_expr, unint is_p, struct token* _s, struct token* _e) {
    struct FuncCallArg* arg = (struct FuncCallArg*)MEM_ALLOC(sizeof(struct FuncCallArg), "function call arg");
    if(arg == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for function call arg"); 
        return FAIL;
    }

    arg->arg_expr = arg_expr;
    arg->_s = _s;
    arg->_e = _e;
    arg->next = NULL;

    // Link
    if(func_call->args_head == NULL) func_call->args_head = arg;

    if(func_call->args_tail != NULL) func_call->args_tail->next = arg;
    func_call->args_tail = arg;

    return SUCCESS;
}
*/

struct Ast_node* new_ast_at_string(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e, struct Ast_node* align_start_expr, struct token* string) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST string node");
        return NULL;
    }

    node->type = STRING_NODE;

    node->node.string.expr = expr;
    node->node.string._s = _s;
    node->node.string._e = _e;
    node->node.string.align_start_expr = align_start_expr;
    node->node.string.string = string;

    return node;
}

struct Ast_node* new_ast_del(struct Parser* p, struct token* _token, struct token* ident) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST string node");
        return NULL;
    }

    node->type = DEL_NODE;
    node->node.del.ident = ident;

    return node;
}

/*
struct Ast_node* new_ast_return(struct Parser* p, struct token* _token, struct token* ident) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST return node");
        return NULL;
    }

    node->type = RETURN_NODE;
    node->node._return.ident = ident;

    return node;
}


struct Ast_node* new_ast_function_call(struct Parser* p, struct token* _token, struct token* ident) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST function call node");
        return NULL;
    }

    node->type = FUNC_CALL_NODE;
    node->node.func_call.func_call = ident;
    node->node.func_call.args_head = NULL;
    node->node.func_call.args_tail = NULL;

    return node;
}
*/

unint insert_instruction_arg(struct Parser* p, struct token* _token, struct AstInstruction* instruction, struct token* _s, struct token* _e) {
    struct InstructionArg* arg = (struct InstructionArg*)MEM_ALLOC(sizeof(struct InstructionArg), "instruction arg");
    if(arg == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for instruction arg"); 
        return FAIL;
    }

    arg->_s = _s;
    arg->_e = _e;
    arg->next = NULL;

    // Link
    if(instruction->args_head == NULL) instruction->args_head = arg;

    if(instruction->args_tail != NULL) instruction->args_tail->next = arg;
    instruction->args_tail = arg;

    instruction->arg_count += 1;

    return SUCCESS;
}

struct Ast_node* new_ast_instruction(struct Parser* p, struct token* _token, struct token* ident/*, unint is_inside_func */) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST instruction node");
        return NULL;
    }

    node->type = INSTRUCTION_NODE;
    node->node.instruction.arg_count = 0;
    node->node.instruction.name = ident;
    node->node.instruction.args_head = NULL;
    node->node.instruction.args_tail = NULL;
    // node->node.instruction.is_inside_func = is_inside_func;

    return node;
}

struct Ast_node* new_ast_align(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST align node");
        return NULL;
    }

    node->type = ALIGN_NODE;
    node->node.align.expr = expr;
    node->node.align._s = _s;
    node->node.align._e = _e;

    return node;
}

struct Ast_node* new_ast_org(struct Parser* p, struct token* _token, struct Ast_node* expr, struct token* _s, struct token* _e, struct token* org) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST org node");
        return NULL;
    }

    node->type = ORG_NODE;
    node->node.org.expr = expr;
    node->node.org._s = _s;
    node->node.org._e = _e;
    node->node.org.org = org;

    return node;
}

struct Ast_node* new_ast_code(struct Parser* p, struct token* _token, struct asm_lang_t* lang) {
    struct Ast_node* node = new_ast_node(p);
    if(node == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for AST code node");
        return NULL;
    }

    node->type = CODE_NODE;
    node->node.code.lang = lang;

    return node;
}

void dbg_ast_recur(struct Ast_node *ast, unint level);

void dbg_struct_var_fields(struct StructAssignField* head, unint level) {
    for(struct StructAssignField* field = head; field; field = field->next) {
        for(unint i=0; i<level; ++i) LOG("\t");
        LOG("[FIELD] ");
        for(unint i=0; i<field->field_name->len; ++i) LOG_CP(field->field_name->cps[i]);
        LOG("\n");
        if(field->value) dbg_ast_recur(field->value, level+1);
        else dbg_struct_var_fields(field->head, level+1);
    }
}


char* assign_token_to_text(unsigned int ass) {
    switch (ass) {
        case EQUAL:
            return "=";

        case PLUSEQUAL:
            return "+=";

        case MINEQUAL:
            return "-=";

        case STAREQUAL:
            return "*=";

        case SLASHEQUAL:
            return "/=";

        case PERCENTEQUAL:
            return "%=";

        case AMPEREQUAL:
            return "&=";

        case VBAREQUAL:
            return "|=";

        case CIRCUMFLEXEQUAL:
            return "^=";

        case LEFTSHIFTEQUAL:
            return "<<=";

        case RIGHTSHIFTEQUAL:
            return ">>=";

        default:
            return "UNKNOWN";
    }
}

#define IS_SAVE_TYPE(type) ((type) == SAVEB_NODE || (type) == SAVEW_NODE || (type) == SAVEDW_NODE || (type) == SAVEQ_NODE || (type) == SAVEF_NODE || (type) == SAVED_NODE || (type) == SAVEP_NODE)


void dbg_ast_recur(struct Ast_node* ast, unint level) {
    for(unint i=0; i<level; ++i) LOG("\t");
    LOG("[");
    switch(ast->type) {
        case BINOP_NODE: { LOG("BINOP_NODE"); break; }
        case LITERAL_NODE: { LOG("LITERAL_NODE"); break; }
        case VAR_NODE: { LOG("VARIABLE_NODE"); break; }
        case LEN_NODE: { LOG("LEN_NODE"); break; }
        case DOLLAR_NODE: { LOG("DOLLAR_NODE]\n"); return; }


        case STATEMENTS_NODE: { LOG("STATEMENTS_NODE"); break; }
    
        case IF_NODE: { LOG("IF_NODE"); break; }
    
        case WHILE_NODE: { LOG("WHILE_NODE"); break; }
        case REPEAT_NODE: { LOG("REPEAT_NODE"); break; }
        case BREAK_NODE: { LOG("BREAK_NODE]\n"); return; }
        case CONTINUE_NODE: { LOG("CONTINUE]\n"); return; }

        // case IMPORT_NODE: { LOG("IMPORT_NODE"); break; }

        case STRING_NODE: { LOG("STRING_NODE"); break; }

        case DEL_NODE: { LOG("DEL_NODE"); break; }

        // case RETURN_NODE: { LOG("RETURN_NODE"); break; }

        // case FUN_NODE: { LOG("FUN_NODE"); break; }

        // case FUNC_CALL_NODE: { LOG("FUNC_CALL_NODE"); break; }

        case BYTE_NODE: { LOG("BYTE_NODE"); break; }
        case WORD_NODE: { LOG("WORD_NODE"); break; }
        case DWORD_NODE : { LOG("DWORD_NODE"); break; }
        case QWORD_NODE : { LOG("QWORD_NODE"); break; }
        case FLOAT_NODE : { LOG("FLOAT_NODE"); break; }
        case DOUBLE_NODE : { LOG("DOUBLE_NODE"); break; }
        case PTR_NODE : { LOG("PTR_NODE"); break; }

        case SAVEB_NODE: { LOG("SAVEB_NODE"); break; }
        case SAVEW_NODE: { LOG("SAVEW_NODE"); break; }
        case SAVEDW_NODE: { LOG("SAVEDW_NODE"); break; }
        case SAVEQ_NODE: { LOG("SAVEQ_NODE"); break; }
        case SAVEF_NODE: { LOG("SAVEF_NODE"); break; }
        case SAVED_NODE: { LOG("SAVED_NODE"); break; }
        case SAVEP_NODE: { LOG("SAVEP_NODE"); break; }

        case LABEL_NODE: { LOG("LABEL_NODE"); break; }

        case STRUCT_DECL_NODE: { LOG("STRUCT_DECL_NODE"); break; }
        case STRUCT_VAR_NODE: { LOG("STRUCT_VAR_NODE"); break; }

        case ASSIGN_VAR_NODE: { LOG("ASSIGN_VAR_NODE"); break; }
        case ASSIGN_APPEND_ARRAY_NODE: { LOG("ASSIGN_APPEND_ARRAY_NODE"); break; }
    
        case ERROR_NODE: { LOG("ERROR_NODE"); break; }
        case WARN_NODE: { LOG("WARN_NODE"); break; }
        case ASSERT_NODE: { LOG("ASSERT_NODE"); break; }

        case CODE_NODE: {LOG("CODE_NODE"); break; }

        case ALIGN_NODE: {LOG("ALIGN_NODE"); break; }
        case ORG_NODE: {LOG("ORG_NODE"); break; }

        case INSTRUCTION_NODE: { LOG("INSTRUCTION_NODE"); break; }

        default: { LOG("INVALID_NODE]"); return; }
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
                case PERCENT:      { LOG("%%"); break; }
                case STAR:         { LOG("*"); break; }

                case TILDE:        { LOG("~"); break; }
                case EXCLAMATION:  { LOG("!"); break; }

                case DOUBLEVBAR:   { LOG("||"); break; }
                case DOUBLEAMPER:  { LOG("&&"); break; }

                case LSQB:         { LOG("idx"); break; }

                default: { LOG("X'\n"); return; }
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

        case LEN_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.len_expr.expr, level+1);
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

            if(ast->node.literal.value->type == VALUE_ARRAY) {
                struct ArrayElement* el = ast->node.literal.value->val.arr.head;

                while (el != NULL) {

                    dbg_ast_recur(el->this_expr, level+1);
                    LOG("\n");

                    if (el == ast->node.literal.value->val.arr.tail)
                        break;

                    el = el->next;
                }
            }
            break;
 
        case VAR_NODE:
            for(unint i=0; i<ast->node.var->len; ++i) LOG_CP(ast->node.var->cps[i]);
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

            dbg_ast_recur(ast->node._if.cond, level+2);

            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[STATEMENTS]\n");

            struct Ast_node stmts;
            stmts.type = STATEMENTS_NODE;
            stmts.node.statements = ast->node._if.statements;
            dbg_ast_recur(&stmts, level+2);

            for(struct ElifCondBlock* _elif = ast->node._if._elifs_start; _elif; _elif = _elif->next) {
                for(unint i=0; i<level; ++i) LOG("\t");
                LOG("[ELIF]");

                LOG("\n");
                for(unint i=0; i<level+1; ++i) LOG("\t");
                LOG("[CONDITION]\n");

                dbg_ast_recur(_elif->cond, level+2);

                for(unint i=0; i<level+1; ++i) LOG("\t");
                LOG("[STATEMENTS]\n");

                stmts.type = STATEMENTS_NODE;
                stmts.node.statements = _elif->statements;
                dbg_ast_recur(&stmts, level+2);
            }

            if(ast->node._if._else == NULL) break;

            for(unint i=0; i<level; ++i) LOG("\t");
            LOG("[ELSE]\n");

            stmts.type = STATEMENTS_NODE;
            stmts.node.statements = *ast->node._if._else;
            dbg_ast_recur(&stmts, level+1);       

            break;

        case WHILE_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[CONDITION]\n");

            dbg_ast_recur(ast->node._while.cond, level+2);

            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[STATEMENTS]\n");

            stmts.type = STATEMENTS_NODE;
            stmts.node.statements = ast->node._while.statements;
            dbg_ast_recur(&stmts, level+2);
            break;

        case REPEAT_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[ITERATOR]\n");

            dbg_ast_recur(ast->node._repeat.expr, level+2);

            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[STATEMENTS]\n");

            stmts.type = STATEMENTS_NODE;
            stmts.node.statements = ast->node._repeat.statements;
            dbg_ast_recur(&stmts, level+2);
            break;

        case ERROR_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.error.error, level+1);
            break;
        case WARN_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.warn.warn, level+1);
            break;
        case ASSERT_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.assert.assert, level+1);
            break;
        
        case ASSIGN_VAR_NODE:
        case ASSIGN_APPEND_ARRAY_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[VARIABLE] ");
            for(unint i=0; i<ast->node.var_assign.name->len; ++i) LOG_CP(ast->node.var_assign.name->cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[IDX]\n");
            if(ast->node.var_assign.idx) dbg_ast_recur(ast->node.var_assign.idx, level+2);
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[=]\n");
            dbg_ast_recur(ast->node.var_assign.expr, level+2);
            break;

        case BYTE_NODE:
        case WORD_NODE:
        case DWORD_NODE:
        case QWORD_NODE:
        case FLOAT_NODE:
        case DOUBLE_NODE:
        case PTR_NODE:

        case SAVEB_NODE:
        case SAVEW_NODE:
        case SAVEDW_NODE:
        case SAVEQ_NODE:
        case SAVEF_NODE:
        case SAVED_NODE:
        case SAVEP_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[LEN] ");
            if(ast->node.space.len_expr == NULL) LOG("<NULL>");
            else {
                LOG("\n");
                dbg_ast_recur(ast->node.space.len_expr, level+2);
            }
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[ALIGNMENT PER ELEMENT] ");
            if(ast->node.space.align_per_el_expr == NULL) LOG("<NULL>");
            else {
                LOG("\n");
                dbg_ast_recur(ast->node.space.align_per_el_expr, level+2);
            }
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[ALIGNMENT START] ");
            if(ast->node.space.align_start_expr == NULL) LOG("<NULL>");
            else {
                LOG("\n");
                dbg_ast_recur(ast->node.space.align_start_expr, level+2);
            }
            
            if(!IS_SAVE_TYPE(ast->type)) {
                /* if(ast->node.space.name != NULL) {
                    LOG("\n");
                    for(unint i=0; i<level+1; ++i) LOG("\t");
                    LOG("[NAME] ");
                    for(unint i=0; i<ast->node.space.name->len; ++i) LOG_CP(ast->node.space.name->cps[i]);
                    LOG("\n");
                } else */{
                    LOG("\n");
                    for(unint i=0; i<level+1; ++i) LOG("\t");
                    LOG("[VALUE]\n");
                    dbg_ast_recur(ast->node.space.value, level+2);

                }
            }
            LOG("\n");
            break;

        case LABEL_NODE:
            for(unint i=0; i<ast->node.label.name->len; ++i) LOG_CP(ast->node.label.name->cps[i]);
            LOG("\n");
            break;

        
        case STRUCT_DECL_NODE:
            for(unint i=0; i<ast->node.struct_decl.struct_name->len; ++i) LOG_CP(ast->node.struct_decl.struct_name->cps[i]);
            LOG("\n");
            for(struct StructDeclField* field = ast->node.struct_decl.head; field; field = field->next) {
                for(unint i=0; i<level+1; ++i) LOG("\t");
                LOG("[FIELD] ");
                for(unint i=0; i<field->name->len; ++i) LOG_CP(field->name->cps[i]);
                LOG("\n");
                for(unint i=0; i<level+2; ++i) LOG("\t");
                LOG("[TYPE] ");
                switch(field->type) {
                    case BYTE_NODE: { LOG("BYTE_NODE"); break; }
                    case WORD_NODE: { LOG("WORD_NODE"); break; }
                    case DWORD_NODE : { LOG("DWORD_NODE"); break; }
                    case QWORD_NODE : { LOG("QWORD_NODE"); break; }
                    case FLOAT_NODE : { LOG("FLOAT_NODE"); break; }
                    case DOUBLE_NODE : { LOG("DOUBLE_NODE"); break; }
                    case PTR_NODE: { LOG("PTR_NODE"); break; }
                    case STRUCT_DECL_NODE: { 
                        LOG("STRUCT_DECL_NODE "); 
                        for(unint i=0; i<field->struct_name->len; ++i) LOG_CP(field->struct_name->cps[i]);
                        break;
                    }
                }
                LOG("\n");
                for(unint i=0; i<level+2; ++i) LOG("\t");
                LOG("[LEN] ");
                if(field->len_expr == NULL) LOG("<NULL>");
                else {
                    LOG("\n");
                    dbg_ast_recur(field->len_expr, level+3);
                }
                LOG("\n");
                for(unint i=0; i<level+2; ++i) LOG("\t");
                LOG("[ALIGNMENT PER ELEMENT] ");
                if(field->align_per_el_expr == NULL) LOG("<NULL>");
                else {
                    LOG("\n");
                    dbg_ast_recur(field->align_per_el_expr, level+3);
                }
                LOG("\n");
                for(unint i=0; i<level+2; ++i) LOG("\t");
                LOG("[ALIGNMENT START] ");
                if(field->align_start_expr == NULL) LOG("<NULL>");
                else {
                    LOG("\n");
                    dbg_ast_recur(field->align_start_expr, level+3);
                }
                LOG("\n");
            }
            break;

        case STRING_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.string.expr, level+1);
            break;

        case DEL_NODE:
            for(unint i=0; i<ast->node.del.ident->len; ++i) LOG_CP(ast->node.del.ident->cps[i]);
            LOG("\n");
            break;

        /*
        case RETURN_NODE:
            if(ast->node._return.ident == NULL) LOG("<NULL>");
            else for(unint i=0; i<ast->node._return.ident->len; ++i) LOG_CP(ast->node._return.ident->cps[i]);
            LOG("\n");
            break;
        */
        case STRUCT_VAR_NODE:
            for(unint i=0; i<ast->node.struct_var.struct_name->len; ++i) LOG_CP(ast->node.struct_var.struct_name->cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[NAME] ");
            for(unint i=0; i<ast->node.struct_var.var_name->len; ++i) LOG_CP(ast->node.struct_var.var_name->cps[i]);
            LOG("\n");
            dbg_struct_var_fields(ast->node.struct_var.head, level+1);
            break;

        /*
        case FUN_NODE:
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[CALLING CONV] ");
            for(unint i=0; i<ast->node.fun_decl.calling_conv->len; ++i) LOG_CP(ast->node.fun_decl.calling_conv->cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[NAME] ");
            for(unint i=0; i<ast->node.fun_decl.func_name->len; ++i) LOG_CP(ast->node.fun_decl.func_name->cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[ARGS]\n");
            for(struct FuncDeclArg* arg = ast->node.fun_decl.head; arg; arg = arg->next) {
                for(unint i=0; i<level+2; ++i) LOG("\t");
                    LOG("[");
                    switch(arg->type) {
                        case BYTE_NODE: { LOG("BYTE_NODE"); break; }
                        case WORD_NODE: { LOG("WORD_NODE"); break; }
                        case DWORD_NODE : { LOG("DWORD_NODE"); break; }
                        case QWORD_NODE : { LOG("QWORD_NODE"); break; }
                        case FLOAT_NODE : { LOG("FLOAT_NODE"); break; }
                        case DOUBLE_NODE : { LOG("DOUBLE_NODE"); break; }
                        case PTR_NODE: { LOG("PTR_NODE"); break; }
                    }
                    LOG("] ");
                    for(unint i=0; i<arg->arg_name->len; ++i) LOG_CP(arg->arg_name->cps[i]);
                    LOG("\n");
            }
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[RETURN] ");
            switch(ast->node.fun_decl.return_type) {
                case BYTE_NODE: { LOG("BYTE_NODE"); break; }
                case WORD_NODE: { LOG("WORD_NODE"); break; }
                case DWORD_NODE : { LOG("DWORD_NODE"); break; }
                case QWORD_NODE : { LOG("QWORD_NODE"); break; }
                case FLOAT_NODE : { LOG("FLOAT_NODE"); break; }
                case DOUBLE_NODE : { LOG("DOUBLE_NODE"); break; }
                case PTR_NODE: { LOG("PTR_NODE"); break; }
                case VOID_RETURN_TYPE: { LOG("VOID"); break; }
            }
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[STATEMENTS]\n");

            stmts.type = STATEMENTS_NODE;
            stmts.node.statements = ast->node.fun_decl.statements;
            dbg_ast_recur(&stmts, level+2);
            break;
        
        case FUNC_CALL_NODE:
            for(unint i=0; i<ast->node.func_call.func_call->len; ++i) LOG_CP(ast->node.func_call.func_call->cps[i]);
            LOG("\n");
            for(struct FuncCallArg* arg = ast->node.func_call.args_head; arg; arg = arg->next) {
                for(unint i=0; i<level+1; ++i) LOG("\t");
                LOG("[ARGS]\n");
                dbg_ast_recur(arg->arg_expr, level+2);
            }
            LOG("\n");
            break;
        */
        case INSTRUCTION_NODE:
            for(unint i=0; i<ast->node.instruction.name->len; ++i) LOG_CP(ast->node.instruction.name->cps[i]);
            LOG("\n");
            for(unint i=0; i<level+1; ++i) LOG("\t");
            LOG("[ARGS]\n");
            for(struct InstructionArg* arg = ast->node.instruction.args_head; arg; arg = arg->next) {
                if(!arg->_s) {
                    for(unint i=0; i<level+2; ++i) LOG("\t");
                    LOG("<EMPTY>");
                } else {
                    struct token* _t = arg->_s;
                    while(true) {
                        for(unint i=0; i<level+2; ++i) LOG("\t");
                        LOG("[%s:l%d:c%d]\n", _Parser_TokenNames[_t->type], _t->lineno, _t->col_offset);
                        if(_t == arg->_e) break;
                        _t = _t->next;
                    }
                }
                LOG("\n");
            }
            LOG("\n");
            break;

        case CODE_NODE:
            for(unsigned char* i=(unsigned char*)ast->node.code.lang->code_name; *i; ++i) LOG_CP((int32_t)*i);
            LOG("\n");
            break;
        
        case ALIGN_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.align.expr, level+1);
            break;

        case ORG_NODE:
            LOG("\n");
            dbg_ast_recur(ast->node.align.expr, level+1);
            break;
        default: { LOG("INVALID_NODE"); break; }
    }
}

void dbg_ast(struct Ast_node* ast) {
    if(ast == NULL) { LOG("AST is <NULL>\n"); return; }
    dbg_ast_recur(ast, 0);
}

