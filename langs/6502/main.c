#include <stdio.h>


#include "asm_lang.h"
#include "../../src/core/helper.h"
#include "../../src/core/token.h"
#include "../../src/core/variables.h"
#include "types.h"

#include "api/debug.h"
#include "api/file.h"

#define LDA_INSTRUCTION ((int32_t[]){'l', 'd', 'a', -1})
#define LDX_INSTRUCTION ((int32_t[]){'l', 'd', 'x', -1})
#define LDY_INSTRUCTION ((int32_t[]){'l', 'd', 'y', -1})
#define STA_INSTRUCTION ((int32_t[]){'s', 't', 'a', -1})
#define STX_INSTRUCTION ((int32_t[]){'s', 't', 'x', -1})
#define STY_INSTRUCTION ((int32_t[]){'s', 't', 'y', -1})
#define TAX_INSTRUCTION ((int32_t[]){'t', 'a', 'x', -1})
#define TAY_INSTRUCTION ((int32_t[]){'t', 'a', 'y', -1})
#define TSX_INSTRUCTION ((int32_t[]){'t', 's', 'x', -1})
#define TXA_INSTRUCTION ((int32_t[]){'t', 'x', 'a', -1})
#define TXS_INSTRUCTION ((int32_t[]){'t', 'x', 's', -1})
#define TYA_INSTRUCTION ((int32_t[]){'t', 'y', 'a', -1})
#define SEI_INSTRUCTION ((int32_t[]){'s', 'e', 'i', -1})
#define SEC_INSTRUCTION ((int32_t[]){'s', 'e', 'c', -1})
#define SED_INSTRUCTION ((int32_t[]){'s', 'e', 'd', -1})
#define SBC_INSTRUCTION ((int32_t[]){'s', 'b', 'c', -1})
#define RTS_INSTRUCTION ((int32_t[]){'r', 't', 's', -1})
#define RTI_INSTRUCTION ((int32_t[]){'r', 't', 'i', -1})
#define ROL_INSTRUCTION ((int32_t[]){'r', 'o', 'l', -1})
#define ROR_INSTRUCTION ((int32_t[]){'r', 'o', 'r', -1})
#define PLP_INSTRUCTION ((int32_t[]){'p', 'l', 'p', -1})
#define PLA_INSTRUCTION ((int32_t[]){'p', 'l', 'a', -1})
#define PHP_INSTRUCTION ((int32_t[]){'p', 'h', 'p', -1})
#define PHA_INSTRUCTION ((int32_t[]){'p', 'h', 'a', -1})
#define ORA_INSTRUCTION ((int32_t[]){'o', 'r', 'a', -1})
#define NOP_INSTRUCTION ((int32_t[]){'n', 'o', 'p', -1})
#define LSR_INSTRUCTION ((int32_t[]){'l', 's', 'r', -1})
#define JSR_INSTRUCTION ((int32_t[]){'j', 's', 'r', -1})
#define JMP_INSTRUCTION ((int32_t[]){'j', 'm', 'p', -1})
#define INY_INSTRUCTION ((int32_t[]){'i', 'n', 'y', -1})
#define INX_INSTRUCTION ((int32_t[]){'i', 'n', 'x', -1})
#define DEC_INSTRUCTION ((int32_t[]){'d', 'e', 'c', -1})
#define DEX_INSTRUCTION ((int32_t[]){'d', 'e', 'x', -1})
#define DEY_INSTRUCTION ((int32_t[]){'d', 'e', 'y', -1})
#define EOR_INSTRUCTION ((int32_t[]){'e', 'o', 'r', -1})
#define INC_INSTRUCTION ((int32_t[]){'i', 'n', 'c', -1})
#define CLC_INSTRUCTION ((int32_t[]){'c', 'l', 'c', -1})
#define CLD_INSTRUCTION ((int32_t[]){'c', 'l', 'd', -1})
#define CLI_INSTRUCTION ((int32_t[]){'c', 'l', 'i', -1})
#define CLV_INSTRUCTION ((int32_t[]){'c', 'l', 'v', -1})
#define CMP_INSTRUCTION ((int32_t[]){'c', 'm', 'p', -1})
#define CPX_INSTRUCTION ((int32_t[]){'c', 'p', 'x', -1})
#define CPY_INSTRUCTION ((int32_t[]){'c', 'p', 'y', -1})
#define BNE_INSTRUCTION ((int32_t[]){'b', 'n', 'e', -1})
#define BPL_INSTRUCTION ((int32_t[]){'b', 'p', 'l', -1})
#define BRK_INSTRUCTION ((int32_t[]){'b', 'r', 'k', -1})
#define BVC_INSTRUCTION ((int32_t[]){'b', 'v', 'c', -1})
#define BVS_INSTRUCTION ((int32_t[]){'b', 'v', 's', -1})
#define ADC_INSTRUCTION ((int32_t[]){'a', 'd', 'c', -1})
#define AND_INSTRUCTION ((int32_t[]){'a', 'n', 'd', -1})
#define ASL_INSTRUCTION ((int32_t[]){'a', 's', 'l', -1})
#define BCC_INSTRUCTION ((int32_t[]){'b', 'c', 'c', -1})
#define BCS_INSTRUCTION ((int32_t[]){'b', 'c', 's', -1})
#define BEQ_INSTRUCTION ((int32_t[]){'b', 'e', 'q', -1})
#define BIT_INSTRUCTION ((int32_t[]){'b', 'i', 't', -1})
#define BMI_INSTRUCTION ((int32_t[]){'b', 'm', 'i', -1})

