#include <stdarg.h>
#include <math.h>
#include "api/memory.h"
#include "api/log.h"
#include "api/debug.h"

#include "token.h"
#include "err.h"

#include "lexer.h"
#include "parser.h"
#include "variables.h"
#include "ast.h"

#include "types.h"

#define IF_IDENTIDIER ((int32_t[]){'i', 'f', -1})
#define INCLUDE_IDENTIFIER ((int32_t[]){'i', 'n', 'c', 'l', 'u', 'd', 'e', -1})
#define EXPORT_IDENTIFIER ((int32_t[]){'e', 'x', 'p', 'o', 'r', 't', -1})
#define EXTERN_IDENTIFIER ((int32_t[]){'e', 'x', 't', 'e', 'r', 'n', -1})

extern const char * const _Parser_TokenNames[];

// Errors

// Do not call on multiline comment tokens
void _error_from_token(struct Parser* p, struct token* _token, const char *stype, const char *format, ...) {
    va_list va;
    va_start(va, format);
    //DBG(1, "col_offset = %d | end_col_offset = %d\n", _token->col_offset, _token->end_col_offset);
    _syntaxerror_range_with_type(p->tok, stype, format, _token->lineno, _token->end_lineno, _token->col_offset+1, _token->end_col_offset+1, va);
    //_format_syntax_error(stype, format, p->tok->source, &_token->lineno, &col_offset, &_token->end_lineno, &end_col_offset, _token->start, _token->end, va);
    va_end(va);
}

void _error_no_err_line(struct Parser* p, const char* errmsg, ...) {
    va_list va;
    va_start(va, errmsg);
    _format_syntax_error(ERROR_TYPE_MESSAGE, errmsg, p->tok->source, &p->tok->lineno, NULL, &p->tok->lineno, NULL, NULL, NULL, va);
    va_end(va);
}

void _error_known_location(struct Parser* p, nint lineno, nint col_offset, nint end_lineno, nint end_col_offset, const char* errmsg, ...) {
    va_list va;
    va_start(va, errmsg);
    _syntaxerror_range(p->tok, errmsg, lineno, end_lineno, col_offset, end_col_offset, va);
    va_end(va);
}

void memory_error(struct Parser* p, const char* format, ...) {
    va_list va;
    va_start(va, format);
    _format_syntax_error(ERROR_TYPE_MEMORY, format, p->tok->source, NULL, NULL, NULL, NULL, NULL, NULL, va);
    va_end(va);
}

// Token

unint _fill_token(struct Parser* p) {
    // Allocate
    struct token* new_token = (struct token*)MEM_ALLOC(sizeof(struct token), "token struct");
    if(new_token == NULL) {
        memory_error(p, "no available memory for token structure"); 
        return FAIL;
    }

    unint type = tokenize(p->tok, new_token);

    // Link
    if(p->tail != NULL) p->tail->next = new_token;
    p->tail = new_token;

    // Firt token
    if(p->head == NULL) p->head = new_token;

    // Check for errors
    if (type == ERRORTOKEN && p->tok->done == E_DECODE) return FAIL;
    const char *msg = NULL;
    if(type == ERRORTOKEN) {
        switch(p->tok->done) {
            case E_EOF:
                if(p->tok->level) {
                    nint error_lineno = p->tok->parenlinenostack[p->tok->level-1];
                    nint error_col = p->tok->parencolstack[p->tok->level-1];
                    _error_known_location(p, error_lineno, error_col, error_lineno, -1, "'%c' was never closed", p->tok->parenstack[p->tok->level-1]);
                } else _Tokenizer_syntaxerror_known_range(p->tok, -1, -1, "unexpected EOF while parsing");
                return FAIL;
            case E_DEDENT:
                _Tokenizer_syntaxerror_known_range(p->tok, -1, -1, "unindent does not match any outer indentation level");
                return FAIL;
            case E_NOMEM:
                memory_error(p, "no available memory for token");
                return FAIL;
            case E_TABSPACE:
                msg = "inconsistent use of tabs and spaces in indentation";
                break;
            case E_TOODEEP:
                msg = "too many levels of indentation";
                break;
            case E_COLUMNOVERFLOW:
                _error_no_err_line(p, "parser column offset overflow - source line is too big");
                return FAIL;
            default:
                msg = "unknown parsing error";        
        }
        _error_known_location(p, p->tok->lineno, 0, p->tok->lineno, -1, msg);
        return FAIL;
    }

    return SUCCESS;
}

