#ifndef ASM_LANG_H
#define ASM_LANG_H

#include <stdint.h>
#include "../src/core/ast.h"
#include "../src/core/parser.h"
#include "types.h"

#define INSTRUCTION_FAILED -1
#define INSTRUCTION_UNRESOLVED -2

typedef nint (*instruction_t)(struct Parser*, struct AstInstruction* inst);

struct asm_lang_t {
    const char* const lang_name;
    const char* const code_name;

    nint max_addr;
    unint size_of_ptr;

    instruction_t exec;
};

extern struct asm_lang_t asm_langs[];
extern struct asm_lang_t asm_langs_end[];


#define langs_count() \
    ((struct asm_lang_t*)&asm_langs_end - (struct asm_lang_t*)&asm_langs)

#define ASM_LANG __attribute__((used, section(".asm_langs"))) struct asm_lang_t

#endif