#define A_REGISTER ((int32_t[]){'a', -1})
#define X_REGISTER ((int32_t[]){'x', -1})
#define Y_REGISTER ((int32_t[]){'y', -1})

#define ERROR_TYPE_6502 "6502Error"

nint _6502_exec(struct Parser* p, struct AstInstruction* inst);

ASM_LANG _6502 = {
    .lang_name = "6502 assembly v1.0.0",
    .code_name = "_6502",
    .max_addr = 0Xffff,
    .size_of_ptr = 2,
    .le = 1,

    .exec = _6502_exec,
};

enum SelectorKind {
    SELECT_NONE,
    SELECT_LOW,
    SELECT_HIGH
};

struct Scalar {
    nint value;
    unint unresolved;
    enum SelectorKind selector;
    struct token *token;
};
static void emit_byte(struct Parser *p, unsigned char byte) {
    (void)p;
    if(p->last_pass) OUT_FILE_WRITE_BYTE(byte);
}

static void emit_word(struct Parser *p, nint value) {
    emit_byte(p, (unsigned char)(value & 0xff));
    emit_byte(p, (unsigned char)((value >> 8) & 0xff));
}

static nint emit_implied(struct Parser *p, struct AstInstruction *inst,
                         unsigned char opcode, const char *mnemonic) {
    if(inst->arg_count != 0) {
        _error_from_token(p, inst->args_head ? inst->args_head->_s : inst->name,
                          ERROR_TYPE_6502, "invalid %s operand", mnemonic);
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    return 1;
}

static unint token_is(struct token *tok, int32_t *name) {
    return tok && tok->type == NAME &&
           compare_identifiers_cp_array(tok, name) == SUCCESS;
}

static void record_error_token(struct token **error_token, struct token *tok) {
    if(error_token && tok && (!*error_token || tok > *error_token)) {
        *error_token = tok;
    }
}

static unint tks_at_end(struct TokenStream *tks) {
    struct token *tok = tks_peek(tks);
    tks_reset_peek(tks);
    return tok == NULL;
}

static unint fits_u8(nint value) {
    return value >= 0 && value <= 0xff;
}

static unint fits_i8_or_u8(nint value) {
    return value >= -128 && value <= 0xff;
}

static unint fits_u16(nint value) {
    return value >= 0 && value <= 0xffff;
}

static nint selected_value(struct Scalar *scalar) {
    if(scalar->selector == SELECT_LOW) return scalar->value & 0xff;
    if(scalar->selector == SELECT_HIGH) return (scalar->value >> 8) & 0xff;
    return scalar->value;
}

static nint read_number(struct Parser *p, struct TokenStream *tks, nint *value,
                        struct token **error_token) {
    struct token *tok = tks_read(tks);
    unint is_neg = 0;
    struct Value *v;

    if(tok && tok->type == MINUS) {
        is_neg = 1;
        record_error_token(error_token, tok);
        tok = tks_read(tks);
    }

    if(!tok || tok->type != NUMBER) return 0;
    record_error_token(error_token, tok);

    v = new_number(p, tok, is_neg);
    if(!v) return 0;
    if(v->type != VALUE_INT) {
        _error_from_token(p, tok, ERROR_TYPE_TYPE, "invalid type");
        return INSTRUCTION_FAILED;
    }

    *value = v->val.number;
    return 1;
}

static nint read_scalar(struct Parser *p, struct TokenStream *tks,
                        struct Scalar *scalar, struct token **error_token) {
    struct token *tok = tks_peek(tks);
    unint status;
    nint number;
    tks_reset_peek(tks);

    scalar->value = 0;
    scalar->unresolved = 0;
    scalar->selector = SELECT_NONE;
    scalar->token = tok;
    if(!tok) return 0;

    if(tok->type == LESS || tok->type == GREATER) {
        tks_read(tks);
        record_error_token(error_token, tok);
        scalar->selector = tok->type == LESS ? SELECT_LOW : SELECT_HIGH;
        tok = tks_peek(tks);
        tks_reset_peek(tks);
        scalar->token = tok;
    }

    if(tok && tok->type == NAME) {
        status = parse_potential_variable(p, tks, &scalar->value, 1);
        if(status == VP_SUCCESS) {
            record_error_token(error_token, tok);
            return 1;
        }
        if(status == VP_UNRESOLVED_LABEL) {
            record_error_token(error_token, tok);
            scalar->unresolved = 1;
            scalar->value = 0;
            if(p->last_pass) return unresolved_label(p, tok);
            return 1;
        }
        if(status == VP_FAIL) return INSTRUCTION_FAILED;
        return 0;
    }

    if(read_number(p, tks, &number, error_token) > 0) {
        scalar->value = number;
        return 1;
    }

    return 0;
}

static nint read_scalar_arg(struct Parser *p, struct InstructionArg *arg,
                            struct Scalar *scalar, struct token **error_token) {
    struct TokenStream tks;
    nint status;

    tks_init(&tks, arg->_s, arg->_e);
    status = read_scalar(p, &tks, scalar, error_token);
    if(status <= 0) return status;
    if(!tks_at_end(&tks)) {
        record_error_token(error_token, tks.read);
        return 0;
    }
    return 1;
}

static nint read_immediate_arg(struct Parser *p, struct InstructionArg *arg,
                               struct Scalar *scalar, struct token **error_token) {
    struct TokenStream tks;
    struct token *tok;
    nint status;

    tks_init(&tks, arg->_s, arg->_e);
    tok = tks_read(&tks);
    if(!expected_token(p, tok, EXCLAMATION)) {
        record_error_token(error_token, tok ? tok : arg->_s);
        return 0;
    }
    record_error_token(error_token, tok);

    status = read_scalar(p, &tks, scalar, error_token);
    if(status <= 0) return status;
    if(!tks_at_end(&tks)) {
        record_error_token(error_token, tks.read);
        return 0;
    }
    return 1;
}

static nint read_parenthesized_scalar(struct Parser *p, struct InstructionArg *arg,
                                      struct Scalar *scalar,
                                      struct token **error_token) {
    struct TokenStream tks;
    struct token *tok;
    nint status;

    tks_init(&tks, arg->_s, arg->_e);
    tok = tks_read(&tks);
    if(!expected_token(p, tok, LPAR)) {
        record_error_token(error_token, tok ? tok : arg->_s);
        return 0;
    }
    record_error_token(error_token, tok);

    status = read_scalar(p, &tks, scalar, error_token);
    if(status <= 0) return status;

    tok = tks_read(&tks);
    if(!expected_token(p, tok, RPAR)) {
        record_error_token(error_token, tok ? tok : arg->_e);
        return 0;
    }
    record_error_token(error_token, tok);

    if(!tks_at_end(&tks)) {
        record_error_token(error_token, tks.read);
        return 0;
    }
    return 1;
}

static nint read_open_parenthesized_scalar(struct Parser *p, struct InstructionArg *arg,
                                           struct Scalar *scalar,
                                           struct token **error_token) {
    struct TokenStream tks;
    struct token *tok;
    nint status;

    tks_init(&tks, arg->_s, arg->_e);
    tok = tks_read(&tks);
    if(!expected_token(p, tok, LPAR)) {
        record_error_token(error_token, tok ? tok : arg->_s);
        return 0;
    }
    record_error_token(error_token, tok);

    status = read_scalar(p, &tks, scalar, error_token);
    if(status <= 0) return status;
    if(!tks_at_end(&tks)) {
        record_error_token(error_token, tks.read);
        return 0;
    }
    return 1;
}

static nint read_index_arg(struct InstructionArg *arg, int32_t *name,
                           unint allow_rpar, struct token **error_token) {
    struct token *tok;

    if(!arg || !arg->_s || !arg->_e) return 0;
    tok = arg->_s;
    if(!token_is(tok, name)) {
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);

    if(tok == arg->_e) return 1;
    if(allow_rpar && tok + 1 == arg->_e && (tok + 1)->type == RPAR) {
        record_error_token(error_token, tok + 1);
        return 1;
    }
    record_error_token(error_token, tok + 1);
    return 0;
}

static nint read_accumulator_arg(struct InstructionArg *arg,
                                 struct token **error_token) {
    struct token *tok;

    if(!arg || !arg->_s || !arg->_e) return 0;
    tok = arg->_s;
    if(token_is(tok, A_REGISTER) && tok == arg->_e) {
        record_error_token(error_token, tok);
        return 1;
    }

    record_error_token(error_token, tok);
    return 0;
}

static nint emit_imm8(struct Parser *p, struct AstInstruction *inst,
                      unsigned char opcode, const char *mnemonic) {
    struct Scalar scalar;
    struct token *error_token = inst->args_head->_s;
    nint status = read_immediate_arg(p, inst->args_head, &scalar, &error_token);
    nint value;
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_6502, "invalid %s immediate",
                          mnemonic);
        return INSTRUCTION_FAILED;
    }

    value = selected_value(&scalar);
    if(!scalar.unresolved && scalar.selector == SELECT_NONE && !fits_i8_or_u8(value)) {
        _error_from_token(p, scalar.token, ERROR_TYPE_OVERFLOW,
                          "immediate does not fit 8 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(value & 0xff));
    return 2;
}

static nint emit_memory_operand(struct Parser *p, struct AstInstruction *inst,
                                unsigned char zp_opcode, unsigned char abs_opcode,
                                unint allow_zp, const char *mnemonic) {
    struct Scalar scalar;
    struct token *error_token = inst->args_head->_s;
    nint status = read_scalar_arg(p, inst->args_head, &scalar, &error_token);
    nint value;
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_6502, "invalid %s operand",
                          mnemonic);
        return INSTRUCTION_FAILED;
    }

    value = selected_value(&scalar);
    if(scalar.selector == SELECT_LOW && !allow_zp) {
        _error_from_token(p, scalar.token, ERROR_TYPE_6502,
                          "invalid zero page addressing mode");
        return INSTRUCTION_FAILED;
    }

    if(scalar.selector == SELECT_LOW) {
        emit_byte(p, zp_opcode);
        emit_byte(p, (unsigned char)(value & 0xff));
        return 2;
    }

    if(scalar.selector == SELECT_HIGH) {
        _error_from_token(p, scalar.token, ERROR_TYPE_6502,
                          "invalid high-byte address selector");
        return INSTRUCTION_FAILED;
    }

    if(scalar.unresolved || (scalar.token && scalar.token->type == NAME)) {
        emit_byte(p, abs_opcode);
        emit_word(p, value);
        return 3;
    }

    if(allow_zp && fits_u8(value)) {
        emit_byte(p, zp_opcode);
        emit_byte(p, (unsigned char)(value & 0xff));
        return 2;
    }

    if(!fits_u16(value)) {
        _error_from_token(p, scalar.token, ERROR_TYPE_OVERFLOW,
                          "address does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, abs_opcode);
    emit_word(p, value);
    return 3;
}

static nint emit_zp_operand(struct Parser *p, struct AstInstruction *inst,
                            unsigned char opcode, const char *mnemonic) {
    struct Scalar scalar;
    struct token *error_token = inst->args_head->_s;
    nint status = read_scalar_arg(p, inst->args_head, &scalar, &error_token);
    nint value;
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_6502, "invalid %s operand",
                          mnemonic);
        return INSTRUCTION_FAILED;
    }

    value = selected_value(&scalar);
    if(scalar.selector == SELECT_HIGH) {
        _error_from_token(p, scalar.token, ERROR_TYPE_6502,
                          "invalid high-byte address selector");
        return INSTRUCTION_FAILED;
    }

    if(scalar.selector != SELECT_LOW &&
       (scalar.unresolved || (scalar.token && scalar.token->type == NAME))) {
        _error_from_token(p, scalar.token, ERROR_TYPE_6502,
                          "invalid zero page addressing mode");
        return INSTRUCTION_FAILED;
    }

    if(scalar.selector == SELECT_NONE && !fits_u8(value)) {
        _error_from_token(p, scalar.token, ERROR_TYPE_OVERFLOW,
                          "address does not fit 8 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(value & 0xff));
    return 2;
}

static nint emit_absolute_arg(struct Parser *p, struct InstructionArg *arg,
                              unsigned char opcode, const char *mnemonic,
                              unint parenthesized) {
    struct Scalar scalar;
    struct token *error_token = arg->_s;
    nint status = parenthesized ?
                  read_parenthesized_scalar(p, arg, &scalar, &error_token) :
                  read_scalar_arg(p, arg, &scalar, &error_token);
    nint value;
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_6502, "invalid %s operand",
                          mnemonic);
        return INSTRUCTION_FAILED;
    }

    if(scalar.selector != SELECT_NONE) {
        _error_from_token(p, scalar.token, ERROR_TYPE_6502,
                          "invalid address selector");
        return INSTRUCTION_FAILED;
    }

    value = selected_value(&scalar);
    if(!scalar.unresolved && !fits_u16(value)) {
        _error_from_token(p, scalar.token, ERROR_TYPE_OVERFLOW,
                          "address does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_word(p, value);
    return 3;
}

static nint emit_relative_arg(struct Parser *p, struct AstInstruction *inst,
                              unsigned char opcode, const char *mnemonic) {
    struct Scalar scalar;
    struct token *error_token;
    nint status;
    nint target;
    nint offset;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    error_token = inst->args_head->_s;
    status = read_scalar_arg(p, inst->args_head, &scalar, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_6502, "invalid %s operand",
                          mnemonic);
        return INSTRUCTION_FAILED;
    }

    if(scalar.selector != SELECT_NONE) {
        _error_from_token(p, scalar.token, ERROR_TYPE_6502,
                          "invalid address selector");
        return INSTRUCTION_FAILED;
    }

    target = selected_value(&scalar);
    offset = scalar.unresolved ? 0 : target - (p->addr + 2);
    if(!scalar.unresolved && (offset < -128 || offset > 127)) {
        _error_from_token(p, scalar.token, ERROR_TYPE_OVERFLOW,
                          "branch target out of range");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(offset & 0xff));
    return 2;
}

