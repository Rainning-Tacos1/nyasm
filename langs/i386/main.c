#include <stdio.h>


#include "asm_lang.h"
#include "../../src/core/helper.h"
#include "../../src/core/token.h"
#include "../../src/core/variables.h"
#include "types.h"

#include "api/debug.h"
#include "api/file.h"

#define ERROR_TYPE_I386 "i386Error"

#define MOV_INSTRUCTION ((int32_t[]){'m', 'o', 'v', -1})
#define LEA_INSTRUCTION ((int32_t[]){'l', 'e', 'a', -1})
#define LDS_INSTRUCTION ((int32_t[]){'l', 'd', 's', -1})
#define LES_INSTRUCTION ((int32_t[]){'l', 'e', 's', -1})
#define JMP_INSTRUCTION ((int32_t[]){'j', 'm', 'p', -1})
#define CALL_INSTRUCTION ((int32_t[]){'c', 'a', 'l', 'l', -1})
#define RET_INSTRUCTION ((int32_t[]){'r', 'e', 't', -1})
#define RETN_INSTRUCTION ((int32_t[]){'r', 'e', 't', 'n', -1})
#define RETF_INSTRUCTION ((int32_t[]){'r', 'e', 't', 'f', -1})
#define XCHG_INSTRUCTION ((int32_t[]){'x', 'c', 'h', 'g', -1})
#define PUSH_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', -1})
#define POP_INSTRUCTION ((int32_t[]){'p', 'o', 'p', -1})
#define PUSHF_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', 'f', -1})
#define POPF_INSTRUCTION ((int32_t[]){'p', 'o', 'p', 'f', -1})
#define LAHF_INSTRUCTION ((int32_t[]){'l', 'a', 'h', 'f', -1})
#define SAHF_INSTRUCTION ((int32_t[]){'s', 'a', 'h', 'f', -1})
#define XLAT_INSTRUCTION ((int32_t[]){'x', 'l', 'a', 't', -1})
#define XLATB_INSTRUCTION ((int32_t[]){'x', 'l', 'a', 't', 'b', -1})
#define MOVS_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', -1})
#define MOVSB_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', 'b', -1})
#define MOVSW_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', 'w', -1})
#define CMPS_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', -1})
#define CMPSB_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', 'b', -1})
#define CMPSW_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', 'w', -1})
#define SCAS_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', -1})
#define SCASB_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', 'b', -1})
#define SCASW_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', 'w', -1})
#define LODS_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', -1})
#define LODSB_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', 'b', -1})
#define LODSW_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', 'w', -1})
#define STOS_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', -1})
#define STOSB_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', 'b', -1})
#define STOSW_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', 'w', -1})
#define LOCK_INSTRUCTION ((int32_t[]){'l', 'o', 'c', 'k', -1})
#define REP_INSTRUCTION ((int32_t[]){'r', 'e', 'p', -1})
#define REPE_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'e', -1})
#define REPZ_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'z', -1})
#define REPNE_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'n', 'e', -1})
#define REPNZ_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'n', 'z', -1})
#define IN_INSTRUCTION ((int32_t[]){'i', 'n', -1})
#define OUT_INSTRUCTION ((int32_t[]){'o', 'u', 't', -1})
#define INT_INSTRUCTION ((int32_t[]){'i', 'n', 't', -1})
#define INTO_INSTRUCTION ((int32_t[]){'i', 'n', 't', 'o', -1})
#define IRET_INSTRUCTION ((int32_t[]){'i', 'r', 'e', 't', -1})
#define LOOP_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', -1})
#define LOOPE_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'e', -1})
#define LOOPZ_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'z', -1})
#define LOOPNE_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'n', 'e', -1})
#define LOOPNZ_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'n', 'z', -1})
#define JCXZ_INSTRUCTION ((int32_t[]){'j', 'c', 'x', 'z', -1})
#define JO_INSTRUCTION ((int32_t[]){'j', 'o', -1})
#define JNO_INSTRUCTION ((int32_t[]){'j', 'n', 'o', -1})
#define JB_INSTRUCTION ((int32_t[]){'j', 'b', -1})
#define JC_INSTRUCTION ((int32_t[]){'j', 'c', -1})
#define JNAE_INSTRUCTION ((int32_t[]){'j', 'n', 'a', 'e', -1})
#define JNB_INSTRUCTION ((int32_t[]){'j', 'n', 'b', -1})
#define JAE_INSTRUCTION ((int32_t[]){'j', 'a', 'e', -1})
#define JNC_INSTRUCTION ((int32_t[]){'j', 'n', 'c', -1})
#define JE_INSTRUCTION ((int32_t[]){'j', 'e', -1})
#define JZ_INSTRUCTION ((int32_t[]){'j', 'z', -1})
#define JNE_INSTRUCTION ((int32_t[]){'j', 'n', 'e', -1})
#define JNZ_INSTRUCTION ((int32_t[]){'j', 'n', 'z', -1})
#define JBE_INSTRUCTION ((int32_t[]){'j', 'b', 'e', -1})
#define JNA_INSTRUCTION ((int32_t[]){'j', 'n', 'a', -1})
#define JA_INSTRUCTION ((int32_t[]){'j', 'a', -1})
#define JNBE_INSTRUCTION ((int32_t[]){'j', 'n', 'b', 'e', -1})
#define JS_INSTRUCTION ((int32_t[]){'j', 's', -1})
#define JNS_INSTRUCTION ((int32_t[]){'j', 'n', 's', -1})
#define JP_INSTRUCTION ((int32_t[]){'j', 'p', -1})
#define JPE_INSTRUCTION ((int32_t[]){'j', 'p', 'e', -1})
#define JNP_INSTRUCTION ((int32_t[]){'j', 'n', 'p', -1})
#define JPO_INSTRUCTION ((int32_t[]){'j', 'p', 'o', -1})
#define JL_INSTRUCTION ((int32_t[]){'j', 'l', -1})
#define JNGE_INSTRUCTION ((int32_t[]){'j', 'n', 'g', 'e', -1})
#define JGE_INSTRUCTION ((int32_t[]){'j', 'g', 'e', -1})
#define JNL_INSTRUCTION ((int32_t[]){'j', 'n', 'l', -1})
#define JLE_INSTRUCTION ((int32_t[]){'j', 'l', 'e', -1})
#define JNG_INSTRUCTION ((int32_t[]){'j', 'n', 'g', -1})
#define JG_INSTRUCTION ((int32_t[]){'j', 'g', -1})
#define JNLE_INSTRUCTION ((int32_t[]){'j', 'n', 'l', 'e', -1})
#define ADD_INSTRUCTION ((int32_t[]){'a', 'd', 'd', -1})
#define ADC_INSTRUCTION ((int32_t[]){'a', 'd', 'c', -1})
#define SUB_INSTRUCTION ((int32_t[]){'s', 'u', 'b', -1})
#define SBB_INSTRUCTION ((int32_t[]){'s', 'b', 'b', -1})
#define CMP_INSTRUCTION ((int32_t[]){'c', 'm', 'p', -1})
#define INC_INSTRUCTION ((int32_t[]){'i', 'n', 'c', -1})
#define DEC_INSTRUCTION ((int32_t[]){'d', 'e', 'c', -1})
#define NEG_INSTRUCTION ((int32_t[]){'n', 'e', 'g', -1})
#define MUL_INSTRUCTION ((int32_t[]){'m', 'u', 'l', -1})
#define IMUL_INSTRUCTION ((int32_t[]){'i', 'm', 'u', 'l', -1})
#define DIV_INSTRUCTION ((int32_t[]){'d', 'i', 'v', -1})
#define IDIV_INSTRUCTION ((int32_t[]){'i', 'd', 'i', 'v', -1})
#define CBW_INSTRUCTION ((int32_t[]){'c', 'b', 'w', -1})
#define CWD_INSTRUCTION ((int32_t[]){'c', 'w', 'd', -1})
#define AAA_INSTRUCTION ((int32_t[]){'a', 'a', 'a', -1})
#define AAD_INSTRUCTION ((int32_t[]){'a', 'a', 'd', -1})
#define AAM_INSTRUCTION ((int32_t[]){'a', 'a', 'm', -1})
#define AAS_INSTRUCTION ((int32_t[]){'a', 'a', 's', -1})
#define DAA_INSTRUCTION ((int32_t[]){'d', 'a', 'a', -1})
#define DAS_INSTRUCTION ((int32_t[]){'d', 'a', 's', -1})
#define AND_INSTRUCTION ((int32_t[]){'a', 'n', 'd', -1})
#define OR_INSTRUCTION ((int32_t[]){'o', 'r', -1})
#define XOR_INSTRUCTION ((int32_t[]){'x', 'o', 'r', -1})
#define TEST_INSTRUCTION ((int32_t[]){'t', 'e', 's', 't', -1})
#define NOT_INSTRUCTION ((int32_t[]){'n', 'o', 't', -1})
#define CLC_INSTRUCTION ((int32_t[]){'c', 'l', 'c', -1})
#define STC_INSTRUCTION ((int32_t[]){'s', 't', 'c', -1})
#define CMC_INSTRUCTION ((int32_t[]){'c', 'm', 'c', -1})
#define CLD_INSTRUCTION ((int32_t[]){'c', 'l', 'd', -1})
#define STD_INSTRUCTION ((int32_t[]){'s', 't', 'd', -1})
#define CLI_INSTRUCTION ((int32_t[]){'c', 'l', 'i', -1})
#define STI_INSTRUCTION ((int32_t[]){'s', 't', 'i', -1})
#define HLT_INSTRUCTION ((int32_t[]){'h', 'l', 't', -1})
#define WAIT_INSTRUCTION ((int32_t[]){'w', 'a', 'i', 't', -1})
#define FWAIT_INSTRUCTION ((int32_t[]){'f', 'w', 'a', 'i', 't', -1})
#define ESC_INSTRUCTION ((int32_t[]){'e', 's', 'c', -1})
#define NOP_INSTRUCTION ((int32_t[]){'n', 'o', 'p', -1})
#define SHL_INSTRUCTION ((int32_t[]){'s', 'h', 'l', -1})
#define SAL_INSTRUCTION ((int32_t[]){'s', 'a', 'l', -1})
#define SHR_INSTRUCTION ((int32_t[]){'s', 'h', 'r', -1})
#define SAR_INSTRUCTION ((int32_t[]){'s', 'a', 'r', -1})
#define ROL_INSTRUCTION ((int32_t[]){'r', 'o', 'l', -1})
#define ROR_INSTRUCTION ((int32_t[]){'r', 'o', 'r', -1})
#define RCL_INSTRUCTION ((int32_t[]){'r', 'c', 'l', -1})
#define RCR_INSTRUCTION ((int32_t[]){'r', 'c', 'r', -1})

nint _86_exec(struct Parser* p, struct AstInstruction* inst);

#ifdef i386
#undef i386
#endif

ASM_LANG i386 = {
    .lang_name = "i386 assembly v0.0.1",
    .code_name = "i386",
    .max_addr = 0Xffff,
    .size_of_ptr = 2,
    .le = 1,

    .exec = _86_exec,
};

