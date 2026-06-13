#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "types.h"
#include "helper.h"
#include "token.h"
#include "eval.h"

#include "api/debug.h"

extern const char * const _Parser_TokenNames[];

// Token stream
void tks_init(struct TokenStream* tks, struct token* start, struct token* end) {
    tks->start = start;
    tks->end = end;

    tks->peek = start;
    tks->read = start;
}

struct token* tks_read(struct TokenStream* tks) {
    struct token* tok = tks->read;

    if (!tok) return NULL;

    if (tok == tks->end) {
        tks->read = NULL;
        tks->peek = NULL;
        return tok;
    }

    tks->read = tok->next;
    tks->peek = tks->read;

    return tok;
}

struct token* tks_peek(struct TokenStream* tks) {
    struct token* tok = tks->peek;

    if (!tok) return NULL;

    if (tok == tks->end) tks->peek = NULL;
    else tks->peek = tok->next;

    return tok;
}

void tks_reset_peek(struct TokenStream* tks) {
    tks->peek = tks->read; 
}


unint compare_identifiers_cp_array(struct token* ident1, int32_t* cmp) {
    if (!ident1 || !cmp) return FAIL;

    int32_t* a = ident1->cps;
    unint alen = ident1->len;

    unint i = 0;

    while (1) {
        int32_t b = cmp[i];
        if (b == -1) break;

        if (i >= alen) return FAIL;

        if (a[i] != b) return FAIL;

        i++;
    }

    if (i != alen) return FAIL;

    return SUCCESS;
}

unint parse_potential_register(struct TokenStream* tks, int32_t** registers, unint* idx) {
    if(tks->start != tks->end) return FAIL; // Registers are only one token
    
    struct token* name = tks_peek(tks);
    if ((name == NULL) || (name->type != NAME)) return FAIL;

    // make sure there isnt a struct access next to it
    struct token* dot = tks_peek(tks);
    if ((dot != NULL) && (dot->type == DOT)) return FAIL;

    // Search for the register
    for (unint i = 0; registers[i]; ++i) {
        if (compare_identifiers_cp_array(name, registers[i]) == SUCCESS) {
            *idx = i;
            tks_read(tks);
            return SUCCESS;
        }
    }
    return FAIL;
}

unint parse_potential_variable(struct Parser* p, struct TokenStream* tks, nint* addr, unint permissive) {
    struct token* name = tks_peek(tks);
    if (!name || name->type != NAME) return VP_NONE;

    // Variable
    struct Variable* var = get_variable(p, name);
    if(var) {
        if(var->val.type != VALUE_INT) {
            _error_from_token(p, name, ERROR_TYPE_TYPE, "invalid type");
            return VP_FAIL;
        }
        *addr = var->val.val.number;
        return VP_SUCCESS;  
    }

    /* ---------------- resolve root ---------------- */
    struct LabelDecl* sym = NULL;
    for (struct LabelDecl* it = p->global_label_decl; it != NULL; it = it->next){
        if (_compare_identifiers(it->name->cps, it->name->len, name->cps, name->len) == SUCCESS) {
            sym = it;
            break;
        }

        if (it == p->global_label_decl_tail) break;
    }

    // Not a label nor a struct var
    if (!sym && (tks_peek(tks) == NULL)) {
        tks_read(tks);
        return VP_UNRESOLVED_LABEL;
    }

    // Not a variable / struct field
    if(!sym) {
        tks_read(tks);
        struct token* _err = tks_read(tks);
        if(_err) _error_from_token(p, _err, ERROR_TYPE_NAME, "unexpected token");
        return VP_FAIL;
    }

    tks_read(tks);

    nint base_addr = sym->addr;
    nint current_addr = base_addr;

    struct LabelDecl* current_struct = sym;

    while(true) {

        struct token* tok = tks_peek(tks);
        if (tok == NULL) break;

        if (tok->type == DOT) {
            tks_read(tks); // '.'

            struct token* field = tks_peek(tks);
            if (!field || field->type != NAME) {
                _error_from_token(p, tok, ERROR_TYPE_NAME, "expected struct field after '.'");
                return VP_FAIL;
            }

            tks_read(tks); // field name

            if (!current_struct->deep_head) {
                _error_from_token(p, field, ERROR_TYPE_TYPE, "not a struct type");
                return VP_FAIL;
            }

            struct LabelDecl* member = current_struct->deep_head;
            struct LabelDecl* found = NULL;

            while (member) {

                if (_compare_identifiers(member->name->cps, member->name->len, field->cps, field->len) == SUCCESS) {
                    found = member;
                    break;
                }

                if (member == current_struct->deep_tail) break;
                member = member->next;
            }

            if (!found) {
                _error_from_token(p, field, ERROR_TYPE_NAME, "unknown struct field");
                return VP_FAIL;
            }

            // Address calculation
            nint field_offset = found->addr - current_struct->addr;
            current_addr += field_offset;
            current_struct = found;

            // Optional indexing
            struct token* next = tks_peek(tks);
            if (next && next->type == LSQB) {
                tks_read(tks); // [

                struct token* num = tks_peek(tks);
                if (expected_token(p, num, NUMBER) == NULL)
                    return VP_FAIL;

                struct Value* val = new_number(p, num, 0);
                if (!val)
                    return VP_FAIL;

                if (val->type != VALUE_INT) {
                    _error_from_token(p, num, ERROR_TYPE_TYPE, "invalid index type");
                    return VP_FAIL;
                }

                if (val->val.number >= found->len) {
                    _error_from_token(p, num, ERROR_TYPE_INDEX_ERROR, "index out of range");
                    return VP_FAIL;
                }

                tks_read(tks); // number

                struct token* rsqb = tks_read(tks);
                if(!rsqb) {
                    _error_from_token(p, num, ERROR_TYPE_MESSAGE, "argument ended witout closing ']'");
                    return VP_FAIL;
                } 
                else if (expected_token(p, rsqb, RSQB) == NULL) return VP_FAIL;

                current_addr += val->val.number * found->stride;
            } else tks_reset_peek(tks);


            continue;
        }

        break;
    }
    tks_reset_peek(tks);

    if(!permissive) {
        // Trailling tokens
        struct token* trailling = tks_peek(tks);
        if (trailling != NULL) {
            _error_from_token(
                p,
                trailling,
                ERROR_TYPE_NAME,
                "unexpected token");
            return VP_FAIL;
        }
    }

    *addr = current_addr;
    return VP_SUCCESS;
}

nint unresolved_label(struct Parser* p, struct token* _token) {
    _error_from_token(p, _token, ERROR_TYPE_NAME, "unresolved label");
    return INSTRUCTION_FAILED;
}