static nint emit_zp_indirect(struct Parser *p, struct AstInstruction *inst,
                             unsigned char opcode, unint indexed_indirect,
                             const char *mnemonic) {
    struct Scalar scalar;
    struct token *error_token = inst->args_head->_s;
    nint status = indexed_indirect ?
                  read_open_parenthesized_scalar(p, inst->args_head, &scalar,
                                                 &error_token) :
                  read_parenthesized_scalar(p, inst->args_head, &scalar,
                                            &error_token);
    nint value;
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_6502, "invalid %s operand",
                          mnemonic);
        return INSTRUCTION_FAILED;
    }

    value = selected_value(&scalar);
    if(!scalar.unresolved && scalar.selector == SELECT_NONE && !fits_u8(value)) {
        _error_from_token(p, scalar.token, ERROR_TYPE_OVERFLOW,
                          "indirect operand does not fit zero page");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(value & 0xff));
    return 2;
}

static nint encode_lda(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0xa9, "lda");
        }
        return emit_memory_operand(p, inst, 0xa5, 0xad, 1, "lda");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0xa1, 1, "lda");
            }
            return emit_memory_operand(p, inst, 0xb5, 0xbd, 1, "lda");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0xb1, 0, "lda");
            }
            return emit_memory_operand(p, inst, 0xb9, 0xb9, 0, "lda");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid lda index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_ldx(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0xa2, "ldx");
        }
        return emit_memory_operand(p, inst, 0xa6, 0xae, 1, "ldx");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            return emit_memory_operand(p, inst, 0xb6, 0xbe, 1, "ldx");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid ldx index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_ldy(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0xa0, "ldy");
        }
        return emit_memory_operand(p, inst, 0xa4, 0xac, 1, "ldy");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 0, &error_token)) {
            return emit_memory_operand(p, inst, 0xb4, 0xbc, 1, "ldy");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid ldy index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_sta(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        return emit_memory_operand(p, inst, 0x85, 0x8d, 1, "sta");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0x81, 1, "sta");
            }
            return emit_memory_operand(p, inst, 0x95, 0x9d, 1, "sta");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0x91, 0, "sta");
            }
            return emit_memory_operand(p, inst, 0x99, 0x99, 0, "sta");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid sta index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_stx(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        return emit_memory_operand(p, inst, 0x86, 0x8e, 1, "stx");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            return emit_zp_operand(p, inst, 0x96, "stx");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid stx index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_sty(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        return emit_memory_operand(p, inst, 0x84, 0x8c, 1, "sty");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 0, &error_token)) {
            return emit_zp_operand(p, inst, 0x94, "sty");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid sty index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_tax(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xaa, "tax");
}