struct token* _read_token(struct Parser* p) {
    // Advance if we already have tokens
    if (p->last_token && p->last_token != p->tail)
        return (p->peek = p->last_token = p->last_token->next);

    // Ensure we have tokens
    if (!p->last_token) {
        if (!p->head && _fill_token(p) == FAIL)
            return NULL;
        return (p->peek = p->last_token = p->head);
    }

    // We're at tail, try to extend
    if (_fill_token(p) == FAIL)
        return NULL;

    return (p->peek = p->last_token = p->tail);
}

struct token* _peek_token(struct Parser* p) {
    // Advance if we already have tokens
    if (p->peek && p->peek != p->tail)
        return (p->peek = p->peek->next);

    // Ensure we have tokens
    if (!p->peek) {
        if (!p->head && _fill_token(p) == FAIL)
            return NULL;
        return (p->peek = p->head);
    }

    // We're at tail, try to extend
    if (_fill_token(p) == FAIL)
        return NULL;

    return (p->peek = p->tail);
}

void _reset_peek(struct Parser* p) {
    p->peek = p->last_token;
}

struct Parser* _Parser_New(struct tok_state* tok) {
    struct Parser* p = MEM_ALLOC(sizeof(struct Parser), "parser");
    if(p == NULL) return NULL;

    p->tok = tok;
    p->variables = p->variables_tail = NULL;
    p->head = p->tokens = p->last_token = p->peek = p->tail = NULL;

    return p;
}

unint is_at_identifier(struct token* _token, int32_t* identifier) {
    // Reject if its not an identifier
    if(_token->type != NAME) return FAIL;

    // Just check if it starts with @
    if(identifier == NULL) {
        if(_token->len < 1) return FAIL;
        return (_token->cps[0] == '@') ? SUCCESS : FAIL;
    }

    // Get the len of the identifier
    unint identifier_len = 0;
    while (identifier[identifier_len] != -1) ++identifier_len;

    // First size check
    if(_token->len < identifier_len + 1) return FAIL;

    // Check for @
    if(_token->cps[0] != '@') return FAIL;

    for(unint i=0; i<(identifier_len); ++i) if(_token->cps[i+1] != identifier[i]) return FAIL;
    return SUCCESS;
}

// Pratt Parsing
#define UNARY_BP 120
#define INDEX_BP 130

unint _expr_get_binding_power(unint type, unint* lbp, unint* rbp) {
    switch(type) {
        case DOUBLEVBAR: { *lbp = 10; *rbp = 11; break; }
        case DOUBLEAMPER:  { *lbp = 20; *rbp = 21; break; }

        case NOTEQUAL:
        case GREATER:
        case GREATEREQUAL:
        case EQEQUAL:
        case LESS:
        case LESSEQUAL: { *lbp = 30; *rbp = 31; break; }

        case VBAR: { *lbp = 40; *rbp = 41; break; }
        case CIRCUMFLEX: { *lbp = 50; *rbp = 51; break; }
        case AMPER: { *lbp = 60; *rbp = 61; break; }

        case LEFTSHIFT:
        case RIGHTSHIFT: { *lbp = 70; *rbp = 71; break; } // Done

        case DOT: { *lbp = 90; *rbp = 80; break; }

        case PLUS:
        case MINUS: { *lbp = 100; *rbp = 101; break; } // Done

        case SLASH:
        case PERCENT:      // Done
        case STAR: { *lbp = 110; *rbp = 111; break; }

        case DOUBLESTAR: { *lbp = 140; *rbp = 130; break; }
        default : return FAIL;
    }
    return SUCCESS;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_bp, unint stop_on_comma);
unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val);

// Will return NULL or an AST
struct Ast_node* _parse_expr_prefix(struct Parser* p, unint stop_on_comma) {
    // Request a token
    struct token* _token = _read_token(p);
    if(_token == NULL) return NULL;

    switch(_token->type) {
        case LPAR:
            struct Ast_node* left = _parse_expr(p, 0, 0); // Do not allow commas
            if(left == NULL) return NULL;

            struct token * rpar = _read_token(p);
            if(rpar == NULL) return NULL;

            if(rpar->type != RPAR) {
                DBG(1, "does not end on ')'\n");
                return NULL;
            }

            return left;
        case STRING:
            DBG(1, "Expr pref is string\n");
            return new_ast_string(p, _token);
        case NUMBER:
            DBG(1, "Expr pref is number\n");
            return new_ast_number(p, _token, 0);
        case MINUS:
            // might be a negation or a negative number
            struct token* _peek = _peek_token(p);
            if(_peek == NULL) return NULL;

            // Handle the number
            if(_peek->type == NUMBER) {
                DBG(1, "Expr pref is negative number\n");

                // Eat the number
                _token = _read_token(p);
                if(_token == NULL) return NULL;

                return new_ast_number(p, _token, 1);
            }

            // Reset the peek and treat the token as unary
            _reset_peek(p);
        case PLUS:
        case TILDE:
        case EXCLAMATION:
            /*
                It doesnt really matter to pass stop_on_comma or 0 to the _parse_expr as the expression being parsed is just one token
                and expression_prefix does not allow ',' so ...
            */
            struct Ast_node* node = _parse_expr(p, UNARY_BP, stop_on_comma);
            if(node == NULL) return NULL;

            return new_ast_binop(p, _token, _token->type, node, NULL);

        case NAME:
            if(is_at_identifier(_token, NULL) == SUCCESS) {
                _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "@ identifiers are not allowed on expressions");
                return NULL;
            }

            struct Variable* var = get_variable(p, _token->cps, _token->len);
            if(var == NULL) {
                _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "variable is not declared");
                return NULL;
            }

            return new_ast_variable(p, _token, var);
        case LSQB:

            if((_token = _peek_token(p)) == NULL || _token->type == COMMA) {
                _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected a value before the comma");
                return NULL;
            }
            _reset_peek(p); // Reset so it does not consume any valid start

            // Create Array
            struct Value* arr = new_array(p, _token);
            if(arr == NULL) return NULL;

            while(true) {
                // Finished
                if(_token->type == RSQB) break;

                // parse the expression
                DBG(1, "STARTED PARSING ELEMENT\n");
                struct Ast_node* el = _parse_expr(p, 0, 1); // Do stop on commas
                if(el == NULL) return NULL;

                // Eval the expression
                struct Value val;
                unint suc = _eval_expr(p, el, &val);
                if(suc == FAIL) return NULL;

                // Append to the array
                if(append_array(p, _token, arr, &val) == FAIL) return NULL;

                DBG(1, "-------------------- ELEMENT APPENDED P=%p\n", el);

                // Read possibly the next comma or ]
                if((_token = _read_token(p)) == NULL) return NULL;
                if(_token->type == COMMA) {
                    struct token* rsqb = _peek_token(p);
                    if(rsqb == NULL) return NULL;

                    if(rsqb->type == RSQB) {
                        _error_from_token(p, _token, ERROR_TYPE_MESSAGE, "expected a value after the comma");
                        return NULL;
                    }
                    _reset_peek(p);
                }
            }

            return new_ast_array(p, _token, arr);
        default:
            _error_from_token(p, _token, ERROR_TYPE_EXPRESSION, "invalid token for expression");
            return NULL;
    }
    // Just in case
    return NULL;
}