static int32_t *registers[] = {
    (int32_t[]){'a', 'x', -1}, (int32_t[]){'b', 'x', -1},
    (int32_t[]){'c', 'x', -1}, (int32_t[]){'d', 'x', -1},
    (int32_t[]){'s', 'p', -1}, (int32_t[]){'b', 'p', -1},
    (int32_t[]){'s', 'i', -1}, (int32_t[]){'d', 'i', -1},
    (int32_t[]){'i', 'p', -1}, (int32_t[]){'c', 's', -1},
    (int32_t[]){'d', 's', -1}, (int32_t[]){'e', 's', -1},
    (int32_t[]){'s', 's', -1}, (int32_t[]){'a', 'l', -1},
    (int32_t[]){'a', 'h', -1}, (int32_t[]){'b', 'l', -1},
    (int32_t[]){'b', 'h', -1}, (int32_t[]){'c', 'l', -1},
    (int32_t[]){'c', 'h', -1}, (int32_t[]){'d', 'l', -1},
    (int32_t[]){'d', 'h', -1}, NULL};

static int32_t *reg8_names[] = {
    (int32_t[]){'a', 'l', -1}, (int32_t[]){'c', 'l', -1},
    (int32_t[]){'d', 'l', -1}, (int32_t[]){'b', 'l', -1},
    (int32_t[]){'a', 'h', -1}, (int32_t[]){'c', 'h', -1},
    (int32_t[]){'d', 'h', -1}, (int32_t[]){'b', 'h', -1}, NULL};

static int32_t *reg16_names[] = {
    (int32_t[]){'a', 'x', -1}, (int32_t[]){'c', 'x', -1},
    (int32_t[]){'d', 'x', -1}, (int32_t[]){'b', 'x', -1},
    (int32_t[]){'s', 'p', -1}, (int32_t[]){'b', 'p', -1},
    (int32_t[]){'s', 'i', -1}, (int32_t[]){'d', 'i', -1}, NULL};

static int32_t *seg_names[] = {
    (int32_t[]){'e', 's', -1}, (int32_t[]){'c', 's', -1},
    (int32_t[]){'s', 's', -1}, (int32_t[]){'d', 's', -1}, NULL};

#define BYTE_TYPE ((int32_t[]){'b', 'y', 't', 'e', -1})
#define WORD_TYPE ((int32_t[]){'w', 'o', 'r', 'd', -1})
#define SHORT_TYPE ((int32_t[]){'s', 'h', 'o', 'r', 't', -1})
#define NEAR_TYPE ((int32_t[]){'n', 'e', 'a', 'r', -1})
#define FAR_TYPE ((int32_t[]){'f', 'a', 'r', -1})
#define PTR_NAME ((int32_t[]){'p', 't', 'r', -1})

enum OperandKind {
    OPERAND_INVALID,
    OPERAND_REG8,
    OPERAND_REG16,
    OPERAND_SEG,
    OPERAND_IMM,
    OPERAND_MEM
};

struct MemoryOperand {
    nint rm;
    unint direct;
    nint disp;
    unint has_disp;
    nint seg_override;
};

struct Operand {
    enum OperandKind kind;
    nint size;
    nint reg;
    nint imm;
    struct MemoryOperand mem;
};

static unint tks_at_end(struct TokenStream *tks) {
    struct token *tok = tks_peek(tks);
    tks_reset_peek(tks);
    return tok == NULL;
}

static void record_error_token(struct token **error_token, struct token *tok) {
    if(error_token && tok && (!*error_token || tok > *error_token)) {
        *error_token = tok;
    }
}

static unint expect_tks_end(struct TokenStream *tks, struct token **error_token) {
    struct token *tok = tks_peek(tks);
    tks_reset_peek(tks);
    if(!tok) return 1;
    record_error_token(error_token, tok);
    return 0;
}

static unint read_token_type(struct Parser *p, struct TokenStream *tks, unint type,
                             struct token **error_token) {
    struct token *tok = tks_read(tks);
    if(!expected_token(p, tok, type)) {
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);
    return 1;
}

static unint token_matches(struct token *tok, int32_t **names, unint *idx) {
    if(!tok || tok->type != NAME) return 0;
    for(unint i = 0; names[i]; i++) {
        if(compare_identifiers_cp_array(tok, names[i]) == SUCCESS) {
            *idx = i;
            return 1;
        }
    }
    return 0;
}

static unint parse_register_exact(struct InstructionArg *arg, int32_t **names,
                                  unint *idx, struct token **error_token) {
    struct TokenStream tks;
    tks_init(&tks, arg->_s, arg->_e);
    if(parse_potential_register(&tks, names, idx) != SUCCESS) return 0;
    record_error_token(error_token, arg->_s);
    return expect_tks_end(&tks, error_token);
}

static unint read_integer(struct Parser *p, struct TokenStream *tks, nint *out,
                          struct token **err_token) {
    unint is_neg = 0;
    struct token *tok = tks_read(tks);
    if(tok && tok->type == MINUS) {
        is_neg = 1;
        record_error_token(err_token, tok);
        tok = tks_read(tks);
    }

    if(!tok || tok->type != NUMBER) {
        return 0;
    }

    record_error_token(err_token, tok);
    struct Value *v = new_number(p, tok, is_neg);
    if(!v) return 0;
    if(v->type != VALUE_INT) {
        _error_from_token(p, tok, ERROR_TYPE_TYPE, "invalid type");
        return 0;
    }

    *out = v->val.number;
    return 1;
}

static unint parse_immediate(struct Parser *p, struct InstructionArg *arg,
                             struct Operand *op, struct token **error_token) {
    struct TokenStream tks;
    nint value;

    tks_init(&tks, arg->_s, arg->_e);
    if(!read_integer(p, &tks, &value, error_token)) return 0;
    if(!expect_tks_end(&tks, error_token)) return 0;

    op->kind = OPERAND_IMM;
    op->imm = value;
    return 1;
}

static nint mem_reg_class(unint reg) {
    switch(reg) {
        case 3: return 1; /* bx */
        case 5: return 1; /* bp */
        case 6: return 2; /* si */
        case 7: return 2; /* di */
        default: return 0;
    }
}

static unint rm_code_from_regs(unint first, nint second, nint *rm) {
    if(second < 0) {
        switch(first) {
            case 6: *rm = 4; return 1; /* si */
            case 7: *rm = 5; return 1; /* di */
            case 5: *rm = 6; return 1; /* bp */
            case 3: *rm = 7; return 1; /* bx */
            default: return 0;
        }
    }

    if(first == 3 && second == 6) { *rm = 0; return 1; }
    if(first == 3 && second == 7) { *rm = 1; return 1; }
    if(first == 5 && second == 6) { *rm = 2; return 1; }
    if(first == 5 && second == 7) { *rm = 3; return 1; }
    return 0;
}

static unint read_memory_displacement(struct Parser *p, struct TokenStream *tks,
                                      nint sign, nint *disp,
                                      struct token **error_token) {
    nint value;

    if(!read_integer(p, tks, &value, error_token)) return 0;
    *disp = sign < 0 ? -value : value;
    return 1;
}

static nint explicit_type_size(struct token *tok) {
    if(compare_identifiers_cp_array(tok, BYTE_TYPE) == SUCCESS) return 1;
    if(compare_identifiers_cp_array(tok, WORD_TYPE) == SUCCESS) return 2;
    return 0;
}

static void set_direct_memory(struct Operand *op, nint addr, nint seg_override) {
    op->kind = OPERAND_MEM;
    op->mem.rm = 6;
    op->mem.direct = 1;
    op->mem.disp = addr;
    op->mem.has_disp = 1;
    op->mem.seg_override = seg_override;
}

static nint parse_bracketed_variable(struct Parser *p, struct TokenStream *tks,
                                     struct token *start, struct Operand *op,
                                     nint seg_override,
                                     struct token **error_token) {
    nint addr = 0;
    unint status;

    tks->read = start;
    tks_reset_peek(tks);
    status = parse_potential_variable(p, tks, &addr, 1);

    if(status == VP_FAIL) return INSTRUCTION_FAILED;
    if(status == VP_UNRESOLVED_LABEL) {
        record_error_token(error_token, start);
        if(p->last_pass) return unresolved_label(p, start);
        if(!read_token_type(p, tks, RSQB, error_token) ||
           !expect_tks_end(tks, error_token)) return INSTRUCTION_FAILED;
        set_direct_memory(op, 0, seg_override);
        return 1;
    }
    if(status != VP_SUCCESS) return 0;
    record_error_token(error_token, start);
    if(!read_token_type(p, tks, RSQB, error_token) ||
       !expect_tks_end(tks, error_token)) return INSTRUCTION_FAILED;

    set_direct_memory(op, addr, seg_override);
    return 1;
}

static nint finish_memory_operand(struct Parser *p, struct TokenStream *tks,
                                  struct Operand *op, nint seg_override,
                                  struct token **error_token) {
    struct token *tok = tks_read(tks);
    unint first;
    nint second = -1;
    nint rm;
    nint disp = 0;
    unint has_disp = 0;

    if(!tok) return 0;

    if(tok->type == NUMBER || tok->type == MINUS) {
        tks->read = tok;
        if(!read_integer(p, tks, &disp, error_token)) return 0;
        if(!read_token_type(p, tks, RSQB, error_token) ||
           !expect_tks_end(tks, error_token)) return 0;

        set_direct_memory(op, disp, seg_override);
        return 1;
    }

    if(!token_matches(tok, reg16_names, &first) || !mem_reg_class(first)) {
        if(tok->type == NAME) {
            return parse_bracketed_variable(p, tks, tok, op, seg_override,
                                            error_token);
        }
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);

    tok = tks_read(tks);
    if(!tok) return 0;

    if(tok->type == PLUS || tok->type == MINUS) {
        nint sign = tok->type == MINUS ? -1 : 1;
        record_error_token(error_token, tok);
        tok = tks_read(tks);
        if(!tok) return 0;

        if(tok->type == NAME) {
            unint reg;
            if(sign < 0) {
                record_error_token(error_token, tok);
                return 0;
            }
            if(!token_matches(tok, reg16_names, &reg) || !mem_reg_class(reg)) {
                record_error_token(error_token, tok);
                return 0;
            }
            if(mem_reg_class(first) != 1 || mem_reg_class(reg) != 2) return 0;
            record_error_token(error_token, tok);
            second = (nint)reg;

            tok = tks_read(tks);
            if(tok && (tok->type == PLUS || tok->type == MINUS)) {
                sign = tok->type == MINUS ? -1 : 1;
                record_error_token(error_token, tok);
                if(!read_memory_displacement(p, tks, sign, &disp, error_token)) return 0;
                has_disp = 1;
                tok = tks_read(tks);
            }
        } else if(tok->type == NUMBER) {
            tks->read = tok;
            if(!read_memory_displacement(p, tks, sign, &disp, error_token)) return 0;
            has_disp = 1;
            tok = tks_read(tks);
        } else {
            record_error_token(error_token, tok);
            return 0;
        }
    }

    if(!tok || tok->type != RSQB) {
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);
    if(!expect_tks_end(tks, error_token)) return 0;
    if(!rm_code_from_regs(first, second, &rm)) return 0;

    op->kind = OPERAND_MEM;
    op->mem.rm = rm;
    op->mem.direct = 0;
    op->mem.disp = disp;
    op->mem.has_disp = has_disp;
    op->mem.seg_override = seg_override;
    return 1;
}