static nint encode_tay(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xa8, "tay");
}

static nint encode_tsx(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xba, "tsx");
}

static nint encode_txa(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x8a, "txa");
}

static nint encode_txs(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x9a, "txs");
}

static nint encode_tya(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x98, "tya");
}

static nint encode_sei(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x78, "sei");
}

static nint encode_sec(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x38, "sec");
}

static nint encode_sed(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xf8, "sed");
}

static nint encode_sbc(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0xe9, "sbc");
        }
        return emit_memory_operand(p, inst, 0xe5, 0xed, 1, "sbc");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0xe1, 1, "sbc");
            }
            return emit_memory_operand(p, inst, 0xf5, 0xfd, 1, "sbc");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0xf1, 0, "sbc");
            }
            return emit_memory_operand(p, inst, 0xf9, 0xf9, 0, "sbc");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid sbc index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_ora(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0x09, "ora");
        }
        return emit_memory_operand(p, inst, 0x05, 0x0d, 1, "ora");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0x01, 1, "ora");
            }
            return emit_memory_operand(p, inst, 0x15, 0x1d, 1, "ora");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0x11, 0, "ora");
            }
            return emit_memory_operand(p, inst, 0x19, 0x19, 0, "ora");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid ora index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_eor(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0x49, "eor");
        }
        return emit_memory_operand(p, inst, 0x45, 0x4d, 1, "eor");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0x41, 1, "eor");
            }
            return emit_memory_operand(p, inst, 0x55, 0x5d, 1, "eor");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0x51, 0, "eor");
            }
            return emit_memory_operand(p, inst, 0x59, 0x59, 0, "eor");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid eor index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_cmp(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0xc9, "cmp");
        }
        return emit_memory_operand(p, inst, 0xc5, 0xcd, 1, "cmp");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0xc1, 1, "cmp");
            }
            return emit_memory_operand(p, inst, 0xd5, 0xdd, 1, "cmp");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0xd1, 0, "cmp");
            }
            return emit_memory_operand(p, inst, 0xd9, 0xd9, 0, "cmp");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid cmp index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_adc(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0x69, "adc");
        }
        return emit_memory_operand(p, inst, 0x65, 0x6d, 1, "adc");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0x61, 1, "adc");
            }
            return emit_memory_operand(p, inst, 0x75, 0x7d, 1, "adc");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0x71, 0, "adc");
            }
            return emit_memory_operand(p, inst, 0x79, 0x79, 0, "adc");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid adc index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_and(struct Parser *p, struct AstInstruction *inst) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
            return emit_imm8(p, inst, 0x29, "and");
        }
        return emit_memory_operand(p, inst, 0x25, 0x2d, 1, "and");
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 1, &error_token)) {
            if(inst->args_tail->_e->type == RPAR) {
                return emit_zp_indirect(p, inst, 0x21, 1, "and");
            }
            return emit_memory_operand(p, inst, 0x35, 0x3d, 1, "and");
        }

        error_token = NULL;
        if(read_index_arg(inst->args_tail, Y_REGISTER, 0, &error_token)) {
            if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
                return emit_zp_indirect(p, inst, 0x31, 0, "and");
            }
            return emit_memory_operand(p, inst, 0x39, 0x39, 0, "and");
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid and index");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_immediate_memory(struct Parser *p, struct AstInstruction *inst,
                                    unsigned char imm_opcode,
                                    unsigned char zp_opcode,
                                    unsigned char abs_opcode,
                                    const char *mnemonic) {
    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(inst->args_head->_s && inst->args_head->_s->type == EXCLAMATION) {
        return emit_imm8(p, inst, imm_opcode, mnemonic);
    }

    return emit_memory_operand(p, inst, zp_opcode, abs_opcode, 1, mnemonic);
}

