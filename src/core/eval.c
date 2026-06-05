#include <stdarg.h>
#include <math.h>

#include "api/memory.h"
#include "api/debug.h"
#include "api/unicode.h"
#include "api/file.h"

#include "asm_lang.h"
#include "eval.h"
#include "token.h"

unint _compare_identifiers(int32_t* cps1, unint len1, int32_t* cps2, unint len2);
void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...);
void _error_from_multiple_tokens(struct Parser* p, struct token* _s, struct token* _e, const char *stype, const char *format);

// Variables

struct Variable* new_variable(struct Parser* p, struct token* _token, struct Value* val) {
    struct Variable* var = (struct Variable*)MEM_ALLOC(sizeof(struct Variable), "new variable");
    if(var == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_MEMORY, "no available memory for variable");
        return NULL;
    }

    // Fill
    var->var_name = _token;
    var->next = NULL;

    // Copy
    var->val = *val;

    // Link
    var->prev = p->variables_tail;

    if (p->variables_tail) p->variables_tail->next = var;
    else p->variables = var;

    p->variables_tail = var;

    return var;
}

struct Variable* get_variable(struct Parser* p, struct token* _token) {
    if(p == NULL || p->variables == NULL) return NULL;

    for(struct Variable* var = p->variables; var != NULL; var = var->next) {
        if(var->var_name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(var->var_name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return var;
    }
    return NULL;
}

unint is_variable_declared(struct Parser* p, struct token* _token) {
    return (get_variable(p, _token) == NULL) ? FAIL : SUCCESS;
}

// Struct declarations

struct StructsDecl* new_struct_decl(struct Parser* p, struct AstStructDecl* struct_decl) {
    struct StructsDecl* var = (struct StructsDecl*)MEM_ALLOC(sizeof(struct StructsDecl), "eval: new struct decl");
    if(var == NULL) {
        _error_from_token(p, struct_decl->struct_name, ERROR_TYPE_MEMORY, "no available memory for struct declaration");
        return NULL;
    }

    var->ast_struct_decl = struct_decl;
    var->next = NULL;

    // First Variable
    if(p->struct_decl == NULL) p->struct_decl = var;

    // Link the new variable
    if(p->struct_decl_tail != NULL) p->struct_decl_tail->next = var;
    p->struct_decl_tail = var;

    return var;

}

struct StructsDecl* get_struct_decl(struct Parser* p, struct token* _token) {
    if(p == NULL || p->struct_decl == NULL) return NULL;

    for(struct StructsDecl* var = p->struct_decl; var != NULL; var = var->next) {
        if(var->ast_struct_decl->struct_name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(var->ast_struct_decl->struct_name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return var;
    }
    return NULL;   
}

unint is_struct_declared(struct Parser* p, struct token* _token) {
    return (get_struct_decl(p, _token) == NULL) ? FAIL : SUCCESS;
}

// Function declaration
/*
struct FuncDecl* new_func_decl(struct Parser* p, struct AstFuncDecl* ast_func_decl) {
    struct FuncDecl* var = (struct FuncDecl*)MEM_ALLOC(sizeof(struct FuncDecl), "eval: new func decl");
    if(var == NULL) {
        _error_from_token(p, ast_func_decl->func_name, ERROR_TYPE_MEMORY, "no available memory for function declaration");
        return NULL;
    }

    var->ast_func_decl = ast_func_decl;
    var->next = NULL;

    // First Variable
    if(p->func_decl == NULL) p->func_decl = var;

    // Link the new variable
    if(p->func_decl_tail != NULL) p->func_decl_tail->next = var;
    p->func_decl_tail = var;

    return var;

}

struct FuncDecl* get_func_decl(struct Parser* p, struct token* _token) {
    if(p == NULL || p->func_decl == NULL) return NULL;

    for(struct FuncDecl* var = p->func_decl; var != NULL; var = var->next) {
        if(var->ast_func_decl->func_name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(var->ast_func_decl->func_name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return var;
    }
    return NULL;   
}

unint is_func_declared(struct Parser* p, struct token* _token) {
    return (get_func_decl(p, _token) == NULL) ? FAIL : SUCCESS;
}
*/

// Label declaration

struct LabelDecl* new_label_decl(struct Parser* p, struct token* name, struct LabelDecl** append_head, struct LabelDecl** append_tail, nint addr) {
    struct LabelDecl* label = (struct LabelDecl*)MEM_ALLOC(sizeof(struct LabelDecl), "eval: new label decl");
    if(label == NULL) {
        _error_from_token(p, name, ERROR_TYPE_MEMORY, "no available memory for label declaration");
        return NULL;
    }

    // label->label = ast_label_decl;
    label->addr = addr;
    label->name = name;
    label->next = NULL;
    label->deep_head = NULL;
    label->deep_tail = NULL;
    label->len = 0;
    label->stride = 0;

    // First Variable
    if(*append_head == NULL) *append_head = label;

    // Link the new label
    if(*append_tail != NULL) (*append_tail)->next = label;
    *append_tail = label;

    return label;

}

struct LabelDecl* get_label_decl(struct Parser* p, struct token* _token) {
    for(struct LabelDecl* label = p->global_label_decl; label != NULL; label = label->next) {
        if(label->deep_head != NULL || label->deep_tail != NULL) continue;
        if(label->name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(label->name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return label;
    }

    return NULL;   
}

unint is_label_declared(struct Parser* p, struct token* _token/* , unint is_inside_func*/) {
    return (get_label_decl(p, _token/*, is_inside_func */) == NULL) ? FAIL : SUCCESS;
}

void print_labels(struct LabelDecl* head, struct LabelDecl* tail, unint level) {
    struct LabelDecl* label = head;

    LOG("[\n");
    while(label != NULL) {

        for(unint i=0; i<level+1; ++i) LOG("\t");
        for(unint i=0; i<label->name->len; ++i) LOG_CP(label->name->cps[i]);

        LOG(" = ");
        LOG("0x%x ", label->addr);
    
        // struct field
        if(label->len != 0) {
            LOG("(len = %d, stride = %d) ", label->len, label->stride);
        }

        if(label->deep_head && label->deep_tail) print_labels(label->deep_head, label->deep_tail, level+1);
        else LOG("\n");

        if(label == tail) break;
        label = label->next;
    }
    for(unint i=0; i<level; ++i) LOG("\t");
    LOG("]\n");
}

// Struct variable

struct LabelDecl* get_struct_var(struct Parser* p, struct token* _token) {
    for(struct LabelDecl* label = p->global_label_decl; label != NULL; label = label->next) {
        if(label->deep_head == NULL || label->deep_tail == NULL) continue;
        if(label->name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(label->name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return label;
    }

    return NULL;    
}

unint is_struct_var(struct Parser* p, struct token* _token) {
    return (get_struct_var(p, _token) == NULL) ? FAIL : SUCCESS;
}

// Expression evaluation

unint _equality_check(struct Value* vleft, unint op, struct Value* vright) {
    // except for int and floats, if both types dont match, return false(==), or true(!=)
    if((vleft->type == VALUE_DOUBLE || vleft->type == VALUE_INT) &&
        (vright->type == VALUE_DOUBLE || vright->type == VALUE_INT)
    ) {
        // if one is float, promote all to float
        if(vleft->type == VALUE_DOUBLE || vright->type == VALUE_DOUBLE) {

            double da = (vleft->type == VALUE_DOUBLE) ? vleft->val.flt : (double)vleft->val.number;
            double db = (vright->type == VALUE_DOUBLE) ? vright->val.flt : (double)vright->val.number;

            return (op == EQEQUAL) ? (nint)(da == db) : (nint)(da != db);;
        }

        nint a = vleft->val.number;
        nint b = vright->val.number;

        return (op == EQEQUAL) ? (nint)(a == b) : (nint)(a != b);

    }

    // Do types miss-match?
    if(vleft->type != vright->type) return !(op == EQEQUAL);

    // types left: string == string and array == array
    unint alen;
    unint blen;

    if(vleft->type == VALUE_STR) {
        alen = vleft->val.string.len;
        blen = vright->val.string.len;

        if(alen != blen) return !(op == EQEQUAL);

        for(unint i=0; i<alen; ++i) if(vleft->val.string.str[i] != vright->val.string.str[i]) return !(op == EQEQUAL);

        // Strings match
        return (op == EQEQUAL);
    }

    // Array:
    alen = vleft->val.arr.len;
    blen = vright->val.arr.len;

    if(alen != blen) return !(op == EQEQUAL);

    struct ArrayElement* tleft = vleft->val.arr.head;
    struct ArrayElement* tright = vright->val.arr.head;
    while(tleft != NULL) {
        if(_equality_check(tleft->this_expr->node.literal.value, NOTEQUAL, tright->this_expr->node.literal.value)) return !(op == EQEQUAL);
        tleft = tleft->next;
        tright = tright->next;
    }

    // Arrays match
    return (op == EQEQUAL);
}

unint _type_check(struct Parser* p, struct AstBinOp* binop, struct Value* vleft, struct Value* vright, unint tcleft, unint tcright) {
    struct token* op_token = binop->op_token;
    unint op = binop->op;

    // Type checks
    // Unary only on numbers
    DBG(DO_PARSER_RADOM_STUFF_DBG, "tcright = %d | tcleft = %d\n", tcright, tcleft);
    if((op == PLUS || op == MINUS) && binop->right == NULL && (
        !tcright && (tcleft && (vleft->type != VALUE_DOUBLE && vleft->type != VALUE_INT))
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "invalid unary operator");
        return FAIL;
    }

    // Concatnation only allowed on Strings/characters
    if(op == PLUS && (tcleft == 1 && tcright == 1) && ( 
        (vleft->type == VALUE_STR) != (vright->type == VALUE_STR)
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "can only perform concatnation on strings types");
        return FAIL;
    }


    // Arithmetic only on ints/floats 
    if ( op == PLUS || op == MINUS || op == SLASH || op == STAR || op == PERCENT ) {

        unint left_numeric =
            !tcleft ||
            vleft->type == VALUE_INT ||
            vleft->type == VALUE_DOUBLE;

        unint right_numeric =
            !tcright ||
            vright->type == VALUE_INT ||
            vright->type == VALUE_DOUBLE;

        unint string_concat =
            op == PLUS &&
            tcleft && tcright &&
            vleft->type == VALUE_STR &&
            vright->type == VALUE_STR;

        if (!(left_numeric && right_numeric) && !string_concat) {
            _error_from_token( p, op_token, ERROR_TYPE_EXPRESSION, "can only perform arithmetic operations on integer/decimal types" );
            return FAIL;
        }
    }
    // Bitwise operators only on ints
    if((op == LEFTSHIFT || op == RIGHTSHIFT || op == AMPER || op == VBAR || op == CIRCUMFLEX || op == TILDE) && (
        (tcleft && vleft->type != VALUE_INT) ||
        (tcright && vright->type != VALUE_INT)
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "can only perform bitwise operations on integer types");
        return FAIL;
    }

    // Relational operators

    // >= > <= < only to integers/floats
    if((op == GREATER || op == GREATEREQUAL || op == LESS || op == LESSEQUAL) && (
        (tcleft && vleft->type != VALUE_INT && vleft->type != VALUE_DOUBLE) ||
        (tcright && vright->type != VALUE_INT && vright->type != VALUE_DOUBLE)
    )) {
        _error_from_token(p, op_token, ERROR_TYPE_TYPE, "can only perform comparison on integer/double types");
        return FAIL;
    }
    
    // Indexation only on array types or strings
    if(op == LSQB && 
        (tcleft && vleft->type != VALUE_ARRAY && vleft->type != VALUE_STR)
    ) {
        // _error_from_multiple_tokens(p, binop->_s, binop->_e, ERROR_TYPE_TYPE, "can only perform indexation on array/string types");
        _error_from_token(p, op_token, ERROR_TYPE_TYPE, "can only perform indexation on array/string types %d", vleft->type);
        return FAIL;
    }

    // Index only with integer
    if(op == LSQB &&
        (tcright && vright->type != VALUE_INT)
    ) {
        _error_from_multiple_tokens(p, binop->_s, binop->_e, ERROR_TYPE_TYPE, "indices can only by of integer type");
        return FAIL;
    }
    
    return SUCCESS;
}

unint bool_eval(struct Value* val) {
    switch(val->type) {
        case VALUE_INT:
            return !!val->val.number;
        case VALUE_STR:
            return !!val->val.string.len;
        case VALUE_DOUBLE:
            return !!val->val.flt;
        case VALUE_ARRAY:
            return !!val->val.arr.len;
        default:
            return 0;
    }
}

unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val, struct Value** var_ref_idx) {
    switch(expr->type) {
        case LITERAL_NODE:
            if(expr->node.literal.value->type != VALUE_ARRAY) {
                *val = *expr->node.literal.value;
                return SUCCESS;
            }

            // Arrays
            struct ArrayElement* el = expr->node.literal.value->val.arr.head;
            while (el != NULL) {
                if (el->this_expr->type == LITERAL_NODE) goto _el_iter;

                DBG(1, "NEW ALLOC\n");
                // Freeze each element
                struct Value* el_val = (struct Value*)MEM_ALLOC(sizeof(struct Value), "array el value");
                if(el_val == NULL) {
                    memory_error(p, "no available memory for the array value");
                    return FAIL;
                }

                if(_eval_expr(p, el->this_expr, el_val, var_ref_idx) == FAIL) return FAIL;
                el->this_expr->type = LITERAL_NODE;
                el->this_expr->node.literal.value = el_val;

_el_iter:

                if (el == expr->node.literal.value->val.arr.tail) break;

                el = el->next;
            }

            *val = *expr->node.literal.value;
            return SUCCESS;
        case LEN_NODE:
            struct Value len_val;
            struct Value* len_val_ref_idx;
            if(_eval_expr(p, expr->node.len_expr.expr, &len_val, &len_val_ref_idx) == FAIL) return FAIL;

            if(len_val.type == VALUE_ARRAY || len_val.type == VALUE_STR) {

                unint u_len = (len_val.type == VALUE_ARRAY) ? 
                    len_val.val.arr.len : 
                    len_val.val.string.len;

                if (u_len > NINT_MAX) {
                    _error_from_multiple_tokens(p, expr->node.len_expr._s, expr->node.len_expr._e, ERROR_TYPE_OVERFLOW, "indice is too big to store in signed nint");
                    return FAIL;
                }
                val->type = VALUE_INT;
                val->val.number = (nint)u_len;
                return SUCCESS;
            } else {
                _error_from_multiple_tokens(p, expr->node.len_expr._s, expr->node.len_expr._e, ERROR_TYPE_TYPE, "invalid type for @len");
                return FAIL;
            }
        case VAR_NODE:
            // Check if variable is declared
            // Get the variable
            struct Variable* var = get_variable(p, expr->node.var);
            if(var == NULL) {
                _error_from_token(p, expr->node.var, ERROR_TYPE_EXPRESSION, "variable is not declared");
                return FAIL;
            }

            *val = var->val;
            return SUCCESS;
        case DOLLAR_NODE:
            val->type = VALUE_INT;
            val->val.number = p->addr;

            return SUCCESS;
        case BINOP_NODE:
            struct AstBinOp* binop = &expr->node.binop;
            struct Ast_node* pleft = binop->left;
            struct Ast_node* pright = binop->right;
            struct token* op_token = binop->op_token;
            
            struct Value vleft, vright;

            // && and || have manual evaluation
            if(binop->op == DOUBLEAMPER || binop->op == DOUBLEVBAR) goto _skip_extra_type_checks;

            // Early type check
            unint tcleft = 0, tcright = 0;

            if(pleft->type == LITERAL_NODE || pleft->type == VAR_NODE || pleft->type == LEN_NODE){
                if(_eval_expr(p, pleft, &vleft, var_ref_idx) == FAIL) return FAIL;
                tcleft = 1;
            }

            if(pright && (pright->type == LITERAL_NODE || pright->type == VAR_NODE || pright->type == LEN_NODE)) {
                if(_eval_expr(p, pright, &vright, var_ref_idx) == FAIL) return FAIL;
                tcright = 1;
            }

            DBG(DO_EXPRESSION_EVAL_TYPE_CHECK_DBG, "Type check #1: tcleft = %d | tcright = %d\n", tcleft, tcright);
            if(_type_check(p, binop, &vleft, &vright, tcleft, tcright) == FAIL) return FAIL;

            // Evaluate
            if(_eval_expr(p, pleft, &vleft, var_ref_idx) == FAIL) return FAIL;

            if(tcleft == 1 && tcright == 1) goto _skip_extra_type_checks;

            DBG(DO_EXPRESSION_EVAL_TYPE_CHECK_DBG, "Type check #2: tcleft = %d | tcright = %d\n", 1, 0);
            if(_type_check(p, binop, &vleft, &vright, 1, 0) == FAIL) return FAIL;

            // May not exist depending on the operator
            if(pright && _eval_expr(p, pright, &vright, var_ref_idx) == FAIL) return FAIL;

            DBG(DO_EXPRESSION_EVAL_TYPE_CHECK_DBG, "Type check #3: tcleft = %d | tcright = %d\n", 0, 1);
            if(pright && _type_check(p, binop, &vleft, &vright, 0, 1) == FAIL) return FAIL;

_skip_extra_type_checks:
            nint a, b;
            double da, db;

            // Eval operators
            switch(binop->op) {
                case PLUS:

                    // unary plus, if right is not present, preserve the original type
                    if(!pright) { *val = vleft; return SUCCESS; }

                    // String concatnation
                    if(vleft.type == VALUE_STR && vright.type == VALUE_STR) {
                        struct Value* non_empty_str = NULL;
                        unint alen = vleft.val.string.len;
                        unint blen = vright.val.string.len;
                        if(alen == 0) non_empty_str = &vright;
                        else if(blen == 0) non_empty_str = &vleft;
                        if(non_empty_str) {
                            *val = *non_empty_str;
                            return SUCCESS;
                        }

                        int32_t* cps = MEM_ALLOC((alen + blen) * sizeof(int32_t), "eval of concatnation");
                        if(cps == NULL) {
                            _error_from_token(p, op_token, ERROR_TYPE_MEMORY, "no available memory to concatnate the string");
                            return FAIL;   
                        }

                        // Copy
                        for(unint i=0; i<alen; ++i) cps[i] = vleft.val.string.str[i];
                        for(unint i=0; i<blen; ++i) cps[alen+i] = vright.val.string.str[i];

                        val->type = VALUE_STR;
                        val->val.string.str = cps;
                        val->val.string.len = (alen + blen);

                        return SUCCESS;
                    }

                    // Convert types
                    if(vleft.type == VALUE_INT && vright.type == VALUE_INT) {
                        val->type = VALUE_INT;
                        // Check for overflows
                        
                        a = vleft.val.number;
                        b = vright.val.number;

                        // Overflow?
                        if ((b > 0 && a > NINT_MAX - b) ||
                            (b < 0 && a < NINT_MIN - b)) {
                            _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in addition");
                            return FAIL;
                        }

                        val->type = VALUE_INT;
                        val->val.number = a + b;
                        return SUCCESS;
                    }

                    // Promote to flt
                    da = (vleft.type == VALUE_INT)  ? (double)vleft.val.number  : vleft.val.flt;
                    db = (vright.type == VALUE_INT) ? (double)vright.val.number : vright.val.flt;

                    val->type = VALUE_DOUBLE;
                    val->val.flt = da + db;
                    return SUCCESS;

                case MINUS:
                    if(vleft.type == VALUE_INT || (vleft.type == VALUE_INT && (expr->node.binop.right && vright.type == VALUE_INT))) {

                        a = expr->node.binop.right ? vleft.val.number : 0;
                        b = expr->node.binop.right ? vright.val.number : vleft.val.number;

                        if ((b < 0 && a > NINT_MAX + b) ||
                            (b > 0 && a < NINT_MIN + b)) {
                            _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in subtraction");
                            return FAIL;
                        }

                        val->type = VALUE_INT;
                        val->val.number = a - b;
                        return SUCCESS;
                    }

                    // Promote to flt
                    da = expr->node.binop.right ? (
                        (vleft.type == VALUE_INT) ?  (double)vleft.val.number : vleft.val.flt
                    ) : (
                        (vleft.type == VALUE_INT) ? 0 : (double)0.0
                    );
                    db = expr->node.binop.right ? (
                        (vright.type == VALUE_INT) ? (double)vright.val.number : vright.val.flt
                    ) : (
                        (vleft.type == VALUE_INT) ?  (double)vleft.val.number : vleft.val.flt
                    );
                    
                    val->type = VALUE_DOUBLE;
                    val->val.flt = da - db;
                    return SUCCESS;
                
                case PERCENT:
                    if( (vright.type == VALUE_DOUBLE && vright.val.flt == (double)0.0) || (vright.type == VALUE_INT && vright.val.number == 0) ) {
                        _error_from_token(p, op_token, ERROR_TYPE_DIVISION_ERROR, "modulo by 0");
                        return FAIL;
                    }
                    // if one is float, promote all to float
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {
                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.flt;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.flt;

                        val->type = VALUE_DOUBLE;
                        val->val.flt = fmod( fmod(da, db) + db, db);
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    val->type = VALUE_INT;
                    val->val.number = ((a % b) + b) % b;
                    return SUCCESS;
                    
                case SLASH:
                    if( (vright.type == VALUE_DOUBLE && vright.val.flt == (double)0.0) || (vright.type == VALUE_INT && vright.val.number == 0) ) {
                        _error_from_token(p, op_token, ERROR_TYPE_DIVISION_ERROR, "division by 0");
                        return FAIL;
                    }
                    // if one is float, promote all to float
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {

                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.number;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.number;

                        val->type = VALUE_DOUBLE;
                        val->val.flt = da / db;
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    if(a == NINT_MIN && b == -1) {
                        _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in division");
                        return FAIL;
                    }
                    val->type = VALUE_INT;
                    val->val.number = a / b;
                    return SUCCESS;

                case STAR:
                    // if one is float, promote all to float
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {

                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.number;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.number;

                        val->type = VALUE_DOUBLE;
                        val->val.flt = da * db;
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    if (a > 0) {
                        if (b > 0) {
                            if (a > NINT_MAX / b) goto mul_overflow;
                        } else {
                            if (b < NINT_MIN / a) goto mul_overflow;
                        }
                    } else {
                        if (b > 0) {
                            if (a < NINT_MIN / b) goto mul_overflow;
                        } else {
                            if (a != 0 && b < NINT_MAX / a) goto mul_overflow;
                        }
                    }

                    val->type = VALUE_INT;
                    val->val.number = a * b;
                    return SUCCESS;
mul_overflow:
                    _error_from_token(p, op_token, ERROR_TYPE_OVERFLOW, "integer overflow in multiplication");
                    return FAIL;

                case LSQB:

                    
                    // Valid index?
                    nint index = vright.val.number;
                    unint len = (vleft.type == VALUE_ARRAY) ? vleft.val.arr.len : vleft.val.string.len;
                    if(index < 0) index = len + index;

                    if(index < 0 || index >= len) {
                        _error_from_multiple_tokens(p, binop->_s, binop->_e, ERROR_TYPE_INDEX_ERROR, "index out of range");
                        return FAIL;
                    }

                    if(vleft.type == VALUE_ARRAY) {
                        struct ArrayElement* el = vleft.val.arr.head;
                        for(nint i=0; i<index; ++i) el = el->next;

                        // Value is assured to exitst
                        *val = *el->this_expr->node.literal.value;
                        *var_ref_idx = el->this_expr->node.literal.value;

                    } else { // String
                        int32_t* cp = MEM_ALLOC(sizeof(int32_t), "eval of string index");
                        if(cp == NULL) {
                            _error_from_token(p, op_token, ERROR_TYPE_MEMORY, "no available memory to index the string");
                            return FAIL;   
                        }

                        *cp = vleft.val.string.str[index];

                        val->type = VALUE_STR;
                        val->val.string.len = 1;
                        val->val.string.str = cp;
                    }

                    return SUCCESS;

                case LEFTSHIFT:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number << vright.val.number;
                    return SUCCESS;
                case RIGHTSHIFT:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number >> vright.val.number;
                    return SUCCESS;

                case AMPER:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number & vright.val.number;
                    return SUCCESS;

                case VBAR:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number | vright.val.number;
                    return SUCCESS;

                case CIRCUMFLEX:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number ^ vright.val.number;
                    return SUCCESS;

                case TILDE:
                    val->type = VALUE_INT;
                    val->val.number = ~vleft.val.number;
                    return SUCCESS;

                case EXCLAMATION:
                    val->type = VALUE_INT;
                    val->val.number = !bool_eval(&vleft);
                    return SUCCESS;
                
                case DOUBLEAMPER:
                    val->type = VALUE_INT;
                    // Eval the left branch and skip the right one if it fails
                    if(_eval_expr(p, pleft, &vleft, var_ref_idx) == FAIL) return FAIL;
                    if(bool_eval(&vleft) == 0) {
                        val->val.number = 0;
                        return SUCCESS;
                    }

                    // Eval the right branch
                    if(_eval_expr(p, pright, &vright, var_ref_idx) == FAIL) return FAIL;
                    val->val.number = bool_eval(&vright);
                    return SUCCESS;

                case DOUBLEVBAR:
                    val->type = VALUE_INT;

                    // Eval the left branch and skip the right one if it succeeds
                    if(_eval_expr(p, pleft, &vleft, var_ref_idx) == FAIL) return FAIL;
                    if(bool_eval(&vleft) == 1) {
                        val->val.number = 1;
                        return SUCCESS;
                    }

                    // Eval the right branch
                    if(_eval_expr(p, pright, &vright, var_ref_idx) == FAIL) return FAIL;
                    val->val.number = bool_eval(&vright);
                    return SUCCESS;

                case NOTEQUAL:
                case EQEQUAL:
                    val->type = VALUE_INT;
                    val->val.number = _equality_check(&vleft, binop->op, &vright);
                    return SUCCESS;

                case GREATER:
                case GREATEREQUAL:
                case LESS:
                case LESSEQUAL:
                    // if one is float, promote all to float
                    val->type = VALUE_INT;
                    if(vleft.type == VALUE_DOUBLE || vright.type == VALUE_DOUBLE) {

                        da = (vleft.type == VALUE_DOUBLE) ? vleft.val.flt : (double)vleft.val.number;
                        db = (vright.type == VALUE_DOUBLE) ? vright.val.flt : (double)vright.val.number;

                        switch(binop->op) {
                            case GREATER:      { val->val.number = da >  db; break; }
                            case GREATEREQUAL: { val->val.number = da >= db; break; }
                            case LESS:         { val->val.number = da <  db; break; }
                            case LESSEQUAL:    { val->val.number = da <= db; break; }
                        }
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    switch(binop->op) {
                        case GREATER:      { val->val.number = a >  b; break; }
                        case GREATEREQUAL: { val->val.number = a >= b; break; }
                        case LESS:         { val->val.number = a <  b; break; }
                        case LESSEQUAL:    { val->val.number = a <= b; break; }
                    }
                    return SUCCESS;

                    

                default:
                    _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "evaluation of that operator hasn't been implemented");
                    return FAIL;
            }
        default:
            return FAIL;  
    }
}

unint assign_type_without_equal(unint assign_type) {
    switch(assign_type) {
        case PLUSEQUAL: { return PLUS; }
        case MINEQUAL: { return MINUS; }
        case STAREQUAL: { return STAR; }
        case SLASHEQUAL: { return SLASH; }
        case PERCENTEQUAL: { return PERCENT; }
        case AMPEREQUAL: { return AMPER; }
        case VBAREQUAL: { return VBAR; }
        case CIRCUMFLEXEQUAL: { return CIRCUMFLEX; }
        case LEFTSHIFTEQUAL: { return LEFTSHIFT; }
        case RIGHTSHIFTEQUAL: { return RIGHTSHIFT; }
    }
    return ERRORTOKEN;
}

// Misc

unint align_address_safe(nint address, nint alignment, nint* out) {
    if (!out) return FAIL;
    if (alignment <= 0) return FAIL;

    nint rem = address % alignment;

    if (rem == 0) {
        *out = address;
        return SUCCESS;
    }

    nint m = alignment - rem;

    if (address > NINT_MAX - m) return FAIL;

    *out = address + m;
    return SUCCESS;
}

unint mul_nint_safe(nint a, nint b, nint* out) {
    if (!out) return FAIL;

    if (a == 0 || b == 0) {
        *out = 0;
        return SUCCESS;
    }

    if (a > NINT_MAX / b) {
        return FAIL;
    }

    *out = a * b;
    return SUCCESS;
}

nint safe_abs(nint x, nint *ok) {
    if (x == NINT_MIN) {
        *ok = 0;
        return 0;
    }

    *ok = 1;
    return (x < 0) ? -x : x;
}

// Euclidean GCD
nint gcd(nint a, nint b) {
    nint ok1, ok2;

    a = safe_abs(a, &ok1);
    b = safe_abs(b, &ok2);

    if (!ok1 || !ok2) return 0;

    while (b != 0) {
        nint temp = b;
        b = a % b;
        a = temp;
    }

    return a;
}


unint lcm(nint a, nint b, nint *result) {
    nint ok1, ok2;

    if (a == 0 || b == 0) {
        *result = 0;
        return SUCCESS;
    }

    nint abs_a = safe_abs(a, &ok1);
    nint abs_b = safe_abs(b, &ok2);

    if (!ok1 || !ok2) return FAIL;

    nint g = gcd(abs_a, abs_b);

    if (g == 0) return FAIL;

    nint reduced = abs_a / g;

    if (reduced > INT_MAX / abs_b) return FAIL;

    *result = reduced * abs_b;
    return SUCCESS;
}


/*
  base_address - current address where allocation begins
  n            - number of elements
  elem_size    - size of each element
  elem_align   - alignment of each element
  array_align  - alignment of array start
  out_size     - output variable
*/

unint array_total_size_safe(nint base_address, nint n, nint elem_size, nint elem_align, nint array_align, nint *out_size, nint *aligned_start, nint *stride)
{
    if (!out_size) return FAIL;

    nint effective_align;
    if (lcm(array_align, elem_align, &effective_align) == FAIL) return FAIL;

    if (align_address_safe(base_address, effective_align, aligned_start) == FAIL) return FAIL;
    
    nint front_padding = *aligned_start - base_address;

    if (align_address_safe(elem_size, elem_align, stride) == FAIL) return FAIL;

    nint body;

    if (mul_nint_safe(n, *stride, &body) == FAIL) return FAIL;

    if (front_padding > NINT_MAX - body) return FAIL;

    *out_size = front_padding + body - (*stride - elem_size);
    return SUCCESS;
}

#define IS_SAVE_TYPE(type) ((type) == SAVEB_NODE || (type) == SAVEW_NODE || (type) == SAVEDW_NODE || (type) == SAVEQ_NODE || (type) == SAVEF_NODE || (type) == SAVED_NODE || (type) == SAVEP_NODE)

void overflow(struct Parser* p, struct token* _token) {
    _error_from_token(p, _token, ERROR_TYPE_OVERFLOW, "overflowed address space: max=0x%x", p->active_lang->max_addr);
}

unint get_size_of_type(struct Parser* p, unint type) {
    switch(type) {
        case BYTE_NODE:
        case SAVEB_NODE: return 1;
        case WORD_NODE:
        case SAVEW_NODE: return 2;
        case DWORD_NODE:
        case SAVEDW_NODE: return 4;
        case QWORD_NODE:
        case SAVEQ_NODE: return 8;
        case FLOAT_NODE:
        case SAVEF_NODE: return sizeof(float);
        case DOUBLE_NODE:
        case SAVED_NODE: return sizeof(double);
        case PTR_NODE:
        case SAVEP_NODE: return p->active_lang->size_of_ptr;
    }
    return 0;
}

unint increment_addr(struct Parser* p, nint val) {
    if (p->addr > NINT_MAX - p->addr) return FAIL;
    else if(p->addr + val > p->active_lang->max_addr) return FAIL;
    p->addr += val;
    return SUCCESS;
}

unint ensure_lang(struct Parser* p, struct token* _token) {
    if( p->active_lang == NULL) {
        _error_from_token(p, _token, ERROR_TYPE_RUNTIME, "no language selected");
        return FAIL;
    }
    return SUCCESS;
}

unint encode_integer(struct Parser* p, nint value, unint type, unint le, uint8_t* out_buf) {
    if (!out_buf) return FAIL;

    unint width = get_size_of_type(p, type);

    unint bits = (unint)value;

    if (le) {
        for (nint i = 0; i < width; i++) out_buf[i] = (uint8_t)((bits >> (i * 8)) & 0xFF);

    } else {
        for (nint i = 0; i < width; i++) {
            nint shift = (width - 1 - i) * 8;
            out_buf[i] = (uint8_t)((bits >> shift) & 0xFF);
        }
    }

    return SUCCESS;
}

unint host_is_little_endian() {
    uint16_t x = 1;
    return *((uint8_t*)&x) == 1;
}

void encode_floats_doubles(void* buf, nint size, unint le) {
    uint8_t* bytes = (uint8_t*)buf;

    unint host_le = host_is_little_endian();

    unint need_swap = (host_le  && !le) || (!host_le && le);

    if (!need_swap) return;

    for (size_t i = 0; i < size / 2; i++) {
        uint8_t tmp = bytes[i];

        bytes[i] = bytes[size - 1 - i];

        bytes[size - 1 - i] = tmp;
    }
}

unint encode_space_ident(struct Parser* p, unint type, struct AstSpace* space, nint* total, nint* data_addr, nint* len, nint* stride, unint inhibit) {
    if(ensure_lang(p, space->space_ident) == FAIL) return FAIL;

    nint el_size = (nint)get_size_of_type(p, type);

    nint el_align = (space->align_per_el_expr == NULL ? 1 : space->align_per_el_expr->node.literal.value->val.number);
    nint st_align = (space->align_start_expr  == NULL ? 1 : space->align_start_expr->node.literal.value->val.number);
    nint el_num =   (space->len_expr          == NULL ? 1 : space->len_expr->node.literal.value->val.number);
    *len = el_num;

    nint aligned_start;

    if(array_total_size_safe(p->addr, el_num, el_size, el_align, st_align, total, &aligned_start, stride) == FAIL) {
        overflow(p, space->space_ident);
        return FAIL;
    }

    *data_addr = aligned_start;

    struct Value val;
    struct Value* var_ref_idx;

    nint array_vals;
    if(!IS_SAVE_TYPE(type)) {
        if(_eval_expr(p, space->value, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;

        // Errors
        if(
            ((type == BYTE_NODE  || type == WORD_NODE || type == DWORD_NODE || type == QWORD_NODE) && (val.type != VALUE_INT && val.type != VALUE_ARRAY)) ||
            ((type == FLOAT_NODE || type == DOUBLE_NODE) && (val.type != VALUE_DOUBLE && val.type != VALUE_ARRAY)) ||
            ((type == PTR_NODE) && (val.type != VALUE_INT && val.type != VALUE_ARRAY)) 
        ) goto _invalid_type;       

        if((array_vals = (val.type == VALUE_ARRAY))) {
            if(val.val.arr.len != el_num) {
                _error_from_multiple_tokens(p, space->_s, space->_e, ERROR_TYPE_RUNTIME, "lengths dont match");
                return FAIL;
            } else {
                // type check
                struct ArrayElement* el = val.val.arr.head;
                while(el != NULL) {
                    unint el_type = el->this_expr->node.literal.value->type;
                    if(
                        ((type == BYTE_NODE  || type == WORD_NODE || type == DWORD_NODE || type == QWORD_NODE) && (el_type != VALUE_INT)) ||
                        ((type == FLOAT_NODE || type == DOUBLE_NODE) && (el_type != VALUE_DOUBLE)) ||
                        ((type == PTR_NODE) && (el_type != VALUE_INT)) 
                    ) goto _invalid_type;       

                    if(el == val.val.arr.tail) break;
                    el = el->next;
                }
            }
        }
    } else {
        val.type = VALUE_INT;
        val.val.number = 0x00;
        array_vals = 0;
    }

    nint _addr = p->addr;
    if(increment_addr(p, *total) == FAIL) {
        overflow(p, space->space_ident);
        return FAIL;        
    }
    // Last pass

    if(inhibit || !p->last_pass) return SUCCESS;

    DBG(1, "_addr = %d | aligned_start = %d\n", _addr, aligned_start);
    for(nint i=_addr; i<aligned_start; ++i) {
        OUT_FILE_WRITE_BYTE((unsigned char)0);
        DBG(1, "al - 00\n");
    }
    DBG(1, "\n");

    void* dst = MEM_ALLOC(el_size, "space ident buf");
    if(dst == NULL) {
        _error_from_multiple_tokens(p, space->_s, space->_e, ERROR_TYPE_MEMORY, "no available memory for the buffer");
        return EVAL_ERROR;                        
    }

    struct ArrayElement* curr;
    struct Value* el_val;
    if(array_vals) {
        curr = val.val.arr.head;
        el_val = curr->this_expr->node.literal.value;
    } else el_val = &val;

    for(nint n=0; n<el_num; ++n) {
        
        if(el_val->type == VALUE_DOUBLE) {
            float flt;
            void* data;
            if(type == FLOAT_NODE || type == SAVEF_NODE) {
                flt = (float)el_val->val.flt;
                data = &flt;
            } else data = &el_val->val.flt;

            MEM_CPY(dst, data, el_size);
            encode_floats_doubles(dst, el_size, p->active_lang->le);
        } else {
            if(encode_integer(p, el_val->val.number, type, p->active_lang->le, dst) == FAIL) return FAIL;
        }

        for(nint i=0; i<el_size; ++i) {
            OUT_FILE_WRITE_BYTE(((unsigned char*)dst)[i]);
            DBG(1, "%02X\n", ((unsigned char*)dst)[i]);
        }
        for(nint j=0; j<(n != (el_num - 1) ? (*stride - el_size) : 0); ++j){
            OUT_FILE_WRITE_BYTE((unsigned char)0);
            DBG(1, "st - 00\n");
        }
    
        if(array_vals) { curr = curr->next; if(curr) el_val = curr->this_expr->node.literal.value; }
    }

    return SUCCESS;
_invalid_type:
    _error_from_multiple_tokens(p, space->_s, space->_e, ERROR_TYPE_TYPE, "invalid type");
    return FAIL;
}

// Struct vars
unint encode_struct(struct Parser* p, struct token* var_name, struct AstStructDecl* struct_decl, struct StructAssignField* var_head, struct StructAssignField* var_tail, nint* total, unint create_labels, struct LabelDecl** labels_head, struct LabelDecl** labels_tail) {    
    // Check for unknown fields
    struct StructAssignField* _f = var_head;
    while(_f != NULL) {
        unint not_found = 1;
        // search for a matching ident on the decl
        struct StructDeclField* _df = struct_decl->head;
        while(_df != NULL) {

            if(_compare_identifiers(_f->field_name->cps, _f->field_name->len, _df->name->cps, _df->name->len) == SUCCESS) {
                not_found = 0;
                break;
            }

            if(_df == struct_decl->tail) break;
            _df = _df->next;
        }

        if(not_found) {
            _error_from_token(p, _f->field_name, ERROR_TYPE_NAME, "unknown field");
            return FAIL;
        }

        if(_f == var_tail) break;
        _f = _f->next;

    }

    *total = 0;

    struct StructDeclField* decl_field = struct_decl->head;
    while(decl_field != NULL) {
        // Add a label for each declared field
        struct LabelDecl* label;
        if(create_labels && !p->last_pass) {
            label = new_label_decl(p, var_name, labels_head, labels_tail, 0);
            if(label == NULL) return FAIL;
            label->name = decl_field->name;
        }


        if(decl_field->type != STRUCT_DECL_NODE) {
            struct AstSpace ast_space;
            ast_space.align_start_expr = decl_field->align_start_expr;
            ast_space.len_expr = decl_field->len_expr;
            ast_space.align_per_el_expr = decl_field->align_per_el_expr;
            
            // Find the coresponding var field
            // Default
            struct Ast_node _default;
            _default.type = LITERAL_NODE;

            struct Value _default_value;
            if(decl_field->type == DOUBLE_NODE || decl_field->type == FLOAT_NODE) {
                _default_value.type = VALUE_DOUBLE;
                _default_value.val.flt = 0.0;
            } else {
                _default_value.type = VALUE_INT;
                _default_value.val.number = 0;
            }

            _default.node.literal.value = &_default_value;

            // Since the defalut is 0 and there are no problems with the value 0 they can be set to null
            ast_space._s = ast_space._e = NULL; 
            ast_space.value = &_default;
            
            ast_space.space_ident = var_name;

            struct StructAssignField* ass_field = var_head;
            while(ass_field != NULL) {

                if(_compare_identifiers(decl_field->name->cps, decl_field->name->len, ass_field->field_name->cps, ass_field->field_name->len) == SUCCESS) {
                    ast_space.value = ass_field->value;
                    ast_space._s = ass_field->_s;
                    ast_space._e = ass_field->_e;
                    ast_space.space_ident = ass_field->field_name;
                    break;
                }

                if(ass_field == var_tail) break;
                ass_field = ass_field->next;
            }

            nint _total, _len, _data_addr, _stirde;
            if(encode_space_ident(p, decl_field->type, &ast_space, &_total, &_data_addr, &_len, &_stirde, create_labels) == FAIL) return FAIL;
            
            if(create_labels && !p->last_pass) {
                label->addr = _data_addr;
                label->len = _len;
                label->stride = _stirde;
            }


            *total += _total;
        } else {
            struct StructsDecl* _struct_decl = get_struct_decl(p, decl_field->struct_name);

            // Find the struct var
            struct StructAssignField* ass_field = var_head;
            struct token* error_token = var_name;
            struct StructAssignField* _head = NULL;
            struct StructAssignField* _tail = NULL;
            while(ass_field != NULL) {

                if(_compare_identifiers(decl_field->name->cps, decl_field->name->len, ass_field->field_name->cps, ass_field->field_name->len) == SUCCESS) {
                    
                    if(ass_field->head == NULL || ass_field->tail == NULL) {
                        _error_from_multiple_tokens(p, ass_field->_s, ass_field->_e, ERROR_TYPE_TYPE, "invalid type");
                        return FAIL;
                    }
                    error_token = ass_field->field_name;
                    _head = ass_field->head;
                    _tail = ass_field->tail;
                    break;
                }

                if(ass_field == var_tail) break;
                ass_field = ass_field->next;
            }

            nint el_align = (decl_field->align_per_el_expr == NULL ? 1 : decl_field->align_per_el_expr->node.literal.value->val.number);
            nint st_align = (decl_field->align_start_expr  == NULL ? 1 : decl_field->align_start_expr->node.literal.value->val.number);
            nint el_num =   (decl_field->len_expr          == NULL ? 1 : decl_field->len_expr->node.literal.value->val.number);

            nint size = 0;
            unint success = 0;
            nint struct_aligned_start, stride, out_size;
            nint _addr = p->addr;

            while(true) {
                nint struct_effective_align;
                if (lcm(st_align, el_align, &struct_effective_align) == FAIL) break;

                if (align_address_safe(p->addr, struct_effective_align, &struct_aligned_start) == FAIL) break;

                nint front_padding = struct_aligned_start - p->addr;

                // Compute element size, inhibit label creation
                if(encode_struct(p, var_name, _struct_decl->ast_struct_decl, _head, _tail, &size, 1, &label->deep_head, &label->deep_tail) == FAIL) return FAIL;
                // At least one element fits
                p->addr = _addr; // go back

                if (align_address_safe(size, el_align, &stride) == FAIL) break;

                nint body;

                if (mul_nint_safe(el_num, stride, &body) == FAIL) break;

                if (front_padding > NINT_MAX - body) break;

                out_size = front_padding + body - (stride - size);

                success = 1;
                break;
            }

            // Integer overflow
            if(!success) {
                overflow(p, error_token);
                return FAIL;
            }

            // Check if it fits
            if (increment_addr(p, out_size) == FAIL) return FAIL;
            *total += out_size;

            if(create_labels && !p->last_pass) {
                label->addr = struct_aligned_start;
                label->len = el_num;
                label->stride = stride;
            }

            if(p->last_pass) {
                // Last pass
                for(nint i=_addr; i<struct_aligned_start; ++i) {
                    OUT_FILE_WRITE_BYTE((unsigned char)0);
                    DBG(1, "al - 00\n");
                }
                DBG(1, "\n");

                p->addr = _addr;

                for(nint n=0; n<el_num; ++n) {
                    // DBG(1, "_addr = %d, struct_aligned_start = %d, stride = %d, out_size = %d\n", _addr, struct_aligned_start, stride, out_size);
                    if(encode_struct(p, var_name, _struct_decl->ast_struct_decl, _head, _tail, &size, 0, NULL, NULL) == FAIL) return FAIL;
                    for(nint j=0; j<((n != (el_num-1)) && (stride - size)); ++j) {
                        OUT_FILE_WRITE_BYTE((unsigned char)0);
                        DBG(1, "st - 00\n");
                    }
                }  

                p->addr = _addr;
                increment_addr(p, out_size);
            }


        }

        if(decl_field == struct_decl->tail) break;

        decl_field = decl_field->next;
    }
    return SUCCESS;
}

// AST evaluation
unint eval_ast(struct Parser* p, struct Ast_node* ast) {
    // We already failed :(
    if(ast->type != STATEMENTS_NODE) return EVAL_ERROR;

    struct AstStatements* statements = &ast->node.statements;
    for(struct AstStatementsNode* stmt = statements->head; stmt; stmt = stmt->next) {
        struct Ast_node* ast = stmt->ast;

        switch(ast->type) {
            case ASSIGN_VAR_NODE:
            case ASSIGN_APPEND_ARRAY_NODE: {
                struct AstAssignVariable* var_assign = &stmt->ast->node.var_assign;
                struct token* var_name = var_assign->name;

                struct Variable* var = get_variable(p, var_name);

                if(var == NULL && (var_assign->idx != NULL || ast->type == ASSIGN_APPEND_ARRAY_NODE || var_assign->ass_type != EQUAL)) {
                    _error_from_token(p, var_name, ERROR_TYPE_NAME, "variable is not defined");
                    return EVAL_ERROR;
                }

                // Variables are defined OR we need to create a new one
                struct Value* variable;
                if(var != NULL) {
                    // Index
                    if(var_assign->idx) {
                        struct Value val;
                        struct Value* var_ref_idx = NULL;
                        if(_eval_expr(p, var_assign->idx, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                        variable = var_ref_idx;
                    } else variable = &var->val;

                    // Append
                    if(ast->type == ASSIGN_APPEND_ARRAY_NODE) {
                        if((*variable).type != VALUE_ARRAY) {
                            _error_from_token(p, var_name, ERROR_TYPE_TYPE, "variable is not an array");
                            return EVAL_ERROR;
                        }

                        // Append a literal
                        struct Ast_node* el_ast = new_ast_node(p);
                        if(el_ast == NULL) {
                            memory_error(p, "no available memory for the appended array value");
                            return EVAL_ERROR;
                        }

                        struct Value* el_val = (struct Value*)MEM_ALLOC(sizeof(struct Value), "array append el value");
                        if(el_val == NULL) {
                            memory_error(p, "no available memory for the array value");
                            return FAIL;
                        }

                        el_ast->type = LITERAL_NODE;
                        el_ast->node.literal.value = el_val;

                        if(append_array(p, var_name, variable, el_ast) == FAIL) return EVAL_ERROR;
                        variable = el_val;
                    }
                } else {
                    // just a simple var assignment
                    struct Value val;
                    if((var = new_variable(p, var_name, &val)) == NULL) return EVAL_ERROR;
                    variable = &var->val;
                }

                struct Value _val;
                struct Value* _var_ref_idx;

                if(var_assign->ass_type == EQUAL) {
                    if(_eval_expr(p, var_assign->expr, &_val, &_var_ref_idx) == FAIL) return EVAL_ERROR;
                    *variable = _val;
                } else {
                    struct Ast_node* ast_var;
                    struct Ast_node _ast_variable;

                    // get the first variable itself
                    if(var_assign->idx) ast_var = var_assign->idx;
                    else {
                        _ast_variable.type = VAR_NODE;
                        _ast_variable.node.var = var_name;
                        ast_var = &_ast_variable;
                    }


                    struct Ast_node full_expr;
                    full_expr.type = BINOP_NODE;
                    full_expr.node.binop.left = ast_var;
                    full_expr.node.binop.op_token = var_assign->ass_token;
                    full_expr.node.binop.op = assign_type_without_equal(var_assign->ass_type);
                    full_expr.node.binop.right = var_assign->expr;

                    full_expr.node.binop._s = NULL;
                    full_expr.node.binop._e = NULL;

                    if(_eval_expr(p, &full_expr, &_val, &_var_ref_idx) == FAIL) return EVAL_ERROR;

                    *variable = _val;
                }
                break;
            }

            case IF_NODE: {
                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node._if.cond, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;

                // wrap statements in an AST node
                struct Ast_node body;
                body.type = STATEMENTS_NODE;
                
                // if
                if(bool_eval(&val)) {  
                    body.node.statements = ast->node._if.statements;
                    unint state = eval_ast(p, &body);

                    // Ifs propagate everything execpt normal execution
                    if(state != EVAL_OK) return state;
                    break; // Next statement
                }
                
                for(struct ElifCondBlock* elif = ast->node._if._elifs_start; elif; elif = elif->next) {

                    if(_eval_expr(p, elif->cond, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;

                    // elif
                    if(bool_eval(&val)) {  
                        body.node.statements = elif->statements;

                        unint state = eval_ast(p, &body);
                        if(state != EVAL_OK) return state;
                    }
                }

                if(ast->node._if._else) {
                    body.node.statements = *ast->node._if._else;
                    unint state = eval_ast(p, &body);
                    if(state != EVAL_OK) return state;
                }
                break;
            }

            case WHILE_NODE: {
                struct Value val;
                struct Value* var_ref_idx;

                while(true) {
                    if(_eval_expr(p, ast->node._while.cond, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;

                    if(!bool_eval(&val)) break;

                    // wrap statements in an AST node
                    struct Ast_node body;
                    body.type = STATEMENTS_NODE;
                    body.node.statements = ast->node._while.statements;

                    unint state = eval_ast(p, &body);

                    if(state == EVAL_ERROR) return EVAL_ERROR;
                    // else if(state == RETURN_NODE) return EVAL_RETURN;
                    else if(state == EVAL_BREAK) break;
                }
                // Next statement
                break;
            }

            case REPEAT_NODE: {
                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node._repeat.expr, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                
                if(val.type != VALUE_INT) {
                    _error_from_multiple_tokens(p, ast->node._repeat._s, ast->node._repeat._e, ERROR_TYPE_TYPE, "invalid type for @repeat");
                    return EVAL_ERROR;
                }

                if(val.val.number < 0) {
                    _error_from_multiple_tokens(p, ast->node._repeat._s, ast->node._repeat._e, ERROR_TYPE_RUNTIME, "value must be greater than 0");
                    return EVAL_ERROR;                
                }

                while(val.val.number--) {
                    // wrap statements in an AST node
                    struct Ast_node body;
                    body.type = STATEMENTS_NODE;
                    body.node.statements = ast->node._repeat.statements;

                    unint state = eval_ast(p, &body);

                    if(state == EVAL_ERROR) return EVAL_ERROR;
                    // else if(state == RETURN_NODE) return EVAL_RETURN;
                    else if(state == EVAL_BREAK) break;
                }
                // Next statement
                break;
            }

            case STRUCT_DECL_NODE : {
                if(is_struct_declared(p, ast->node.struct_decl.struct_name) == SUCCESS) {
                    _error_from_token(p, ast->node.struct_decl.struct_name, ERROR_TYPE_NAME, "struct is already declared");
                    return EVAL_ERROR;
                }

                // Ensure all struct fields have declarations
                for(struct StructDeclField* field = ast->node.struct_decl.head; field; field = field->next) {
                    if((field->type == STRUCT_DECL_NODE) && (is_struct_declared(p, field->struct_name) == FAIL)) {
                        _error_from_token(p, field->struct_name, ERROR_TYPE_NAME, "struct is not declared");
                        return EVAL_ERROR;
                    }
                }

                if(new_struct_decl(p, &ast->node.struct_decl) == NULL) return EVAL_ERROR;
                break;
            }

            case ORG_NODE: {
                if(ensure_lang(p, ast->node.org.org) == FAIL) return EVAL_ERROR;

                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node.org.expr, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                
                if(val.type != VALUE_INT) {
                    _error_from_multiple_tokens(p, ast->node.org._s, ast->node.org._e, ERROR_TYPE_TYPE, "invalid type for @org");
                    return EVAL_ERROR;
                }

                if(val.val.number < 0) {
                    _error_from_multiple_tokens(p, ast->node.org._s, ast->node.org._e, ERROR_TYPE_RUNTIME, "value must be greater than 0");
                    return EVAL_ERROR;                
                }

                p->addr = val.val.number;
                break;
            }

            case LABEL_NODE: {
                if(p->last_pass) break;

                if(is_label_declared(p, ast->node.label.name) == SUCCESS) {
                    _error_from_token(p, ast->node.label.name, ERROR_TYPE_NAME, "redeclaration of label");
                    return EVAL_ERROR;
                }

                if(new_label_decl(p, ast->node.label.name, &p->global_label_decl, &p->global_label_decl_tail, p->addr) == NULL) return EVAL_ERROR;
                break;
            }

            /*
            case FUN_NODE: {
                if(is_func_declared(p, ast->node.fun_decl.func_name) == SUCCESS) {
                    _error_from_token(p, ast->node.fun_decl.func_name, ERROR_TYPE_NAME, "function is already declared");
                    return EVAL_ERROR;
                }

                if(new_func_decl(p, &ast->node.fun_decl) == NULL) return EVAL_ERROR;

                // Eval its statements

                // Remove local labels
                p->func_label_decl = p->func_label_decl_tail = NULL;


                break;
            }
            */
            case DEL_NODE: {
                struct Variable* var = get_variable(p, ast->node.del.ident);

                if(var == NULL) break;

                // Remove and link
                if (var->prev) var->prev->next = var->next;
                else p->variables = var->next;

                if (var->next) var->next->prev = var->prev;
                else p->variables_tail = var->prev;

                break;
            }

            case STRUCT_VAR_NODE: {
                // Check struct type
                struct StructsDecl* _struct = get_struct_decl(p, ast->node.struct_var.struct_name);
                if(_struct == NULL) {
                    _error_from_token(p, ast->node.struct_var.struct_name, ERROR_TYPE_NAME, "struct is not declared");
                    return EVAL_ERROR;
                }

                if(!p->last_pass && is_struct_var(p, ast->node.struct_var.var_name) == SUCCESS) {
                    _error_from_token(p, ast->node.struct_var.var_name, ERROR_TYPE_NAME, "redeclaration of struct variable");
                    return EVAL_ERROR;
                }

                struct LabelDecl** deep_head = NULL;
                struct LabelDecl** deep_tail = NULL;

                if(!p->last_pass) {
                    // new label for the variable.
                    struct LabelDecl* label = new_label_decl(p, ast->node.struct_var.var_name, &p->global_label_decl, &p->global_label_decl_tail, p->addr);
                    if(label == NULL) return EVAL_ERROR;

                    deep_head = &label->deep_head;
                    deep_tail = &label->deep_tail;
                }

                nint total;
                if(encode_struct(p, ast->node.struct_var.var_name, _struct->ast_struct_decl, ast->node.struct_var.head, ast->node.struct_var.tail, &total, !p->last_pass, deep_head, deep_tail) == FAIL) return EVAL_ERROR;
                
                DBG(1, "struct total = %d | p->addr = %d\n", total, p->addr);
                break;
            }

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
            case SAVEP_NODE: {
                nint total, len, data_addr, stride;
                if(encode_space_ident(p, ast->type, &ast->node.space, &total, &data_addr, &len, &stride, 0) == FAIL) return EVAL_ERROR;

                DBG(1, "total = %d\n", total);
                break;
            }

            case STRING_NODE: {
                if(ensure_lang(p, ast->node.string.string) == FAIL) return EVAL_ERROR;

                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node.string.expr, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                
                if(val.type != VALUE_STR) {
                    _error_from_multiple_tokens(p, ast->node.string._s, ast->node.string._e, ERROR_TYPE_TYPE, "invalid type for @string");
                    return EVAL_ERROR;
                }

                unsigned char buf[CP_ENCODING_BUF];

                // Get the whole length
                nint len = 0;
                nint addr = p->addr;
                for(unint i=0; i<val.val.string.len; ++i) {
                    nint n = CP_TO_ENCODING_BUF_GET_LEN(val.val.string.str[i], buf) - 1;
                    if(increment_addr(p, n) == FAIL) goto _overflow_str;
                    len += n;
                }

                // Restore
                p->addr = addr;

                nint st_align = (ast->node.string.align_start_expr == NULL ? 1 : ast->node.string.align_start_expr->node.literal.value->val.number);
                nint out_size, aligned_start, stride;
                
                // Add the alignment

                if(array_total_size_safe(p->addr, 1, len, st_align, 1, &out_size, &aligned_start, &stride) == FAIL) goto _overflow_str;

                if(increment_addr(p, out_size) == FAIL) goto _overflow_str;

                // Last pass
                if(p->last_pass) {
                    // Alignment
                    for(nint i=addr; i<aligned_start; ++i) {
                        OUT_FILE_WRITE_BYTE((unsigned char)0);
                        DBG(1, "al - 00\n");
                    }
                    DBG(1, "\n");
                    for(unint i=0; i<val.val.string.len; ++i) {
                        nint n = CP_TO_ENCODING_BUF_GET_LEN(val.val.string.str[i], buf);
                        for(nint j=0; j<(n-1); ++j) {
                            OUT_FILE_WRITE_BYTE(buf[j]);
                            DBG(1, "%02x\n", buf[j]);
                        }
                    }
                }

                break;
_overflow_str:
                overflow(p, ast->node.string.string);
                return EVAL_ERROR;
            }

            case ALIGN_NODE: {
                if(ensure_lang(p, ast->node.string.string) == FAIL) return EVAL_ERROR;

                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node.align.expr, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                
                if(val.type != VALUE_INT) {
                    _error_from_multiple_tokens(p, ast->node.align._s, ast->node.align._e, ERROR_TYPE_TYPE, "invalid type for @align");
                    return EVAL_ERROR;
                }

                if(val.val.number < 0) {
                    _error_from_multiple_tokens(p, ast->node.align._s, ast->node.align._e, ERROR_TYPE_RUNTIME, "value must be greater than 0");
                    return EVAL_ERROR;                
                }

                nint end_addr, size;
                if(align_address_safe(p->addr, val.val.number, &end_addr) == FAIL) goto _align_overflow;

                if(increment_addr(p, (size = end_addr - p->addr)) == FAIL) goto _align_overflow;

                // Last pass
                if(p->last_pass) for(nint i=0; i<size; ++i) {
                    OUT_FILE_WRITE_BYTE((unsigned char)0);
                    DBG(1, "@align - 00\n");
                }

                break;
_align_overflow:
                overflow(p, ast->node.org.org);
                return EVAL_ERROR;
            }

            case ERROR_NODE: {
                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node.error.error, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                
                if(val.type != VALUE_STR) {
                    _error_from_multiple_tokens(p, ast->node.error._s, ast->node.error._e, ERROR_TYPE_TYPE, "invalid type for @error");
                    return EVAL_ERROR;
                }

                LOG("  File: \"%s\", line %d\n    @error \"", p->tok->source, ast->node.error._s->lineno);
                for(unint i=0; i<val.val.string.len; ++i) LOG_CP(val.val.string.str[i]);
                LOG("\"\n");
                return EVAL_ERROR;
            }

            case ASSERT_NODE: {
                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node.assert.assert, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;

                if(!bool_eval(&val)) {
                    _error_from_multiple_tokens(p, ast->node.assert._s, ast->node.assert._e, ERROR_TYPE_ASSERT, "assertion of expression failed");
                    return EVAL_ERROR;
                }
                break;
            }

            case WARN_NODE: {
                struct Value val;
                struct Value* var_ref_idx;

                if(_eval_expr(p, ast->node.warn.warn, &val, &var_ref_idx) == FAIL) return EVAL_ERROR;
                
                if(val.type != VALUE_STR) {
                    _error_from_multiple_tokens(p, ast->node.warn._s, ast->node.warn._e, ERROR_TYPE_TYPE, "invalid type for @warn");
                    return EVAL_ERROR;
                }

                // Except on the last pass
                if(!p->last_pass) {
                    LOG("  File: \"%s\", line %d\n    @warn \"", p->tok->source, ast->node.warn._s->lineno);
                    for(unint i=0; i<val.val.string.len; ++i) LOG_CP(val.val.string.str[i]);
                    LOG("\"\n");
                }
                break; 
            }

            case BREAK_NODE: {
                return EVAL_BREAK;
            }

            case CONTINUE_NODE: {
                return EVAL_CONTINUE;
            }

            /*
            case RETURN_NODE: {
                return EVAL_RETURN;
            }
            */

            case CODE_NODE: {
                p->active_lang = ast->node.code.lang;
                break;
            }

            case INSTRUCTION_NODE: {
                if(ensure_lang(p, ast->node.instruction.name) == FAIL) return EVAL_ERROR;
                // Make sure vars are declared
                // or args or function vars
                unint nbytes = p->active_lang->exec(p, &ast->node.instruction);
                if(nbytes == INSTRUCTION_FAILED) return EVAL_ERROR;

                // check if unresolved
                if(increment_addr(p, nbytes) == FAIL) {
                    overflow(p, ast->node.instruction.name);
                    return EVAL_ERROR;
                };
                break;
            }

            default:
                return EVAL_ERROR;
        }
    }
    
    return EVAL_OK;
}