static nint parse_memory(struct Parser *p, struct InstructionArg *arg,
                         struct Operand *op, struct token **error_token) {
    struct TokenStream tks;
    struct token *tok;
    nint seg_override = -1;
    nint explicit_size = 0;

    tks_init(&tks, arg->_s, arg->_e);
    tok = tks_read(&tks);
    if(!tok) return 0;

    if(tok->type == NAME) {
        explicit_size = explicit_type_size(tok);
        if(explicit_size) {
            struct token *ptr_tok;

            record_error_token(error_token, tok);
            ptr_tok = tks_read(&tks);
            if(!expected_token(p, ptr_tok, NAME)) {
                record_error_token(error_token, ptr_tok);
                return INSTRUCTION_FAILED;
            }
            record_error_token(error_token, ptr_tok);
            if(compare_identifiers_cp_array(ptr_tok, PTR_NAME) != SUCCESS) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386, "expected ptr");
                return INSTRUCTION_FAILED;
            }
            tok = tks_read(&tks);
        }
    }

    if(tok->type == NAME) {
        unint seg;
        struct token *colon;

        if(!token_matches(tok, seg_names, &seg)) return 0;
        record_error_token(error_token, tok);
        colon = tks_read(&tks);
        if(!colon || colon->type != COLON) return 0;
        record_error_token(error_token, colon);
        seg_override = (nint)seg;
        tok = tks_read(&tks);
    }

    if(!tok || tok->type != LSQB) return 0;
    record_error_token(error_token, tok);
    nint status = finish_memory_operand(p, &tks, op, seg_override, error_token);
    if(status > 0) op->size = explicit_size;
    return status;
}

static nint parse_variable_as_immediate(struct Parser *p, struct InstructionArg *arg,
                                        struct Operand *op,
                                        struct token **error_token) {
    struct TokenStream tks;
    nint addr = 0;
    unint status;

    tks_init(&tks, arg->_s, arg->_e);
    status = parse_potential_variable(p, &tks, &addr, 0);

    if(status == VP_FAIL) return -1;
    if(status == VP_UNRESOLVED_LABEL) {
        record_error_token(error_token, arg->_s);
        if(p->last_pass) return unresolved_label(p, arg->_s);
        op->kind = OPERAND_IMM;
        op->imm = 0;
        return 1;
    }
    if(status != VP_SUCCESS) return 0;
    if(!tks_at_end(&tks)) return -1;
    record_error_token(error_token, arg->_e);

    op->kind = OPERAND_IMM;
    op->imm = addr;
    return 1;
}

static nint parse_operand(struct Parser *p, struct InstructionArg *arg,
                          struct Operand *op, struct token **error_token) {
    unint idx;
    nint memory_status;
    nint variable_status;

    op->kind = OPERAND_INVALID;
    op->size = 0;
    op->reg = 0;
    op->imm = 0;
    op->mem.rm = 0;
    op->mem.direct = 0;
    op->mem.disp = 0;
    op->mem.has_disp = 0;
    op->mem.seg_override = -1;
    record_error_token(error_token, arg->_s);

    if(parse_register_exact(arg, reg8_names, &idx, error_token)) {
        op->kind = OPERAND_REG8;
        op->size = 1;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, reg16_names, &idx, error_token)) {
        op->kind = OPERAND_REG16;
        op->size = 2;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, seg_names, &idx, error_token)) {
        op->kind = OPERAND_SEG;
        op->size = 2;
        op->reg = (nint)idx;
        return 1;
    }

    memory_status = parse_memory(p, arg, op, error_token);
    if(memory_status < 0) return memory_status;
    if(memory_status > 0) return 1;

    variable_status = parse_variable_as_immediate(p, arg, op, error_token);
    if(variable_status < 0) return variable_status;
    if(variable_status > 0) return 1;

    if(parse_immediate(p, arg, op, error_token)) return 1;

    return 0;
}

static unint fits_u8(nint value) {
    return value >= 0 && value <= 0xff;
}

static unint fits_u16(nint value) {
    return value >= -32768 && value <= 0xffff;
}

static unint fits_i8(nint value) {
    return value >= -128 && value <= 127;
}

static unint fits_i16(nint value) {
    return value >= -32768 && value <= 32767;
}

static unint fits_push_imm8(nint value) {
    nint low;
    nint word;

    if(fits_i8(value)) return 1;
    if(value < 0 || value > 0xffff) return 0;

    low = value & 0xff;
    word = value & 0xffff;
    if(low & 0x80) return word == (0xff00 | low);
    return word == low;
}

static unint fits_word_sign_extended_i8(nint value) {
    return fits_push_imm8(value);
}

static nint memory_tail_len(struct MemoryOperand *mem) {
    if(mem->direct) return 2;
    if(mem->rm == 6 && !mem->has_disp) return 1;
    if(!mem->has_disp) return 0;
    return fits_i8(mem->disp) ? 1 : 2;
}

static void emit_byte(struct Parser *p, unsigned char byte) {
    if(p->last_pass) OUT_FILE_WRITE_BYTE(byte);
}

static void emit_word(struct Parser *p, nint value) {
    if(p->last_pass) {
        OUT_FILE_WRITE_BYTE((unsigned char)(value & 0xff));
        OUT_FILE_WRITE_BYTE((unsigned char)((value >> 8) & 0xff));
    }
}

static void emit_segment_prefix(struct Parser *p, struct MemoryOperand *mem) {
    static const unsigned char prefixes[] = {0x26, 0x2e, 0x36, 0x3e};
    if(mem->seg_override >= 0) emit_byte(p, prefixes[mem->seg_override]);
}

static nint segment_prefix_len(struct MemoryOperand *mem) {
    return mem->seg_override >= 0 ? 1 : 0;
}

static void emit_modrm_reg(struct Parser *p, nint reg, nint rm) {
    emit_byte(p, (unsigned char)(0xc0 | ((reg & 7) << 3) | (rm & 7)));
}

static void emit_modrm_mem(struct Parser *p, nint reg, struct MemoryOperand *mem) {
    nint mod;

    if(mem->direct) mod = 0;
    else if(mem->rm == 6 && !mem->has_disp) mod = 1;
    else if(!mem->has_disp) mod = 0;
    else mod = fits_i8(mem->disp) ? 1 : 2;

    emit_byte(p, (unsigned char)((mod << 6) | ((reg & 7) << 3) | (mem->rm & 7)));
    if(mem->direct) emit_word(p, mem->disp);
    else if(mod == 1) emit_byte(p, (unsigned char)(mem->disp & 0xff));
    else if(mod == 2) emit_word(p, mem->disp);
}

static unint is_al(struct Operand *op) {
    return op->kind == OPERAND_REG8 && op->reg == 0;
}

static unint is_ax(struct Operand *op) {
    return op->kind == OPERAND_REG16 && op->reg == 0;
}

static unint is_dx(struct Operand *op) {
    return op->kind == OPERAND_REG16 && op->reg == 2;
}

static unint is_cl(struct Operand *op) {
    return op->kind == OPERAND_REG8 && op->reg == 1;
}