static nint encode_accumulator_memory_x(struct Parser *p,
                                        struct AstInstruction *inst,
                                        unsigned char acc_opcode,
                                        unsigned char zp_opcode,
                                        unsigned char abs_opcode,
                                        unsigned char zpx_opcode,
                                        unsigned char absx_opcode,
                                        const char *mnemonic) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        if(read_accumulator_arg(inst->args_head, &error_token)) {
            emit_byte(p, acc_opcode);
            return 1;
        }
        return emit_memory_operand(p, inst, zp_opcode, abs_opcode, 1, mnemonic);
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 0, &error_token)) {
            return emit_memory_operand(p, inst, zpx_opcode, absx_opcode, 1,
                                       mnemonic);
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid %s index", mnemonic);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_memory_x(struct Parser *p, struct AstInstruction *inst,
                            unsigned char zp_opcode, unsigned char abs_opcode,
                            unsigned char zpx_opcode, unsigned char absx_opcode,
                            const char *mnemonic) {
    struct token *error_token = NULL;
    if(inst->arg_count == 1) {
        return emit_memory_operand(p, inst, zp_opcode, abs_opcode, 1, mnemonic);
    }

    if(inst->arg_count == 2) {
        if(read_index_arg(inst->args_tail, X_REGISTER, 0, &error_token)) {
            return emit_memory_operand(p, inst, zpx_opcode, absx_opcode, 1,
                                       mnemonic);
        }

        _error_from_token(p, error_token ? error_token : inst->args_tail->_s,
                          ERROR_TYPE_6502, "invalid %s index", mnemonic);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
    return INSTRUCTION_FAILED;
}

static nint encode_rts(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x60, "rts");
}

