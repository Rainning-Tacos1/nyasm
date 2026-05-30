#include <stdio.h>

#include "asm_lang.h"
#include "../../src/core/helper.h"
#include "types.h"

#include "api/debug.h"

#define ERROR_TYPE_I386 "i386Error"

#define MOV_INSTRUCTION ((int32_t[]){'m', 'o', 'v', -1})

#include <stdint.h>

int32_t *registers[] = {
    // General purpose registers
    (int32_t[]){'a','x', -1},
    (int32_t[]){'b','x', -1},
    (int32_t[]){'c','x', -1},
    (int32_t[]){'d','x', -1},

    // Pointer / index registers
    (int32_t[]){'s','p', -1},
    (int32_t[]){'b','p', -1},
    (int32_t[]){'s','i', -1},
    (int32_t[]){'d','i', -1},
    (int32_t[]){'i','p', -1},

    // Segment registers
    (int32_t[]){'c','s', -1},
    (int32_t[]){'d','s', -1},
    (int32_t[]){'e','s', -1},
    (int32_t[]){'s','s', -1},

    // 8-bit registers
    (int32_t[]){'a','l', -1},
    (int32_t[]){'a','h', -1},
    (int32_t[]){'b','l', -1},
    (int32_t[]){'b','h', -1},
    (int32_t[]){'c','l', -1},
    (int32_t[]){'c','h', -1},
    (int32_t[]){'d','l', -1},
    (int32_t[]){'d','h', -1},

    NULL
};


nint _86_exec(struct Parser* p, struct AstInstruction* inst);

ASM_LANG i386 = {
    .lang_name = "i386 assembly v0.0.1",
    .code_name = "i386",
    .max_addr = 0Xffff,
    .size_of_ptr = 2,
    .le = 1,

    .exec = _86_exec,
};

nint _86_exec(struct Parser* p, struct AstInstruction* inst) {

    if(compare_identifiers_cp_array(inst->name, MOV_INSTRUCTION) == SUCCESS) {
        if(inst->arg_count != 2) {
            _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
            return INSTRUCTION_FAILED;
        }

        struct TokenStream tks;
        tks_init(&tks, inst->args_head->_s, inst->args_head->_e);

        unint idx;
        if(parse_potential_register(&tks, registers, &idx) == SUCCESS) {
            DBG(1, "ITS A REGISTER\n");
            return 3;
        }
        tks_reset_peek(&tks);

        nint addr;
        unint status = parse_potential_variable(p, &tks, &addr);
        if(status == VP_FAIL) return INSTRUCTION_FAILED;
        else if(status == VP_SUCCESS) {
            DBG(1, "ITS A VARIABLE: 0x%x\n", addr);
            return 2;
        }
        tks_reset_peek(&tks);

        // language's custom syntax

        return 5; // 5 bytes for now
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "not implemented yet");

    }
    else {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid i386 instruction");
        return INSTRUCTION_FAILED;
    }

    return INSTRUCTION_FAILED;
    
}
