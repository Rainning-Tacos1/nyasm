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
#define PUSH_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', -1})
#define POP_INSTRUCTION ((int32_t[]){'p', 'o', 'p', -1})

nint _86_exec(struct Parser* p, struct AstInstruction* inst);

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

nint _86_exec(struct Parser *p, struct AstInstruction *inst) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;
    unint is_mov = compare_identifiers_cp_array(inst->name, MOV_INSTRUCTION) == SUCCESS;
    unint is_lea = compare_identifiers_cp_array(inst->name, LEA_INSTRUCTION) == SUCCESS;
    unint is_push = compare_identifiers_cp_array(inst->name, PUSH_INSTRUCTION) == SUCCESS;
    unint is_pop = compare_identifiers_cp_array(inst->name, POP_INSTRUCTION) == SUCCESS;

    (void)registers;

    if(!is_mov && !is_lea && !is_push && !is_pop) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid i386 instruction");
        return INSTRUCTION_FAILED;
    }

    if((is_push || is_pop) && inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(!is_push && !is_pop && inst->arg_count != 2) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status == INSTRUCTION_UNRESOLVED) return INSTRUCTION_UNRESOLVED;
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(is_push) return encode_push(p, inst, &dst);
    if(is_pop) return encode_pop(p, inst, &dst);

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
    return encode_lea(p, inst, &dst, &src);
}