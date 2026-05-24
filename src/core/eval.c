#include <stdarg.h>
#include <math.h>

#include "api/memory.h"
#include "api/debug.h"

#include "eval.h"
#include "token.h"

void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...);
void _error_from_multiple_tokens(struct Parser* p, struct token* _s, struct token* _e, const char *stype, const char *format);

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

    // First Variable
    if(p->variables == NULL) p->variables = var;

    // Link the new variable
    if(p->variables_tail != NULL) p->variables_tail->next = var;
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
    if(op == PLUS && binop->right == NULL && (
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
    if ((op == PLUS || op == MINUS || op == SLASH || op == STAR || op == PERCENT) && ( 
            (tcleft && (vleft->type != VALUE_INT && vleft->type != VALUE_DOUBLE)) ||
            (tcright && (vright->type != VALUE_INT && vright->type != VALUE_DOUBLE)) 
        ) && (
            (tcleft && (vleft->type != VALUE_STR)) &&
            (tcright && (vright->type != VALUE_STR)) 
        )
    ) {
        _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "can only perform arithmetic operations on integer/decimal types");
        return FAIL;
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
            // Grab the current address value as an int
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

            if(pleft->type == LITERAL_NODE || pleft->type == VAR_NODE){
                if(_eval_expr(p, pleft, &vleft, var_ref_idx) == FAIL) return FAIL;
                tcleft = 1;
            }

            if(pright && (pright->type == LITERAL_NODE || pright->type == VAR_NODE)) {
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

unint asssign_type_without_equal(unint assign_type) {
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

unint eval_ast(struct Parser* p, struct Ast_node* ast) {
    // We already failed :(
    if(ast->type != STATEMENTS_NODE) return FAIL;

    struct AstStatements* statements = &ast->node.statements;
    for(struct AstStatementsNode* stmt = statements->head; stmt; stmt = stmt->next) {
        struct Ast_node* ast = stmt->ast;
        switch(ast->type) {
            case ASSIGN_VAR_NODE:
            case ASSIGN_APPEND_ARRAY_NODE:
                struct AstAssignVariable* var_assign = &stmt->ast->node.var_assign;
                struct token* var_name = var_assign->name;

                struct Variable* var = get_variable(p, var_name);

                if(var == NULL && (var_assign->idx != NULL || ast->type == ASSIGN_APPEND_ARRAY_NODE || var_assign->ass_type != EQUAL)) {
                    _error_from_token(p, var_name, ERROR_TYPE_RUNTIME, "variable is not defined");
                    return FAIL;
                }

                // Variables are defined OR we need to create a new one
                struct Value* variable;
                if(var != NULL) {
                    // Index
                    if(var_assign->idx) {
                        struct Value val;
                        struct Value* var_ref_idx = NULL;
                        if(_eval_expr(p, var_assign->idx, &val, &var_ref_idx) == FAIL) return FAIL;
                        variable = var_ref_idx;
                    } else variable = &var->val;

                    // Append
                    if(ast->type == ASSIGN_APPEND_ARRAY_NODE) {
                        if((*variable).type != VALUE_ARRAY) {
                            _error_from_token(p, var_name, ERROR_TYPE_TYPE, "variable is not an array");
                            return FAIL;
                        }

                        // get the new object
                        struct Value tmp;
                        if(append_array(p, var_name, variable, &tmp) == FAIL) return FAIL;
                        variable = &(*variable).val.arr.tail->this;
                    }
                } else {
                    // just a simple var assignment
                    struct Value val;
                    if((var = new_variable(p, var_name, &val)) == NULL) return FAIL;
                    variable = &var->val;
                }

                struct Value _val;
                struct Value* _var_ref_idx;

                if(var_assign->ass_type == EQUAL) {
                    if(_eval_expr(p, var_assign->expr, &_val, &_var_ref_idx) == FAIL) return FAIL;

                    *variable = _val;
                } else {
                    struct Ast_node* ast_var;

                    // get the first variable itself
                    if(var_assign->idx) ast_var = var_assign->idx;
                    else {
                        struct Ast_node _ast_variable;
                        _ast_variable.type = VAR_NODE;
                        _ast_variable.node.var = var_name;
                        ast_var = &_ast_variable;
                    }


                    struct Ast_node full_expr;
                    full_expr.type = BINOP_NODE;
                    full_expr.node.binop.left = ast_var;
                    full_expr.node.binop.op_token = var_name;
                    full_expr.node.binop.op = asssign_type_without_equal(var_assign->ass_type);
                    full_expr.node.binop.right = var_assign->expr;

                    full_expr.node.binop._s = NULL;
                    full_expr.node.binop._e = NULL;

                    if(_eval_expr(p, &full_expr, &_val, &_var_ref_idx) == FAIL) return FAIL;

                    *variable = _val;
                }

                break;
        }
    }

    return SUCCESS;
}
