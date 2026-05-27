#include <stdarg.h>
#include <math.h>

#include "api/memory.h"
#include "api/debug.h"

#include "asm_lang.h"
#include "eval.h"
#include "token.h"

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

struct LabelDecl* new_label_decl(struct Parser* p, struct AstLabel* ast_label_decl, nint addr) {
    struct LabelDecl* label = (struct LabelDecl*)MEM_ALLOC(sizeof(struct LabelDecl), "eval: new label decl");
    if(label == NULL) {
        _error_from_token(p, ast_label_decl->name, ERROR_TYPE_MEMORY, "no available memory for label declaration");
        return NULL;
    }

    label->label = ast_label_decl;
    label->addr = addr;
    label->next = NULL;

    struct LabelDecl** head = &p->global_label_decl; // ast_label_decl->is_inside_func ? &p->func_label_decl : &p->global_label_decl;
    struct LabelDecl** tail = &p->global_label_decl_tail; // ast_label_decl->is_inside_func ? &p->func_label_decl_tail : &p->global_label_decl_tail;

    // First Variable
    if(*head == NULL) *head = label;

    // Link the new variable
    if(*tail != NULL) (*tail)->next = label;
    *tail = label;

    return label;

}

struct LabelDecl* get_label_decl(struct Parser* p, struct token* _token /*, unint is_inside_func */) {
    /*
    if(p == NULL || (is_inside_func && (p->func_label_decl == NULL)) || (!is_inside_func && (p->global_label_decl == NULL))) return NULL;

    if(is_inside_func) {
        for(struct LabelDecl* label = p->func_label_decl; label != NULL; label = label->next) {
            if(label->label->name->len != _token->len) continue;

            unint i;
            for(i = 0; i < _token->len; ++i) if(label->label->name->cps[i] != _token->cps[i]) break;
            
            // Full match
            if(i == _token->len) return label;
        }
    }
    */

    for(struct LabelDecl* label = p->global_label_decl; label != NULL; label = label->next) {
        if(label->label->name->len != _token->len) continue;

        unint i;
        for(i = 0; i < _token->len; ++i) if(label->label->name->cps[i] != _token->cps[i]) break;
        
        // Full match
        if(i == _token->len) return label;
    }

    return NULL;   
}

unint is_label_declared(struct Parser* p, struct token* _token/* , unint is_inside_func*/) {
    return (get_label_decl(p, _token/*, is_inside_func */) == NULL) ? FAIL : SUCCESS;
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
        if(_equality_check(&tleft->this, NOTEQUAL, &tright->this)) return !(op == EQEQUAL);
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
        _error_from_token(p, op_token, ERROR_TYPE_TYPE, "can only perform indexation on array/string types");
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

                        *val = el->this;
                        *var_ref_idx = &el->this;
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
                            case LESSEQUAL:    { val->val.number = da >= db; break; }
                        }
                        return SUCCESS;
                    }

                    a = vleft.val.number;
                    b = vright.val.number;

                    switch(binop->op) {
                        case GREATER:      { val->val.number = a >  b; break; }
                        case GREATEREQUAL: { val->val.number = a >= b; break; }
                        case LESS:         { val->val.number = a <  b; break; }
                        case LESSEQUAL:    { val->val.number = a >= b; break; }
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

// Space identifiers

unint align_address_safe(nint address, nint alignment, nint* out) {
    if (!out) return FAIL;

    nint rem = address % alignment;

    if (alignment == 0 || rem == 0) {
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

/*
  base_address - current address where allocation begins
  n            - number of elements
  elem_size    - size of each element
  elem_align   - alignment of each element
  array_align  - alignment of array start
  out_size     - output variable
*/
unint array_total_size_safe(
    nint base_address,
    nint n,
    nint elem_size,
    nint elem_align,
    nint array_align,
    nint *out_size)
{
    if (!out_size) return FAIL;

    nint aligned_start;

    if (align_address_safe(base_address, array_align, &aligned_start) == FAIL) return FAIL;

    nint front_padding = aligned_start - base_address;

    nint stride;

    if (align_address_safe( elem_size, elem_align, &stride) == FAIL) return FAIL;

    nint body;

    if (mul_nint_safe(n, stride, &body) == FAIL) return FAIL;


    if (front_padding > NINT_MAX - body) return FAIL;

    *out_size = front_padding + body;
    return SUCCESS;
}

// Misc

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

                        // get the new object
                        struct Value tmp;
                        if(append_array(p, var_name, variable, &tmp) == FAIL) return EVAL_ERROR;
                        variable = &(*variable).val.arr.tail->this;
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

                if(new_struct_decl(p, &ast->node.struct_decl) == NULL) return EVAL_ERROR;
                break;
            }

            case ORG_NODE: {
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
                if(is_label_declared(p, ast->node.label.name/* , ast->node.label.is_inside_func */) == SUCCESS) {
                    _error_from_token(p, ast->node.label.name, ERROR_TYPE_NAME, "redeclaration of label");
                    return EVAL_ERROR;
                }

                if(new_label_decl(p, &ast->node.label, p->addr) == NULL) return EVAL_ERROR;
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

                if(ensure_lang(p, ast->node.space.space_ident) == FAIL) return EVAL_ERROR;

                nint size_of_el;
                switch(ast->type) {
                    case BYTE_NODE:
                    case SAVEB_NODE: {size_of_el = 1; break; }
                    case WORD_NODE:
                    case SAVEW_NODE: {size_of_el = 2; break; }
                    case DWORD_NODE:
                    case SAVEDW_NODE: {size_of_el = 4; break; }
                    case QWORD_NODE:
                    case SAVEQ_NODE: {size_of_el = 8; break; }
                    case FLOAT_NODE:
                    case SAVEF_NODE: {size_of_el = 4; break; }
                    case DOUBLE_NODE:
                    case SAVED_NODE: {size_of_el = 8; break; }
                    case PTR_NODE:
                    case SAVEP_NODE: {
                        // make sure lang is set and get the size of a pointer
                    }
                }

                nint total = 0;
                if(
                    array_total_size_safe(
                        p->addr,
                        (ast->node.space.len_expr == NULL ? 1 : ast->node.space.len_expr->node.literal.value->val.number),
                        size_of_el,
                        (ast->node.space.align_per_el_expr == NULL ? 1 : ast->node.space.align_per_el_expr->node.literal.value->val.number),
                        (ast->node.space.align_start_expr == NULL ? 1 : ast->node.space.align_start_expr->node.literal.value->val.number),
                        &total
                    ) == FAIL
                ) goto _space_size_overflow;

                if(increment_addr(p, total) == SUCCESS) break;
_space_size_overflow:
                _error_from_token(p, ast->node.space.space_ident, ERROR_TYPE_OVERFLOW, "size overflow");
                return EVAL_ERROR;
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
                p->addr += nbytes;
                break;
            }

            default:
                return EVAL_ERROR;
        }
    }
    
    return EVAL_OK;
}