static nint encode_mov(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_IMM) {
        if(!fits_u8(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 8 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, (unsigned char)(0xb0 + dst->reg));
        emit_byte(p, (unsigned char)src->imm);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, (unsigned char)(0xb8 + dst->reg));
        emit_word(p, src->imm);
        return 3;
    }

    if(is_al(dst) && src->kind == OPERAND_MEM && src->mem.direct) {
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0xa0);
        emit_word(p, src->mem.disp);
        return segment_prefix_len(&src->mem) + 3;
    }
    if(is_ax(dst) && src->kind == OPERAND_MEM && src->mem.direct) {
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0xa1);
        emit_word(p, src->mem.disp);
        return segment_prefix_len(&src->mem) + 3;
    }
    if(dst->kind == OPERAND_MEM && dst->mem.direct && is_al(src)) {
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0xa2);
        emit_word(p, dst->mem.disp);
        return segment_prefix_len(&dst->mem) + 3;
    }
    if(dst->kind == OPERAND_MEM && dst->mem.direct && is_ax(src)) {
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0xa3);
        emit_word(p, dst->mem.disp);
        return segment_prefix_len(&dst->mem) + 3;
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x88);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }
    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x89);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x8a);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }
    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x8b);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x88);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x89);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_SEG) {
        emit_byte(p, 0x8c);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_SEG) {
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x8c);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }
    if(dst->kind == OPERAND_SEG && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x8e);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 2;
    }
    if(dst->kind == OPERAND_SEG && src->kind == OPERAND_MEM) {
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x8e);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_IMM) {
        if(dst->size == 1) {
            if(!fits_u8(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 8 bits");
                return INSTRUCTION_FAILED;
            }
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xc6);
            emit_modrm_mem(p, 0, &dst->mem);
            emit_byte(p, (unsigned char)src->imm);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        if(dst->size == 2) {
            if(!fits_u16(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xc7);
            emit_modrm_mem(p, 0, &dst->mem);
            emit_word(p, src->imm);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous memory immediate size");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported mov form");
    return INSTRUCTION_FAILED;
}

static nint encode_lea(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    struct Operand direct_src;

    if(src->kind == OPERAND_IMM) {
        direct_src = *src;
        set_direct_memory(&direct_src, src->imm, -1);
        src = &direct_src;
    }

    if(dst->kind != OPERAND_REG16 || src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported lea form");
        return INSTRUCTION_FAILED;
    }

    emit_segment_prefix(p, &src->mem);
    emit_byte(p, 0x8d);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
}

static nint encode_far_load(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src,
                            unsigned char opcode) {
    if(dst->kind != OPERAND_REG16 || src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported far load form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid far load operand size");
        return INSTRUCTION_FAILED;
    }

    emit_segment_prefix(p, &src->mem);
    emit_byte(p, opcode);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
}

#define JMP_CACHE_MAX 512

static nint jmp_cache_addr[JMP_CACHE_MAX];
static nint jmp_cache_size[JMP_CACHE_MAX];
static unint jmp_cache_used[JMP_CACHE_MAX];

static void remember_jmp_size(nint addr, nint size) {
    for(unint i = 0; i < JMP_CACHE_MAX; i++) {
        if(jmp_cache_used[i] && jmp_cache_addr[i] == addr) {
            jmp_cache_size[i] = size;
            return;
        }
    }
    for(unint i = 0; i < JMP_CACHE_MAX; i++) {
        if(!jmp_cache_used[i]) {
            jmp_cache_used[i] = 1;
            jmp_cache_addr[i] = addr;
            jmp_cache_size[i] = size;
            return;
        }
    }
}

static nint remembered_jmp_size(nint addr) {
    for(unint i = 0; i < JMP_CACHE_MAX; i++) {
        if(jmp_cache_used[i] && jmp_cache_addr[i] == addr) return jmp_cache_size[i];
    }
    return 0;
}

static unint token_is(struct token *tok, int32_t *name) {
    return tok && tok->type == NAME &&
           compare_identifiers_cp_array(tok, name) == SUCCESS;
}

static nint read_jump_scalar(struct Parser *p, struct TokenStream *tks,
                             nint *value, unint *unresolved,
                             unint *is_variable,
                             struct token **error_token) {
    struct token *tok = tks_peek(tks);
    unint status;
    tks_reset_peek(tks);
    if(!tok) return 0;

    if(tok->type == NAME) {
        status = parse_potential_variable(p, tks, value, 1);
        if(status == VP_SUCCESS) {
            record_error_token(error_token, tok);
            *is_variable = 1;
            return 1;
        }
        if(status == VP_UNRESOLVED_LABEL) {
            record_error_token(error_token, tok);
            *value = 0;
            *unresolved = 1;
            *is_variable = 1;
            if(p->last_pass) return unresolved_label(p, tok);
            return 1;
        }
        if(status == VP_FAIL) return INSTRUCTION_FAILED;
        return 0;
    }

    if(tok->type == NUMBER || tok->type == MINUS) {
        if(!read_integer(p, tks, value, error_token)) return 0;
        *is_variable = 0;
        return 1;
    }

    return 0;
}

static nint emit_jmp_relative(struct Parser *p, nint target, nint size,
                              struct token *tok) {
    nint disp = target - (p->addr + size);
    if(size == 2) {
        if(!fits_i8(disp)) {
            _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                              "short jump target out of range");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xeb);
        emit_byte(p, (unsigned char)(disp & 0xff));
        return 2;
    }
    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near jump target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_byte(p, 0xe9);
    emit_word(p, disp);
    return 3;
}

static nint parse_jump_memory_from(struct Parser *p, struct token *start,
                                   struct token *end, struct Operand *mem,
                                   struct token **error_token) {
    struct InstructionArg arg;
    nint status;

    arg._s = start;
    arg._e = end;
    arg.next = NULL;
    status = parse_memory(p, &arg, mem, error_token);
    if(status <= 0) return status;
    if(mem->size != 0) {
        _error_from_token(p, start, ERROR_TYPE_I386,
                          "invalid jump pointer size");
        return INSTRUCTION_FAILED;
    }
    return 1;
}

static nint encode_jmp_ptr_mem(struct Parser *p, struct token *start,
                               struct token *end, nint far_ptr) {
    struct Operand mem;
    struct token *error_token = NULL;
    nint status = parse_jump_memory_from(p, start, end, &mem, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid jump pointer");
        return INSTRUCTION_FAILED;
    }

    emit_segment_prefix(p, &mem.mem);
    emit_byte(p, 0xff);
    emit_modrm_mem(p, far_ptr ? 5 : 4, &mem.mem);
    return segment_prefix_len(&mem.mem) + 2 + memory_tail_len(&mem.mem);
}

static nint encode_jmp_far_immediate(struct Parser *p, struct token *start,
                                     struct token *end) {
    struct TokenStream tks;
    struct token *colon;
    struct token *error_token = NULL;
    nint seg = 0;
    nint off = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &seg, &unresolved, &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far jump segment");
        return INSTRUCTION_FAILED;
    }

    colon = tks_read(&tks);
    if(!colon || colon->type != COLON) {
        _error_from_token(p, colon ? colon : start, ERROR_TYPE_I386,
                          "expected far jump separator");
        return INSTRUCTION_FAILED;
    }
    record_error_token(&error_token, colon);

    status = read_jump_scalar(p, &tks, &off, &unresolved, &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far jump offset");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 5;
    if(!fits_u16(seg) || !fits_u16(off)) {
        _error_from_token(p, start, ERROR_TYPE_OVERFLOW,
                          "far jump pointer does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, 0xea);
    emit_word(p, off);
    emit_word(p, seg);
    return 5;
}

static nint encode_jmp_direct(struct Parser *p, struct token *start,
                              struct token *end, nint explicit_size) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    nint size = explicit_size;
    nint cached;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &target, &unresolved, &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(explicit_size) {
        if(unresolved && !p->last_pass) return explicit_size;
        return emit_jmp_relative(p, target, explicit_size, start);
    }

    if(!p->last_pass) {
        if(unresolved) {
            remember_jmp_size(p->addr, 3);
            return 3;
        }
        size = fits_i8(target - (p->addr + 2)) ? 2 : 3;
        remember_jmp_size(p->addr, size);
        return size;
    }

    cached = remembered_jmp_size(p->addr);
    if(cached) size = cached;
    else if(is_variable) size = 3;
    else size = fits_i8(target - (p->addr + 2)) ? 2 : 3;

    return emit_jmp_relative(p, target, size, start);
}

static nint encode_jmp(struct Parser *p, struct AstInstruction *inst) {
    struct TokenStream tks;
    struct token *tok;
    struct token *ptr_tok;
    struct token *target_start;
    struct Operand dst;
    nint is_far;
    nint mem_status;
    nint status;
    struct Operand mem;
    struct token *error_token = NULL;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, SHORT_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
            return INSTRUCTION_FAILED;
        }
        return encode_jmp_direct(p, target_start, inst->args_head->_e, 2);
    }

    if(token_is(tok, NEAR_TYPE) || token_is(tok, FAR_TYPE)) {
        is_far = token_is(tok, FAR_TYPE);
        ptr_tok = tks_read(&tks);
        if(ptr_tok && token_is(ptr_tok, PTR_NAME)) {
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386,
                                  "invalid jump pointer");
                return INSTRUCTION_FAILED;
            }
            if(!is_far) {
                return encode_jmp_ptr_mem(p, target_start, inst->args_head->_e, 0);
            }

            mem_status = parse_jump_memory_from(p, target_start,
                                                inst->args_head->_e, &mem,
                                                &error_token);
            if(mem_status > 0) {
                emit_segment_prefix(p, &mem.mem);
                emit_byte(p, 0xff);
                emit_modrm_mem(p, 5, &mem.mem);
                return segment_prefix_len(&mem.mem) + 2 +
                       memory_tail_len(&mem.mem);
            }
            if(mem_status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
            return encode_jmp_far_immediate(p, target_start, inst->args_head->_e);
        }
        if(is_far) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "expected ptr");
            return INSTRUCTION_FAILED;
        }
        if(ptr_tok) return encode_jmp_direct(p, ptr_tok, inst->args_head->_e, 3);
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 4, dst.reg);
        return 2;
    }

    if(dst.kind == OPERAND_MEM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous jump pointer");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_IMM) {
        return encode_jmp_direct(p, inst->args_head->_s, inst->args_head->_e, 0);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported jmp form");
    return INSTRUCTION_FAILED;
}

static nint encode_rel8_control(struct Parser *p, struct AstInstruction *inst,
                                unsigned char opcode) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    nint disp;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    status = read_jump_scalar(p, &tks, &target, &unresolved,
                              &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : inst->args_head->_s,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 2;
    disp = target - (p->addr + 2);
    if(!fits_i8(disp)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "branch target out of range");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return 2;
}

static nint emit_call_relative(struct Parser *p, nint target,
                               struct token *tok) {
    nint disp = target - (p->addr + 3);
    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near call target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_byte(p, 0xe8);
    emit_word(p, disp);
    return 3;
}

static nint parse_call_memory_from(struct Parser *p, struct token *start,
                                   struct token *end, struct Operand *mem,
                                   struct token **error_token) {
    struct InstructionArg arg;
    nint status;

    arg._s = start;
    arg._e = end;
    arg.next = NULL;
    status = parse_memory(p, &arg, mem, error_token);
    if(status <= 0) return status;
    if(mem->size != 0) {
        _error_from_token(p, start, ERROR_TYPE_I386,
                          "invalid call pointer size");
        return INSTRUCTION_FAILED;
    }
    return 1;
}

static nint encode_call_ptr_mem(struct Parser *p, struct token *start,
                                struct token *end, nint far_ptr) {
    struct Operand mem;
    struct token *error_token = NULL;
    nint status = parse_call_memory_from(p, start, end, &mem, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid call pointer");
        return INSTRUCTION_FAILED;
    }

    emit_segment_prefix(p, &mem.mem);
    emit_byte(p, 0xff);
    emit_modrm_mem(p, far_ptr ? 3 : 2, &mem.mem);
    return segment_prefix_len(&mem.mem) + 2 + memory_tail_len(&mem.mem);
}

static nint encode_call_far_immediate(struct Parser *p, struct token *start,
                                      struct token *end) {
    struct TokenStream tks;
    struct token *colon;
    struct token *error_token = NULL;
    nint seg = 0;
    nint off = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &seg, &unresolved, &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far call segment");
        return INSTRUCTION_FAILED;
    }

    colon = tks_read(&tks);
    if(!colon || colon->type != COLON) {
        _error_from_token(p, colon ? colon : start, ERROR_TYPE_I386,
                          "expected far call separator");
        return INSTRUCTION_FAILED;
    }
    record_error_token(&error_token, colon);

    status = read_jump_scalar(p, &tks, &off, &unresolved, &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far call offset");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 5;
    if(!fits_u16(seg) || !fits_u16(off)) {
        _error_from_token(p, start, ERROR_TYPE_OVERFLOW,
                          "far call pointer does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, 0x9a);
    emit_word(p, off);
    emit_word(p, seg);
    return 5;
}

static nint encode_call_direct(struct Parser *p, struct token *start,
                               struct token *end) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &target, &unresolved, &is_variable, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 3;
    return emit_call_relative(p, target, start);
}

static nint encode_call(struct Parser *p, struct AstInstruction *inst) {
    struct TokenStream tks;
    struct token *tok;
    struct token *ptr_tok;
    struct token *target_start;
    struct Operand dst;
    struct Operand mem;
    struct token *error_token = NULL;
    nint is_far;
    nint mem_status;
    nint status;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, SHORT_TYPE)) {
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid call target size");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, NEAR_TYPE) || token_is(tok, FAR_TYPE)) {
        is_far = token_is(tok, FAR_TYPE);
        ptr_tok = tks_read(&tks);
        if(ptr_tok && token_is(ptr_tok, PTR_NAME)) {
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386,
                                  "invalid call pointer");
                return INSTRUCTION_FAILED;
            }
            if(!is_far) {
                return encode_call_ptr_mem(p, target_start, inst->args_head->_e, 0);
            }

            mem_status = parse_call_memory_from(p, target_start,
                                                inst->args_head->_e, &mem,
                                                &error_token);
            if(mem_status > 0) {
                emit_segment_prefix(p, &mem.mem);
                emit_byte(p, 0xff);
                emit_modrm_mem(p, 3, &mem.mem);
                return segment_prefix_len(&mem.mem) + 2 +
                       memory_tail_len(&mem.mem);
            }
            if(mem_status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
            return encode_call_far_immediate(p, target_start, inst->args_head->_e);
        }
        if(is_far) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "expected ptr");
            return INSTRUCTION_FAILED;
        }
        if(ptr_tok) return encode_call_direct(p, ptr_tok, inst->args_head->_e);
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid call target");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 2, dst.reg);
        return 2;
    }

    if(dst.kind == OPERAND_MEM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous call pointer");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_IMM) {
        return encode_call_direct(p, inst->args_head->_s, inst->args_head->_e);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported call form");
    return INSTRUCTION_FAILED;
}

static nint encode_ret(struct Parser *p, struct AstInstruction *inst,
                       unsigned char plain_opcode,
                       unsigned char imm_opcode) {
    struct Operand imm;
    struct token *error_token = NULL;
    nint status;

    if(inst->arg_count == 0) {
        emit_byte(p, plain_opcode);
        return 1;
    }

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &imm, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid return pop count");
        return INSTRUCTION_FAILED;
    }

    if(imm.kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid return pop count");
        return INSTRUCTION_FAILED;
    }

    if(!fits_u16(imm.imm)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "return pop count does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, imm_opcode);
    emit_word(p, imm.imm);
    return 3;
}

static nint encode_xchg(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        if(dst->reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + src->reg));
            return 1;
        }
        if(src->reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + dst->reg));
            return 1;
        }
        emit_byte(p, 0x87);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x86);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(dst->size == 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x86);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(src->size == 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x86);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported xchg form");
    return INSTRUCTION_FAILED;
}

static nint emit_and_reg_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0x24);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, 4, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 4, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0x25);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 4, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    return 0;
}

static nint emit_and_mem_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, 4, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 4, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 4, &dst->mem);
        emit_word(p, src->imm);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_and(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x20);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x21);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(dst->size == 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x20);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x21);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(src->size == 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x22);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x23);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_and_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_and_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported and form");
    return INSTRUCTION_FAILED;
}

static nint emit_or_reg_imm(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0x0c);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, 1, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 1, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0x0d);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 1, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    return 0;
}