struct Ast_node* _parse_expr(struct Parser* p, unint min_bp, unint stop_on_comma) {
    // Prefix
    // On an array, _parse_expr will never be called when _parse_expr_prefix is a comma, no need to do checks
    struct Ast_node* left = _parse_expr_prefix(p, stop_on_comma);
    if(left == NULL) return NULL;

    unint lbp, rbp;
    while(true) {
        struct token* op = _peek_token(p);
        if(op == NULL) return NULL;

        if(op->type == ENDMARKER || op->type == NEWLINE || op->type == RPAR || op->type == RSQB || (stop_on_comma && op->type == COMMA)) break;

        // Postfix operator
        if(op->type == LSQB) {

            if(_read_token(p) == NULL) return NULL;
            
            struct Ast_node* right = _parse_expr(p, 0, 0); // Do not allow commas
            if(right == NULL) return NULL;

            struct token * rsqb = _read_token(p);
            if(rsqb == NULL) return NULL;
            if(rsqb->type != RSQB) {
                DBG(1, "does not end on ']'\n");
                return NULL;
            }

            DBG(1, "Creating LSQB NODE\n");
            left = new_ast_binop(p, op, LSQB, left, right);
            continue;
        }

        
        unint suc = _expr_get_binding_power(op->type, &lbp, &rbp);
        if(suc == FAIL) {
            _error_from_token(p, op, ERROR_TYPE_EXPRESSION, "Invalid operator in expression");
            return NULL;
        }

        if(lbp < min_bp) break;

        op = _read_token(p);
        if(op == NULL) return NULL;

        struct Ast_node* right = _parse_expr(p, rbp, stop_on_comma); // Propagate the setting used to the other nodes
        if(right == NULL) return NULL;

        // build the AST node
        left = new_ast_binop(p, op, op->type, left, right);
        if(left == NULL) return NULL;
    }

    // reset the peek so it doesnt keep peeking forward
    _reset_peek(p);
    return left;
}

unint _eval_expr(struct Parser* p, struct Ast_node* expr, struct Value* val) {
    switch(expr->type) {
        case LITERAL_NODE:
            *val = *expr->node.literal.value;
            return SUCCESS;
        case VAR_NODE:
            *val = expr->node.var.val;
            return SUCCESS;
        case BINOP_NODE:
            struct AstBinOp* binop = &expr->node.binop;
            struct Ast_node* pleft = binop->left;
            struct Ast_node* pright = binop->right;
            struct token* op_token = binop->op_token;
            unint op = binop->op;
            
            struct Value vleft, vright;
            unint tcleft = 0, tcright = 0;

            if(pleft->type == LITERAL_NODE || pleft->type == VAR_NODE){
                if(_eval_expr(p, pleft, &vleft) == FAIL) return FAIL;
                tcleft = 1;
                // TYPE CHECK NEEDED
            }

            if(pright && (pright->type == LITERAL_NODE || pright->type == VAR_NODE)) {
                if(_eval_expr(p, pright, &vright) == FAIL) return FAIL;
                tcright = 1;
                // TYPE CHECK NEEDED
            }

            // Type checks

            // Concatnation only allowed on Strings/characters
            if(op == DOT && ( 
                (tcleft && (vleft.type != VALUE_STR)) ||
                (tcright && (vright.type != VALUE_STR))
            )) {
                _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "Can only perform concatnation on strings types");
                return FAIL;
            }

            // Arithmetic only on ints/floats 
            if ((op == PLUS || op == MINUS || op == SLASH || op == STAR || op == PERCENT) && ( 
                (tcleft && (vleft.type != VALUE_INT && vleft.type != VALUE_DOUBLE)) ||
                (tcright && (vright.type != VALUE_INT && vright.type != VALUE_DOUBLE)) 
            )) {
                _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "Can only perform arithmetic operations on integer/decimal types");
                return FAIL;
            }

            // Indexation only on array types
            if(op == LSQB && 
                (tcleft && vleft.type != VALUE_ARRAY)
            ) {
                _error_from_token(p, op_token, ERROR_TYPE_TYPE, "Can only perform indexation on array types");
                return FAIL;
            }

            // Invalid index
            if(op == LSQB &&
                (tcright && vright.type != VALUE_INT)
            ) {
                _error_from_token(p, op_token, ERROR_TYPE_TYPE, "Indices can only by of integer type");
                return FAIL;
            }

            // Evaluate
            if(_eval_expr(p, pleft, &vleft) == FAIL) return FAIL;

            // May not exist depending on the operator
            if(pright && _eval_expr(p, pright, &vright) == FAIL) return FAIL;

            nint a, b;
            double da, db;

            // Eval operators
            switch(binop->op) {
                case PLUS:
                    // unary plus, if right is not present, preserve the original type
                    if(!pright) { *val = vleft; return SUCCESS; }

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

                case LEFTSHIFT:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number << vright.val.number;
                    return SUCCESS;
                case RIGHTSHIFT:
                    val->type = VALUE_INT;
                    val->val.number = vleft.val.number >> vright.val.number;
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
                    unint len = vleft.val.arr.len;
                    if(index < 0) index = len + index;

                    if(index < 0 || index >= len) {
                        _error_from_token(p, op_token, ERROR_TYPE_INDEX_ERROR, "Index out of range");
                        return FAIL;
                    }

                    struct ArrayElement* el = vleft.val.arr.head;
                    for(nint i=0; i<index; ++i) el = el->next;

                    *val = el->this;
                    return SUCCESS;

                default:
                    _error_from_token(p, op_token, ERROR_TYPE_EXPRESSION, "Evaluation of that operator hasn't been implemented");
                    return FAIL;
            }
        default:
            return FAIL;  
    }
}