static nint encode_rti(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x40, "rti");
}

static nint encode_rol(struct Parser *p, struct AstInstruction *inst) {
    return encode_accumulator_memory_x(p, inst, 0x2a, 0x26, 0x2e, 0x36, 0x3e,
                                       "rol");
}

static nint encode_ror(struct Parser *p, struct AstInstruction *inst) {
    return encode_accumulator_memory_x(p, inst, 0x6a, 0x66, 0x6e, 0x76, 0x7e,
                                       "ror");
}

static nint encode_lsr(struct Parser *p, struct AstInstruction *inst) {
    return encode_accumulator_memory_x(p, inst, 0x4a, 0x46, 0x4e, 0x56, 0x5e,
                                       "lsr");
}

static nint encode_asl(struct Parser *p, struct AstInstruction *inst) {
    return encode_accumulator_memory_x(p, inst, 0x0a, 0x06, 0x0e, 0x16, 0x1e,
                                       "asl");
}

static nint encode_plp(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x28, "plp");
}

static nint encode_pla(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x68, "pla");
}

static nint encode_php(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x08, "php");
}

static nint encode_pha(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x48, "pha");
}

static nint encode_nop(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xea, "nop");
}

static nint encode_jsr(struct Parser *p, struct AstInstruction *inst) {
    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }
    return emit_absolute_arg(p, inst->args_head, 0x20, "jsr", 0);
}