static nint emit_or_mem_imm(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, 1, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 1, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 1, &dst->mem);
        emit_word(p, src->imm);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_or(struct Parser *p, struct AstInstruction *inst,
                      struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x08);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x09);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(dst->size == 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x08);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x09);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(src->size == 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x0a);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x0b);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_or_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_or_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported or form");
    return INSTRUCTION_FAILED;
}

static nint emit_xor_reg_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0x34);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, 6, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 6, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0x35);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 6, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    return 0;
}

static nint emit_xor_mem_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, 6, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 6, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 6, &dst->mem);
        emit_word(p, src->imm);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_xor(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x30);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x31);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(dst->size == 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x30);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x31);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(src->size == 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x32);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x33);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_xor_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_xor_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported xor form");
    return INSTRUCTION_FAILED;
}

static nint emit_test_reg_imm(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0xa8);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, 0, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0xa9);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, 0, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    return 0;
}

static nint emit_test_mem_imm(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0xf6);
        emit_modrm_mem(p, 0, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0xf7);
        emit_modrm_mem(p, 0, &dst->mem);
        emit_word(p, src->imm);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_test(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x84);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x85);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(dst->size == 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x84);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(src->size == 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x84);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_test_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_test_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported test form");
    return INSTRUCTION_FAILED;
}

static nint shift_target_size(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, nint *size) {
    if(dst->kind == OPERAND_REG8) {
        *size = 1;
        return 1;
    }
    if(dst->kind == OPERAND_REG16) {
        *size = 2;
        return 1;
    }
    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1 || dst->size == 2) {
            *size = dst->size;
            return 1;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous shift operand size");
        return INSTRUCTION_FAILED;
    }
    return 0;
}

static nint emit_shift_modrm(struct Parser *p, struct Operand *dst, nint group) {
    if(dst->kind == OPERAND_MEM) {
        emit_modrm_mem(p, group, &dst->mem);
        return memory_tail_len(&dst->mem);
    }
    emit_modrm_reg(p, group, dst->reg);
    return 0;
}

static nint encode_shift_group(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               nint group) {
    nint size;
    nint tail;
    unsigned char opcode;

    nint status = shift_target_size(p, inst, dst, &size);
    if(status <= 0) {
        if(status == INSTRUCTION_FAILED) return INSTRUCTION_FAILED;
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported shift form");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "shift count does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }

        if(src->imm == 1) {
            opcode = size == 1 ? 0xd0 : 0xd1;
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, opcode);
            tail = emit_shift_modrm(p, dst, group);
            return segment_prefix_len(&dst->mem) + 2 + tail;
        }

        opcode = size == 1 ? 0xc0 : 0xc1;
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, dst, group);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return segment_prefix_len(&dst->mem) + 2 + tail + 1;
    }

    if(is_cl(src)) {
        opcode = size == 1 ? 0xd2 : 0xd3;
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, dst, group);
        return segment_prefix_len(&dst->mem) + 2 + tail;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported shift form");
    return INSTRUCTION_FAILED;
}

static nint encode_in(struct Parser *p, struct AstInstruction *inst,
                      struct Operand *dst, struct Operand *src) {
    if(is_al(dst) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe4);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    if(is_ax(dst) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe5);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    if(is_al(dst) && is_dx(src)) {
        emit_byte(p, 0xec);
        return 1;
    }

    if(is_ax(dst) && is_dx(src)) {
        emit_byte(p, 0xed);
        return 1;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported in form");
    return INSTRUCTION_FAILED;
}

static nint encode_out(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_IMM && is_al(src)) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe6);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return 2;
    }

    if(dst->kind == OPERAND_IMM && is_ax(src)) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe7);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return 2;
    }

    if(is_dx(dst) && is_al(src)) {
        emit_byte(p, 0xee);
        return 1;
    }

    if(is_dx(dst) && is_ax(src)) {
        emit_byte(p, 0xef);
        return 1;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported out form");
    return INSTRUCTION_FAILED;
}

static nint encode_intr(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *dst) {
    if(dst->kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid interrupt number");
        return INSTRUCTION_FAILED;
    }

    if(!fits_u8(dst->imm)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "interrupt number does not fit 8 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, 0xcd);
    emit_byte(p, (unsigned char)(dst->imm & 0xff));
    return 2;
}

static nint emit_arith_reg_imm(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               nint group, unsigned char acc8,
                               unsigned char acc16) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, acc8);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, group, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, group, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, acc16);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, group, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    return 0;
}

static nint emit_arith_mem_imm(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               nint group) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, group, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, group, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, group, &dst->mem);
        emit_word(p, src->imm);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_arith(struct Parser *p, struct AstInstruction *inst,
                         struct Operand *dst, struct Operand *src,
                         unsigned char base, unsigned char acc8,
                         unsigned char acc16, nint group,
                         const char *name) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, base);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(dst->size == 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, base);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_mem(p, src->reg, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(src->size == 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, (unsigned char)(base + 2));
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, (unsigned char)(base + 3));
        emit_modrm_mem(p, dst->reg, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_arith_reg_imm(p, inst, dst, src, group, acc8, acc16);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) {
            return emit_arith_mem_imm(p, inst, dst, src, group);
        }
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_not(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst) {
    if(dst->kind == OPERAND_REG8) {
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, 2, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, 2, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xf6);
            emit_modrm_mem(p, 2, &dst->mem);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 2) {
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, 2, &dst->mem);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous not operand size");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported not form");
    return INSTRUCTION_FAILED;
}

static nint encode_unary_group(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, nint group,
                               const char *name) {
    if(dst->kind == OPERAND_REG8) {
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, group, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, group, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xf6);
            emit_modrm_mem(p, group, &dst->mem);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 2) {
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, group, &dst->mem);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_aad(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *src) {
    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xd5);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported aad form");
    return INSTRUCTION_FAILED;
}

static nint encode_aam(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *src) {
    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xd4);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported aam form");
    return INSTRUCTION_FAILED;
}

static nint encode_inc_dec(struct Parser *p, struct AstInstruction *inst,
                           struct Operand *dst, nint group,
                           unsigned char reg16_base, const char *name) {
    if(dst->kind == OPERAND_REG8) {
        emit_byte(p, 0xfe);
        emit_modrm_reg(p, group, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(reg16_base + dst->reg));
        return 1;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xfe);
            emit_modrm_mem(p, group, &dst->mem);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 2) {
            emit_segment_prefix(p, &dst->mem);
            emit_byte(p, 0xff);
            emit_modrm_mem(p, group, &dst->mem);
            return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_push(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *src) {
    if(src->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(0x50 + src->reg));
        return 1;
    }

    if(src->kind == OPERAND_SEG) {
        emit_byte(p, (unsigned char)(0x06 + (src->reg << 3)));
        return 1;
    }

    if(src->kind == OPERAND_IMM) {
        if(fits_push_imm8(src->imm)) {
            emit_byte(p, 0x6a);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        if(fits_u16(src->imm)) {
            emit_byte(p, 0x68);
            emit_word(p, src->imm);
            return 3;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "immediate does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid push operand size");
            return INSTRUCTION_FAILED;
        }
        if(src->size != 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous push operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src->mem);
        emit_byte(p, 0xff);
        emit_modrm_mem(p, 6, &src->mem);
        return segment_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported push form");
    return INSTRUCTION_FAILED;
}

static nint encode_pop(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst) {
    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(0x58 + dst->reg));
        return 1;
    }

    if(dst->kind == OPERAND_SEG) {
        emit_byte(p, (unsigned char)(0x07 + (dst->reg << 3)));
        return 1;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid pop operand size");
            return INSTRUCTION_FAILED;
        }
        if(dst->size != 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous pop operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst->mem);
        emit_byte(p, 0x8f);
        emit_modrm_mem(p, 0, &dst->mem);
        return segment_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported pop form");
    return INSTRUCTION_FAILED;
}

static nint encode_esc(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *op, struct Operand *rm) {
    unsigned char opcode;
    nint group;

    if(op->kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid escape opcode");
        return INSTRUCTION_FAILED;
    }

    if(op->imm < 0 || op->imm > 63) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "escape opcode does not fit 6 bits");
        return INSTRUCTION_FAILED;
    }

    opcode = (unsigned char)(0xd8 + ((op->imm >> 3) & 7));
    group = op->imm & 7;

    if(rm->kind == OPERAND_MEM) {
        emit_segment_prefix(p, &rm->mem);
        emit_byte(p, opcode);
        emit_modrm_mem(p, group, &rm->mem);
        return segment_prefix_len(&rm->mem) + 2 + memory_tail_len(&rm->mem);
    }

    if(rm->kind == OPERAND_REG16) {
        emit_byte(p, opcode);
        emit_modrm_reg(p, group, rm->reg);
        return 2;
    }

    _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                      "unsupported esc form");
    return INSTRUCTION_FAILED;
}

static nint append_prefix_arg(struct AstInstruction *inner,
                              struct InstructionArg *args,
                              struct token *start, struct token *end) {
    struct InstructionArg *slot;
    if(inner->arg_count >= 2 || !start || !end || start > end) return 0;
    slot = &args[inner->arg_count];
    slot->_s = start;
    slot->_e = end;
    slot->next = NULL;
    if(inner->args_tail) inner->args_tail->next = slot;
    else inner->args_head = slot;
    inner->args_tail = slot;
    inner->arg_count++;
    return 1;
}

static nint build_prefixed_instruction(struct Parser *p,
                                       struct AstInstruction *inst,
                                       struct AstInstruction *inner,
                                       struct InstructionArg *inner_args) {
    struct InstructionArg *first = inst->args_head;
    struct InstructionArg *src;
    struct token *error_token;

    if(inst->arg_count == 0 || !first) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(!first->_s || !first->_e || first->_s > first->_e) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid prefixed instruction");
        return INSTRUCTION_FAILED;
    }

    if(first->_s->type != NAME) {
        _error_from_token(p, first->_s, ERROR_TYPE_I386,
                          "expected instruction after prefix");
        return INSTRUCTION_FAILED;
    }

    inner->name = first->_s;
    inner->arg_count = 0;
    inner->args_head = NULL;
    inner->args_tail = NULL;

    if(first->_s < first->_e) {
        error_token = first->_s + 1;
        if(!append_prefix_arg(inner, inner_args, error_token, first->_e)) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid prefixed instruction");
            return INSTRUCTION_FAILED;
        }
    }

    src = first->next;
    while(src) {
        error_token = src->_s ? src->_s : inner->name;
        if(!append_prefix_arg(inner, inner_args, src->_s, src->_e)) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid number of arguments");
            return INSTRUCTION_FAILED;
        }
        src = src->next;
    }

    return 1;
}

static unint rep_allows_instruction(struct token *name) {
    return token_is(name, MOVS_INSTRUCTION) || token_is(name, MOVSB_INSTRUCTION) ||
           token_is(name, MOVSW_INSTRUCTION) || token_is(name, CMPS_INSTRUCTION) ||
           token_is(name, CMPSB_INSTRUCTION) || token_is(name, CMPSW_INSTRUCTION) ||
           token_is(name, SCAS_INSTRUCTION) || token_is(name, SCASB_INSTRUCTION) ||
           token_is(name, SCASW_INSTRUCTION) || token_is(name, LODS_INSTRUCTION) ||
           token_is(name, LODSB_INSTRUCTION) || token_is(name, LODSW_INSTRUCTION) ||
           token_is(name, STOS_INSTRUCTION) || token_is(name, STOSB_INSTRUCTION) ||
           token_is(name, STOSW_INSTRUCTION);
}