void* _run_parser(struct Parser* p) {

    struct token* _token;
    void* ast = NULL;
    unint suc = 0;

    // Create the first AST node
    if(new_ast(p) == FAIL) {
        memory_error(p, "No available memory for AST's root node");
        return NULL;
    }

    struct Variable* var = new_variable(p, (int32_t[]){'h','e','l','l','o','_','w','o','r','l','d'}, 11, &(struct Value){
        .type = VALUE_INT,
        .val.number = 0x10
    });

    if(var == NULL) {
        DBG(1, "Error building variable\n");
        return NULL;
    }

    (is_variable_declared(p, (int32_t[]){'h', 'i'}, 2) == SUCCESS) ?
        DBG(1, "Variable is declared\n") :
        DBG(1, "Variable is NOT declared\n");

    struct Ast_node* expr = _parse_expr(p, (unint)(0), 0);
    if(expr == NULL) {
        DBG(1, "Error building expression\n");
        return NULL;
    }
    
    DBG(1, "#################################\n");
    dbg_ast(expr);

    DBG(1, "#################################\n");    

    struct Value out;
    suc = _eval_expr(p, expr, &out);
    if(suc == FAIL) {
        DBG(1, "Error evaluating expression\n");
        return NULL;
    }

    DBG(1, "*********************************\n");
    print_value(&out);
    DBG(1, "*********************************\n");   

    // Skip comments && new lines
    while((_token = _read_token(p)) != NULL && (_token->type == COMMENT || _token->type == NEWLINE));
    if(_token == NULL) return NULL;

    DBG(1, "DONE SKIPING\n");

    if(is_at_identifier(_token, IF_IDENTIDIER) == SUCCESS) DBG(1, "FOUND AN IF\n");
    else if (is_at_identifier(_token, INCLUDE_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN INCLUDE\n");
    else if (is_at_identifier(_token, EXTERN_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN EXTER\n");
    else if (is_at_identifier(_token, EXPORT_IDENTIFIER) == SUCCESS) DBG(1, "FOUND AN EXPORT\n");

    while((_token = _read_token(p)) != NULL && _token->type != ENDMARKER);
    return NULL;
}