static nint encode_jmp(struct Parser *p, struct AstInstruction *inst) {
    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(inst->args_head->_s && inst->args_head->_s->type == LPAR) {
        return emit_absolute_arg(p, inst->args_head, 0x6c, "jmp", 1);
    }

    return emit_absolute_arg(p, inst->args_head, 0x4c, "jmp", 0);
}

static nint encode_iny(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xc8, "iny");
}

static nint encode_inx(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xe8, "inx");
}

static nint encode_dec(struct Parser *p, struct AstInstruction *inst) {
    return encode_memory_x(p, inst, 0xc6, 0xce, 0xd6, 0xde, "dec");
}

static nint encode_dex(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xca, "dex");
}

static nint encode_dey(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x88, "dey");
}

static nint encode_inc(struct Parser *p, struct AstInstruction *inst) {
    return encode_memory_x(p, inst, 0xe6, 0xee, 0xf6, 0xfe, "inc");
}

static nint encode_clc(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x18, "clc");
}

static nint encode_cld(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xd8, "cld");
}

static nint encode_cli(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x58, "cli");
}

static nint encode_clv(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0xb8, "clv");
}

static nint encode_cpx(struct Parser *p, struct AstInstruction *inst) {
    return encode_immediate_memory(p, inst, 0xe0, 0xe4, 0xec, "cpx");
}

static nint encode_cpy(struct Parser *p, struct AstInstruction *inst) {
    return encode_immediate_memory(p, inst, 0xc0, 0xc4, 0xcc, "cpy");
}

static nint encode_bne(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0xd0, "bne");
}

static nint encode_bpl(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0x10, "bpl");
}

static nint encode_brk(struct Parser *p, struct AstInstruction *inst) {
    return emit_implied(p, inst, 0x00, "brk");
}

static nint encode_bvc(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0x50, "bvc");
}

static nint encode_bvs(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0x70, "bvs");
}

static nint encode_bcc(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0x90, "bcc");
}

static nint encode_bcs(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0xb0, "bcs");
}

static nint encode_beq(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0xf0, "beq");
}

static nint encode_bit(struct Parser *p, struct AstInstruction *inst) {
    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    return emit_memory_operand(p, inst, 0x24, 0x2c, 1, "bit");
}

static nint encode_bmi(struct Parser *p, struct AstInstruction *inst) {
    return emit_relative_arg(p, inst, 0x30, "bmi");
}