static unint repe_allows_instruction(struct token *name) {
    return token_is(name, CMPS_INSTRUCTION) || token_is(name, CMPSB_INSTRUCTION) ||
           token_is(name, CMPSW_INSTRUCTION) || token_is(name, SCAS_INSTRUCTION) ||
           token_is(name, SCASB_INSTRUCTION) || token_is(name, SCASW_INSTRUCTION);
}

static nint encode_repeat_prefix(struct Parser *p, struct AstInstruction *inst,
                                 unint (*allowed)(struct token *),
                                 const char *message,
                                 unsigned char prefix) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[2];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    if(!allowed(inner.name)) {
        _error_from_token(p, inner.name, ERROR_TYPE_I386, message);
        return INSTRUCTION_FAILED;
    }

    if(inner.arg_count != 0) {
        _error_from_token(p, inner.args_head ? inner.args_head->_s : inner.name,
                          ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, prefix);
    status = _86_exec(p, &inner);
    if(status < 0) return status;
    return status + 1;
}

static nint encode_rep_prefix(struct Parser *p, struct AstInstruction *inst) {
    return encode_repeat_prefix(p, inst, rep_allows_instruction,
                                "unsupported rep instruction", 0xf3);
}

static nint encode_repe_prefix(struct Parser *p, struct AstInstruction *inst) {
    return encode_repeat_prefix(p, inst, repe_allows_instruction,
                                "unsupported repe instruction", 0xf3);
}

static nint encode_repne_prefix(struct Parser *p, struct AstInstruction *inst) {
    return encode_repeat_prefix(p, inst, repe_allows_instruction,
                                "unsupported repne instruction", 0xf2);
}

static unint lock_allows_arith(struct token *name) {
    return token_is(name, ADD_INSTRUCTION) || token_is(name, ADC_INSTRUCTION) ||
           token_is(name, AND_INSTRUCTION) || token_is(name, OR_INSTRUCTION) ||
           token_is(name, SBB_INSTRUCTION) || token_is(name, SUB_INSTRUCTION) ||
           token_is(name, XOR_INSTRUCTION);
}

static unint lock_allows_unary(struct token *name) {
    return token_is(name, INC_INSTRUCTION) || token_is(name, DEC_INSTRUCTION) ||
           token_is(name, NEG_INSTRUCTION) || token_is(name, NOT_INSTRUCTION);
}

static nint parse_lock_operand(struct Parser *p, struct InstructionArg *arg,
                               struct Operand *op, const char *message) {
    struct token *error_token = NULL;
    nint status = parse_operand(p, arg, op, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386, message);
        return INSTRUCTION_FAILED;
    }
    return 1;
}