nint _6502_exec(struct Parser *p, struct AstInstruction *inst) {
    if(compare_identifiers_cp_array(inst->name, LDA_INSTRUCTION) == SUCCESS) {
        return encode_lda(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, LDX_INSTRUCTION) == SUCCESS) {
        return encode_ldx(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, LDY_INSTRUCTION) == SUCCESS) {
        return encode_ldy(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, STA_INSTRUCTION) == SUCCESS) {
        return encode_sta(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, STX_INSTRUCTION) == SUCCESS) {
        return encode_stx(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, STY_INSTRUCTION) == SUCCESS) {
        return encode_sty(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, TAX_INSTRUCTION) == SUCCESS) {
        return encode_tax(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, TAY_INSTRUCTION) == SUCCESS) {
        return encode_tay(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, TSX_INSTRUCTION) == SUCCESS) {
        return encode_tsx(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, TXA_INSTRUCTION) == SUCCESS) {
        return encode_txa(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, TXS_INSTRUCTION) == SUCCESS) {
        return encode_txs(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, TYA_INSTRUCTION) == SUCCESS) {
        return encode_tya(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, SEI_INSTRUCTION) == SUCCESS) {
        return encode_sei(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, SEC_INSTRUCTION) == SUCCESS) {
        return encode_sec(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, SED_INSTRUCTION) == SUCCESS) {
        return encode_sed(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, SBC_INSTRUCTION) == SUCCESS) {
        return encode_sbc(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, RTS_INSTRUCTION) == SUCCESS) {
        return encode_rts(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, RTI_INSTRUCTION) == SUCCESS) {
        return encode_rti(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, ROL_INSTRUCTION) == SUCCESS) {
        return encode_rol(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, ROR_INSTRUCTION) == SUCCESS) {
        return encode_ror(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, PLP_INSTRUCTION) == SUCCESS) {
        return encode_plp(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, PLA_INSTRUCTION) == SUCCESS) {
        return encode_pla(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, PHP_INSTRUCTION) == SUCCESS) {
        return encode_php(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, PHA_INSTRUCTION) == SUCCESS) {
        return encode_pha(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, ORA_INSTRUCTION) == SUCCESS) {
        return encode_ora(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, NOP_INSTRUCTION) == SUCCESS) {
        return encode_nop(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, LSR_INSTRUCTION) == SUCCESS) {
        return encode_lsr(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, JSR_INSTRUCTION) == SUCCESS) {
        return encode_jsr(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, JMP_INSTRUCTION) == SUCCESS) {
        return encode_jmp(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, INY_INSTRUCTION) == SUCCESS) {
        return encode_iny(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, INX_INSTRUCTION) == SUCCESS) {
        return encode_inx(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, DEC_INSTRUCTION) == SUCCESS) {
        return encode_dec(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, DEX_INSTRUCTION) == SUCCESS) {
        return encode_dex(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, DEY_INSTRUCTION) == SUCCESS) {
        return encode_dey(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, EOR_INSTRUCTION) == SUCCESS) {
        return encode_eor(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, INC_INSTRUCTION) == SUCCESS) {
        return encode_inc(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CLC_INSTRUCTION) == SUCCESS) {
        return encode_clc(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CLD_INSTRUCTION) == SUCCESS) {
        return encode_cld(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CLI_INSTRUCTION) == SUCCESS) {
        return encode_cli(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CLV_INSTRUCTION) == SUCCESS) {
        return encode_clv(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CMP_INSTRUCTION) == SUCCESS) {
        return encode_cmp(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CPX_INSTRUCTION) == SUCCESS) {
        return encode_cpx(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, CPY_INSTRUCTION) == SUCCESS) {
        return encode_cpy(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BNE_INSTRUCTION) == SUCCESS) {
        return encode_bne(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BPL_INSTRUCTION) == SUCCESS) {
        return encode_bpl(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BRK_INSTRUCTION) == SUCCESS) {
        return encode_brk(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BVC_INSTRUCTION) == SUCCESS) {
        return encode_bvc(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BVS_INSTRUCTION) == SUCCESS) {
        return encode_bvs(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, ADC_INSTRUCTION) == SUCCESS) {
        return encode_adc(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, AND_INSTRUCTION) == SUCCESS) {
        return encode_and(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, ASL_INSTRUCTION) == SUCCESS) {
        return encode_asl(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BCC_INSTRUCTION) == SUCCESS) {
        return encode_bcc(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BCS_INSTRUCTION) == SUCCESS) {
        return encode_bcs(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BEQ_INSTRUCTION) == SUCCESS) {
        return encode_beq(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BIT_INSTRUCTION) == SUCCESS) {
        return encode_bit(p, inst);
    }
    if(compare_identifiers_cp_array(inst->name, BMI_INSTRUCTION) == SUCCESS) {
        return encode_bmi(p, inst);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_6502, "invalid 6502 instruction");
    return INSTRUCTION_FAILED;
}