static nint validate_lock_arith(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_lock_operand(p, inner->args_head, &dst, "invalid first operand");
    if(status <= 0) return status;
    status = parse_lock_operand(p, inner->args_tail, &src, "invalid second operand");
    if(status <= 0) return status;

    if(dst.kind != OPERAND_MEM) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "lock requires memory destination");
        return INSTRUCTION_FAILED;
    }

    if(src.kind == OPERAND_REG8) {
        if(dst.size == 2) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(src.kind == OPERAND_REG16) {
        if(dst.size == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(src.kind == OPERAND_IMM) {
        if(dst.size != 1 && dst.size != 2) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous lock operand size");
            return INSTRUCTION_FAILED;
        }
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported lock form");
    return INSTRUCTION_FAILED;
}

static nint validate_lock_xchg(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    struct Operand *mem;
    struct Operand *reg;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_lock_operand(p, inner->args_head, &dst, "invalid first operand");
    if(status <= 0) return status;
    status = parse_lock_operand(p, inner->args_tail, &src, "invalid second operand");
    if(status <= 0) return status;

    if(dst.kind == OPERAND_MEM) {
        mem = &dst;
        reg = &src;
    } else if(src.kind == OPERAND_MEM) {
        mem = &src;
        reg = &dst;
    } else {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "lock requires memory operand");
        return INSTRUCTION_FAILED;
    }

    if(reg->kind == OPERAND_REG8) {
        if(mem->size == 2) {
            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(reg->kind == OPERAND_REG16) {
        if(mem->size == 1) {
            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported lock form");
    return INSTRUCTION_FAILED;
}

static nint validate_lock_unary(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_lock_operand(p, inner->args_head, &dst, "invalid operand");
    if(status <= 0) return status;

    if(dst.kind != OPERAND_MEM) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "lock requires memory operand");
        return INSTRUCTION_FAILED;
    }

    if(dst.size != 1 && dst.size != 2) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous lock operand size");
        return INSTRUCTION_FAILED;
    }

    return 1;
}

static nint validate_lock_instruction(struct Parser *p, struct AstInstruction *inner) {
    if(lock_allows_arith(inner->name)) return validate_lock_arith(p, inner);
    if(token_is(inner->name, XCHG_INSTRUCTION)) return validate_lock_xchg(p, inner);
    if(lock_allows_unary(inner->name)) return validate_lock_unary(p, inner);

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported lock instruction");
    return INSTRUCTION_FAILED;
}

static nint encode_lock_prefix(struct Parser *p, struct AstInstruction *inst) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[2];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    status = validate_lock_instruction(p, &inner);
    if(status <= 0) return status;

    emit_byte(p, 0xf0);
    status = _86_exec(p, &inner);
    if(status < 0) return status;
    return status + 1;
}

nint _86_exec(struct Parser *p, struct AstInstruction *inst) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;
    unint is_mov = compare_identifiers_cp_array(inst->name, MOV_INSTRUCTION) == SUCCESS;
    unint is_lea = compare_identifiers_cp_array(inst->name, LEA_INSTRUCTION) == SUCCESS;
    unint is_lds = compare_identifiers_cp_array(inst->name, LDS_INSTRUCTION) == SUCCESS;
    unint is_les = compare_identifiers_cp_array(inst->name, LES_INSTRUCTION) == SUCCESS;
    unint is_jmp = compare_identifiers_cp_array(inst->name, JMP_INSTRUCTION) == SUCCESS;
    unint is_call = compare_identifiers_cp_array(inst->name, CALL_INSTRUCTION) == SUCCESS;
    unint is_ret = compare_identifiers_cp_array(inst->name, RET_INSTRUCTION) == SUCCESS;
    unint is_retn = compare_identifiers_cp_array(inst->name, RETN_INSTRUCTION) == SUCCESS;
    unint is_retf = compare_identifiers_cp_array(inst->name, RETF_INSTRUCTION) == SUCCESS;
    unint is_xchg = compare_identifiers_cp_array(inst->name, XCHG_INSTRUCTION) == SUCCESS;
    unint is_push = compare_identifiers_cp_array(inst->name, PUSH_INSTRUCTION) == SUCCESS;
    unint is_pop = compare_identifiers_cp_array(inst->name, POP_INSTRUCTION) == SUCCESS;
    unint is_pushf = compare_identifiers_cp_array(inst->name, PUSHF_INSTRUCTION) == SUCCESS;
    unint is_popf = compare_identifiers_cp_array(inst->name, POPF_INSTRUCTION) == SUCCESS;
    unint is_lahf = compare_identifiers_cp_array(inst->name, LAHF_INSTRUCTION) == SUCCESS;
    unint is_sahf = compare_identifiers_cp_array(inst->name, SAHF_INSTRUCTION) == SUCCESS;
    unint is_xlat = compare_identifiers_cp_array(inst->name, XLAT_INSTRUCTION) == SUCCESS;
    unint is_xlatb = compare_identifiers_cp_array(inst->name, XLATB_INSTRUCTION) == SUCCESS;
    unint is_movs = compare_identifiers_cp_array(inst->name, MOVS_INSTRUCTION) == SUCCESS;
    unint is_movsb = compare_identifiers_cp_array(inst->name, MOVSB_INSTRUCTION) == SUCCESS;
    unint is_movsw = compare_identifiers_cp_array(inst->name, MOVSW_INSTRUCTION) == SUCCESS;
    unint is_cmps = compare_identifiers_cp_array(inst->name, CMPS_INSTRUCTION) == SUCCESS;
    unint is_cmpsb = compare_identifiers_cp_array(inst->name, CMPSB_INSTRUCTION) == SUCCESS;
    unint is_cmpsw = compare_identifiers_cp_array(inst->name, CMPSW_INSTRUCTION) == SUCCESS;
    unint is_scas = compare_identifiers_cp_array(inst->name, SCAS_INSTRUCTION) == SUCCESS;
    unint is_scasb = compare_identifiers_cp_array(inst->name, SCASB_INSTRUCTION) == SUCCESS;
    unint is_scasw = compare_identifiers_cp_array(inst->name, SCASW_INSTRUCTION) == SUCCESS;
    unint is_lods = compare_identifiers_cp_array(inst->name, LODS_INSTRUCTION) == SUCCESS;
    unint is_lodsb = compare_identifiers_cp_array(inst->name, LODSB_INSTRUCTION) == SUCCESS;
    unint is_lodsw = compare_identifiers_cp_array(inst->name, LODSW_INSTRUCTION) == SUCCESS;
    unint is_stos = compare_identifiers_cp_array(inst->name, STOS_INSTRUCTION) == SUCCESS;
    unint is_stosb = compare_identifiers_cp_array(inst->name, STOSB_INSTRUCTION) == SUCCESS;
    unint is_stosw = compare_identifiers_cp_array(inst->name, STOSW_INSTRUCTION) == SUCCESS;
    unint is_lock = compare_identifiers_cp_array(inst->name, LOCK_INSTRUCTION) == SUCCESS;
    unint is_rep = compare_identifiers_cp_array(inst->name, REP_INSTRUCTION) == SUCCESS;
    unint is_repe = compare_identifiers_cp_array(inst->name, REPE_INSTRUCTION) == SUCCESS;
    unint is_repz = compare_identifiers_cp_array(inst->name, REPZ_INSTRUCTION) == SUCCESS;
    unint is_repne = compare_identifiers_cp_array(inst->name, REPNE_INSTRUCTION) == SUCCESS;
    unint is_repnz = compare_identifiers_cp_array(inst->name, REPNZ_INSTRUCTION) == SUCCESS;
    unint is_in = compare_identifiers_cp_array(inst->name, IN_INSTRUCTION) == SUCCESS;
    unint is_out = compare_identifiers_cp_array(inst->name, OUT_INSTRUCTION) == SUCCESS;
    unint is_intr = compare_identifiers_cp_array(inst->name, INT_INSTRUCTION) == SUCCESS;
    unint is_into = compare_identifiers_cp_array(inst->name, INTO_INSTRUCTION) == SUCCESS;
    unint is_iret = compare_identifiers_cp_array(inst->name, IRET_INSTRUCTION) == SUCCESS;
    unint is_loop = compare_identifiers_cp_array(inst->name, LOOP_INSTRUCTION) == SUCCESS;
    unint is_loope = compare_identifiers_cp_array(inst->name, LOOPE_INSTRUCTION) == SUCCESS;
    unint is_loopz = compare_identifiers_cp_array(inst->name, LOOPZ_INSTRUCTION) == SUCCESS;
    unint is_loopne = compare_identifiers_cp_array(inst->name, LOOPNE_INSTRUCTION) == SUCCESS;
    unint is_loopnz = compare_identifiers_cp_array(inst->name, LOOPNZ_INSTRUCTION) == SUCCESS;
    unint is_jcxz = compare_identifiers_cp_array(inst->name, JCXZ_INSTRUCTION) == SUCCESS;
    unint is_jo = compare_identifiers_cp_array(inst->name, JO_INSTRUCTION) == SUCCESS;
    unint is_jno = compare_identifiers_cp_array(inst->name, JNO_INSTRUCTION) == SUCCESS;
    unint is_jb = compare_identifiers_cp_array(inst->name, JB_INSTRUCTION) == SUCCESS;
    unint is_jc = compare_identifiers_cp_array(inst->name, JC_INSTRUCTION) == SUCCESS;
    unint is_jnae = compare_identifiers_cp_array(inst->name, JNAE_INSTRUCTION) == SUCCESS;
    unint is_jnb = compare_identifiers_cp_array(inst->name, JNB_INSTRUCTION) == SUCCESS;
    unint is_jae = compare_identifiers_cp_array(inst->name, JAE_INSTRUCTION) == SUCCESS;
    unint is_jnc = compare_identifiers_cp_array(inst->name, JNC_INSTRUCTION) == SUCCESS;
    unint is_je = compare_identifiers_cp_array(inst->name, JE_INSTRUCTION) == SUCCESS;
    unint is_jz = compare_identifiers_cp_array(inst->name, JZ_INSTRUCTION) == SUCCESS;
    unint is_jne = compare_identifiers_cp_array(inst->name, JNE_INSTRUCTION) == SUCCESS;
    unint is_jnz = compare_identifiers_cp_array(inst->name, JNZ_INSTRUCTION) == SUCCESS;
    unint is_jbe = compare_identifiers_cp_array(inst->name, JBE_INSTRUCTION) == SUCCESS;
    unint is_jna = compare_identifiers_cp_array(inst->name, JNA_INSTRUCTION) == SUCCESS;
    unint is_ja = compare_identifiers_cp_array(inst->name, JA_INSTRUCTION) == SUCCESS;
    unint is_jnbe = compare_identifiers_cp_array(inst->name, JNBE_INSTRUCTION) == SUCCESS;
    unint is_js = compare_identifiers_cp_array(inst->name, JS_INSTRUCTION) == SUCCESS;
    unint is_jns = compare_identifiers_cp_array(inst->name, JNS_INSTRUCTION) == SUCCESS;
    unint is_jp = compare_identifiers_cp_array(inst->name, JP_INSTRUCTION) == SUCCESS;
    unint is_jpe = compare_identifiers_cp_array(inst->name, JPE_INSTRUCTION) == SUCCESS;
    unint is_jnp = compare_identifiers_cp_array(inst->name, JNP_INSTRUCTION) == SUCCESS;
    unint is_jpo = compare_identifiers_cp_array(inst->name, JPO_INSTRUCTION) == SUCCESS;
    unint is_jl = compare_identifiers_cp_array(inst->name, JL_INSTRUCTION) == SUCCESS;
    unint is_jnge = compare_identifiers_cp_array(inst->name, JNGE_INSTRUCTION) == SUCCESS;
    unint is_jge = compare_identifiers_cp_array(inst->name, JGE_INSTRUCTION) == SUCCESS;
    unint is_jnl = compare_identifiers_cp_array(inst->name, JNL_INSTRUCTION) == SUCCESS;
    unint is_jle = compare_identifiers_cp_array(inst->name, JLE_INSTRUCTION) == SUCCESS;
    unint is_jng = compare_identifiers_cp_array(inst->name, JNG_INSTRUCTION) == SUCCESS;
    unint is_jg = compare_identifiers_cp_array(inst->name, JG_INSTRUCTION) == SUCCESS;
    unint is_jnle = compare_identifiers_cp_array(inst->name, JNLE_INSTRUCTION) == SUCCESS;
    unint is_add = compare_identifiers_cp_array(inst->name, ADD_INSTRUCTION) == SUCCESS;
    unint is_adc = compare_identifiers_cp_array(inst->name, ADC_INSTRUCTION) == SUCCESS;
    unint is_sub = compare_identifiers_cp_array(inst->name, SUB_INSTRUCTION) == SUCCESS;
    unint is_sbb = compare_identifiers_cp_array(inst->name, SBB_INSTRUCTION) == SUCCESS;
    unint is_cmp = compare_identifiers_cp_array(inst->name, CMP_INSTRUCTION) == SUCCESS;
    unint is_inc = compare_identifiers_cp_array(inst->name, INC_INSTRUCTION) == SUCCESS;
    unint is_dec = compare_identifiers_cp_array(inst->name, DEC_INSTRUCTION) == SUCCESS;
    unint is_neg = compare_identifiers_cp_array(inst->name, NEG_INSTRUCTION) == SUCCESS;
    unint is_mul = compare_identifiers_cp_array(inst->name, MUL_INSTRUCTION) == SUCCESS;
    unint is_imul = compare_identifiers_cp_array(inst->name, IMUL_INSTRUCTION) == SUCCESS;
    unint is_div = compare_identifiers_cp_array(inst->name, DIV_INSTRUCTION) == SUCCESS;
    unint is_idiv = compare_identifiers_cp_array(inst->name, IDIV_INSTRUCTION) == SUCCESS;
    unint is_cbw = compare_identifiers_cp_array(inst->name, CBW_INSTRUCTION) == SUCCESS;
    unint is_cwd = compare_identifiers_cp_array(inst->name, CWD_INSTRUCTION) == SUCCESS;
    unint is_aaa = compare_identifiers_cp_array(inst->name, AAA_INSTRUCTION) == SUCCESS;
    unint is_aad = compare_identifiers_cp_array(inst->name, AAD_INSTRUCTION) == SUCCESS;
    unint is_aam = compare_identifiers_cp_array(inst->name, AAM_INSTRUCTION) == SUCCESS;
    unint is_aas = compare_identifiers_cp_array(inst->name, AAS_INSTRUCTION) == SUCCESS;
    unint is_daa = compare_identifiers_cp_array(inst->name, DAA_INSTRUCTION) == SUCCESS;
    unint is_das = compare_identifiers_cp_array(inst->name, DAS_INSTRUCTION) == SUCCESS;
    unint is_and = compare_identifiers_cp_array(inst->name, AND_INSTRUCTION) == SUCCESS;
    unint is_or = compare_identifiers_cp_array(inst->name, OR_INSTRUCTION) == SUCCESS;
    unint is_xor = compare_identifiers_cp_array(inst->name, XOR_INSTRUCTION) == SUCCESS;
    unint is_test = compare_identifiers_cp_array(inst->name, TEST_INSTRUCTION) == SUCCESS;
    unint is_not = compare_identifiers_cp_array(inst->name, NOT_INSTRUCTION) == SUCCESS;
    unint is_clc = compare_identifiers_cp_array(inst->name, CLC_INSTRUCTION) == SUCCESS;
    unint is_stc = compare_identifiers_cp_array(inst->name, STC_INSTRUCTION) == SUCCESS;
    unint is_cmc = compare_identifiers_cp_array(inst->name, CMC_INSTRUCTION) == SUCCESS;
    unint is_cld = compare_identifiers_cp_array(inst->name, CLD_INSTRUCTION) == SUCCESS;
    unint is_std = compare_identifiers_cp_array(inst->name, STD_INSTRUCTION) == SUCCESS;
    unint is_cli = compare_identifiers_cp_array(inst->name, CLI_INSTRUCTION) == SUCCESS;
    unint is_sti = compare_identifiers_cp_array(inst->name, STI_INSTRUCTION) == SUCCESS;
    unint is_hlt = compare_identifiers_cp_array(inst->name, HLT_INSTRUCTION) == SUCCESS;
    unint is_wait = compare_identifiers_cp_array(inst->name, WAIT_INSTRUCTION) == SUCCESS;
    unint is_fwait = compare_identifiers_cp_array(inst->name, FWAIT_INSTRUCTION) == SUCCESS;
    unint is_esc = compare_identifiers_cp_array(inst->name, ESC_INSTRUCTION) == SUCCESS;
    unint is_nop = compare_identifiers_cp_array(inst->name, NOP_INSTRUCTION) == SUCCESS;
    unint is_shl = compare_identifiers_cp_array(inst->name, SHL_INSTRUCTION) == SUCCESS;
    unint is_sal = compare_identifiers_cp_array(inst->name, SAL_INSTRUCTION) == SUCCESS;
    unint is_shr = compare_identifiers_cp_array(inst->name, SHR_INSTRUCTION) == SUCCESS;
    unint is_sar = compare_identifiers_cp_array(inst->name, SAR_INSTRUCTION) == SUCCESS;
    unint is_rol = compare_identifiers_cp_array(inst->name, ROL_INSTRUCTION) == SUCCESS;
    unint is_ror = compare_identifiers_cp_array(inst->name, ROR_INSTRUCTION) == SUCCESS;
    unint is_rcl = compare_identifiers_cp_array(inst->name, RCL_INSTRUCTION) == SUCCESS;
    unint is_rcr = compare_identifiers_cp_array(inst->name, RCR_INSTRUCTION) == SUCCESS;

    (void)registers;

    if(is_lock) return encode_lock_prefix(p, inst);
    if(is_repe || is_repz) return encode_repe_prefix(p, inst);
    if(is_repne || is_repnz) return encode_repne_prefix(p, inst);
    if(is_rep) return encode_rep_prefix(p, inst);

    if(!is_mov && !is_lea && !is_lds && !is_les && !is_jmp && !is_call &&
       !is_ret && !is_retn && !is_retf && !is_xchg &&
       !is_push && !is_pop && !is_pushf && !is_popf && !is_lahf && !is_sahf &&
       !is_xlat && !is_xlatb && !is_movs && !is_movsb && !is_movsw &&
       !is_cmps && !is_cmpsb && !is_cmpsw && !is_scas && !is_scasb && !is_scasw &&
       !is_lods && !is_lodsb && !is_lodsw && !is_stos && !is_stosb && !is_stosw &&
       !is_in && !is_out && !is_intr && !is_into && !is_iret &&
       !is_loop && !is_loope && !is_loopz && !is_loopne && !is_loopnz &&
       !is_jcxz && !is_jo && !is_jno && !is_jb && !is_jc && !is_jnae &&
       !is_jnb && !is_jae && !is_jnc && !is_je && !is_jz && !is_jne && !is_jnz &&
       !is_jbe && !is_jna && !is_ja && !is_jnbe &&
       !is_js && !is_jns && !is_jp && !is_jpe && !is_jnp && !is_jpo &&
       !is_jl && !is_jnge && !is_jge && !is_jnl &&
       !is_jle && !is_jng && !is_jg && !is_jnle &&
       !is_add && !is_adc && !is_sub && !is_sbb && !is_cmp &&
       !is_inc && !is_dec && !is_neg && !is_mul && !is_imul && !is_div && !is_idiv &&
       !is_cbw && !is_cwd && !is_aaa && !is_aad && !is_aam && !is_aas &&
       !is_daa && !is_das && !is_and && !is_or &&
       !is_xor && !is_test && !is_not && !is_clc && !is_stc && !is_cmc &&
       !is_cld && !is_std && !is_cli && !is_sti && !is_hlt && !is_wait &&
       !is_fwait && !is_esc && !is_nop && !is_shl && !is_sal &&
       !is_shr && !is_sar && !is_rol && !is_ror && !is_rcl && !is_rcr) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid i386 instruction");
        return INSTRUCTION_FAILED;
    }

    if((is_pushf || is_popf || is_lahf || is_sahf || is_xlat || is_xlatb ||
        is_movs || is_movsb || is_movsw || is_cmps || is_cmpsb || is_cmpsw ||
        is_scas || is_scasb || is_scasw || is_lods || is_lodsb || is_lodsw ||
        is_stos || is_stosb || is_stosw || is_clc || is_stc || is_cmc ||
        is_cld || is_std || is_cli || is_sti || is_hlt || is_wait || is_fwait ||
        is_nop || is_cbw || is_cwd ||
        is_aaa || is_aas || is_daa || is_das || is_into || is_iret) &&
       inst->arg_count != 0) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if((is_aad || is_aam) && inst->arg_count > 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if((is_push || is_pop || is_not || is_inc || is_dec || is_neg || is_intr ||
        is_mul || is_imul || is_div || is_idiv) &&
       inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(!is_jmp && !is_call && !is_ret && !is_retn && !is_retf &&
       !is_loop && !is_loope && !is_loopz && !is_loopne && !is_loopnz &&
       !is_jcxz && !is_jo && !is_jno && !is_jb && !is_jc && !is_jnae &&
       !is_jnb && !is_jae && !is_jnc && !is_je && !is_jz && !is_jne && !is_jnz &&
       !is_jbe && !is_jna && !is_ja && !is_jnbe &&
       !is_js && !is_jns && !is_jp && !is_jpe && !is_jnp && !is_jpo &&
       !is_jl && !is_jnge && !is_jge && !is_jnl &&
       !is_jle && !is_jng && !is_jg && !is_jnle &&
       !is_push && !is_pop && !is_not && !is_inc && !is_dec && !is_neg &&
       !is_mul && !is_imul && !is_div && !is_idiv &&
       !is_pushf && !is_popf &&
       !is_lahf && !is_sahf && !is_xlat && !is_xlatb && !is_movs &&
       !is_movsb && !is_movsw && !is_cmps && !is_cmpsb && !is_cmpsw &&
       !is_scas && !is_scasb && !is_scasw && !is_lods && !is_lodsb && !is_lodsw &&
       !is_stos && !is_stosb && !is_stosw && !is_clc && !is_stc && !is_cmc &&
       !is_cld && !is_std && !is_cli && !is_sti && !is_hlt && !is_wait &&
       !is_fwait && !is_nop && !is_cbw && !is_cwd &&
       !is_aaa && !is_aad && !is_aam && !is_aas && !is_daa && !is_das &&
       !is_intr && !is_into && !is_iret &&
       !is_loop && !is_loope && !is_loopz && !is_loopne && !is_loopnz &&
       !is_jcxz && !is_jo && !is_jno && !is_jb && !is_jc && !is_jnae &&
       !is_jnb && !is_jae && !is_jnc && !is_je && !is_jz && !is_jne && !is_jnz &&
       !is_jbe && !is_jna && !is_ja && !is_jnbe &&
       !is_js && !is_jns && !is_jp && !is_jpe && !is_jnp && !is_jpo &&
       !is_jl && !is_jnge && !is_jge && !is_jnl &&
       !is_jle && !is_jng && !is_jg && !is_jnle &&
       inst->arg_count != 2) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(is_pushf) {
        emit_byte(p, 0x9c);
        return 1;
    }
    if(is_popf) {
        emit_byte(p, 0x9d);
        return 1;
    }
    if(is_lahf) {
        emit_byte(p, 0x9f);
        return 1;
    }
    if(is_sahf) {
        emit_byte(p, 0x9e);
        return 1;
    }
    if(is_xlat || is_xlatb) {
        emit_byte(p, 0xd7);
        return 1;
    }
    if(is_movsb) {
        emit_byte(p, 0xa4);
        return 1;
    }
    if(is_movs || is_movsw) {
        emit_byte(p, 0xa5);
        return 1;
    }
    if(is_cmpsb) {
        emit_byte(p, 0xa6);
        return 1;
    }
    if(is_cmps || is_cmpsw) {
        emit_byte(p, 0xa7);
        return 1;
    }
    if(is_scasb) {
        emit_byte(p, 0xae);
        return 1;
    }
    if(is_scas || is_scasw) {
        emit_byte(p, 0xaf);
        return 1;
    }
    if(is_lodsb) {
        emit_byte(p, 0xac);
        return 1;
    }
    if(is_lods || is_lodsw) {
        emit_byte(p, 0xad);
        return 1;
    }
    if(is_stosb) {
        emit_byte(p, 0xaa);
        return 1;
    }
    if(is_stos || is_stosw) {
        emit_byte(p, 0xab);
        return 1;
    }
    if(is_cbw) {
        emit_byte(p, 0x98);
        return 1;
    }
    if(is_cwd) {
        emit_byte(p, 0x99);
        return 1;
    }
    if(is_aaa) {
        emit_byte(p, 0x37);
        return 1;
    }
    if(is_aad && inst->arg_count == 0) {
        emit_byte(p, 0xd5);
        emit_byte(p, 0x0a);
        return 2;
    }
    if(is_aam && inst->arg_count == 0) {
        emit_byte(p, 0xd4);
        emit_byte(p, 0x0a);
        return 2;
    }
    if(is_aas) {
        emit_byte(p, 0x3f);
        return 1;
    }
    if(is_daa) {
        emit_byte(p, 0x27);
        return 1;
    }
    if(is_das) {
        emit_byte(p, 0x2f);
        return 1;
    }
    if(is_clc) {
        emit_byte(p, 0xf8);
        return 1;
    }
    if(is_stc) {
        emit_byte(p, 0xf9);
        return 1;
    }
    if(is_cmc) {
        emit_byte(p, 0xf5);
        return 1;
    }
    if(is_cld) {
        emit_byte(p, 0xfc);
        return 1;
    }
    if(is_std) {
        emit_byte(p, 0xfd);
        return 1;
    }
    if(is_cli) {
        emit_byte(p, 0xfa);
        return 1;
    }
    if(is_sti) {
        emit_byte(p, 0xfb);
        return 1;
    }
    if(is_hlt) {
        emit_byte(p, 0xf4);
        return 1;
    }
    if(is_wait || is_fwait) {
        emit_byte(p, 0x9b);
        return 1;
    }
    if(is_nop) {
        emit_byte(p, 0x90);
        return 1;
    }
    if(is_into) {
        emit_byte(p, 0xce);
        return 1;
    }
    if(is_iret) {
        emit_byte(p, 0xcf);
        return 1;
    }

    if(is_jmp) return encode_jmp(p, inst);
    if(is_loop) return encode_rel8_control(p, inst, 0xe2);
    if(is_loope || is_loopz) return encode_rel8_control(p, inst, 0xe1);
    if(is_loopne || is_loopnz) return encode_rel8_control(p, inst, 0xe0);
    if(is_jcxz) return encode_rel8_control(p, inst, 0xe3);
    if(is_jo) return encode_rel8_control(p, inst, 0x70);
    if(is_jno) return encode_rel8_control(p, inst, 0x71);
    if(is_jb || is_jc || is_jnae) return encode_rel8_control(p, inst, 0x72);
    if(is_jnb || is_jae || is_jnc) return encode_rel8_control(p, inst, 0x73);
    if(is_je || is_jz) return encode_rel8_control(p, inst, 0x74);
    if(is_jne || is_jnz) return encode_rel8_control(p, inst, 0x75);
    if(is_jbe || is_jna) return encode_rel8_control(p, inst, 0x76);
    if(is_ja || is_jnbe) return encode_rel8_control(p, inst, 0x77);
    if(is_js) return encode_rel8_control(p, inst, 0x78);
    if(is_jns) return encode_rel8_control(p, inst, 0x79);
    if(is_jp || is_jpe) return encode_rel8_control(p, inst, 0x7a);
    if(is_jnp || is_jpo) return encode_rel8_control(p, inst, 0x7b);
    if(is_jl || is_jnge) return encode_rel8_control(p, inst, 0x7c);
    if(is_jge || is_jnl) return encode_rel8_control(p, inst, 0x7d);
    if(is_jle || is_jng) return encode_rel8_control(p, inst, 0x7e);
    if(is_jg || is_jnle) return encode_rel8_control(p, inst, 0x7f);
    if(is_call) return encode_call(p, inst);
    if(is_ret || is_retn) return encode_ret(p, inst, 0xc3, 0xc2);
    if(is_retf) return encode_ret(p, inst, 0xcb, 0xca);

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(is_push) return encode_push(p, inst, &dst);
    if(is_pop) return encode_pop(p, inst, &dst);
    if(is_intr) return encode_intr(p, inst, &dst);
    if(is_not) return encode_not(p, inst, &dst);
    if(is_inc) return encode_inc_dec(p, inst, &dst, 0, 0x40, "inc");
    if(is_dec) return encode_inc_dec(p, inst, &dst, 1, 0x48, "dec");
    if(is_neg) return encode_unary_group(p, inst, &dst, 3, "neg");
    if(is_mul) return encode_unary_group(p, inst, &dst, 4, "mul");
    if(is_imul) return encode_unary_group(p, inst, &dst, 5, "imul");
    if(is_div) return encode_unary_group(p, inst, &dst, 6, "div");
    if(is_idiv) return encode_unary_group(p, inst, &dst, 7, "idiv");
    if(is_aad) return encode_aad(p, inst, &dst);
    if(is_aam) return encode_aam(p, inst, &dst);

    error_token = NULL;
    status = parse_operand(p, inst->args_tail, &src, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(is_mov) {
        return encode_mov(p, inst, &dst, &src);
    }
    if(is_lds) {
        return encode_far_load(p, inst, &dst, &src, 0xc5);
    }
    if(is_les) {
        return encode_far_load(p, inst, &dst, &src, 0xc4);
    }
    if(is_xchg) {
        return encode_xchg(p, inst, &dst, &src);
    }
    if(is_in) {
        return encode_in(p, inst, &dst, &src);
    }
    if(is_out) {
        return encode_out(p, inst, &dst, &src);
    }
    if(is_esc) {
        return encode_esc(p, inst, &dst, &src);
    }
    if(is_add) {
        return encode_arith(p, inst, &dst, &src, 0x00, 0x04, 0x05, 0, "add");
    }
    if(is_adc) {
        return encode_arith(p, inst, &dst, &src, 0x10, 0x14, 0x15, 2, "adc");
    }
    if(is_sub) {
        return encode_arith(p, inst, &dst, &src, 0x28, 0x2c, 0x2d, 5, "sub");
    }
    if(is_sbb) {
        return encode_arith(p, inst, &dst, &src, 0x18, 0x1c, 0x1d, 3, "sbb");
    }
    if(is_cmp) {
        return encode_arith(p, inst, &dst, &src, 0x38, 0x3c, 0x3d, 7, "cmp");
    }
    if(is_and) {
        return encode_and(p, inst, &dst, &src);
    }
    if(is_or) {
        return encode_or(p, inst, &dst, &src);
    }
    if(is_xor) {
        return encode_xor(p, inst, &dst, &src);
    }
    if(is_test) {
        return encode_test(p, inst, &dst, &src);
    }
    if(is_shl || is_sal) {
        return encode_shift_group(p, inst, &dst, &src, 4);
    }
    if(is_shr) {
        return encode_shift_group(p, inst, &dst, &src, 5);
    }
    if(is_sar) {
        return encode_shift_group(p, inst, &dst, &src, 7);
    }
    if(is_rol) {
        return encode_shift_group(p, inst, &dst, &src, 0);
    }
    if(is_ror) {
        return encode_shift_group(p, inst, &dst, &src, 1);
    }
    if(is_rcl) {
        return encode_shift_group(p, inst, &dst, &src, 2);
    }
    if(is_rcr) {
        return encode_shift_group(p, inst, &dst, &src, 3);
    }
    return encode_lea(p, inst, &dst, &src);
}