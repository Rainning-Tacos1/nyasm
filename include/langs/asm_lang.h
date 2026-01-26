#ifndef ASM_LANG_H
#define ASM_LANG_H

struct asm_lang_t {
	const char* const lang_name;
	const char* const code_name;
};

extern struct asm_lang_t asm_langs[];
extern struct asm_lang_t asm_langs_end[];


#define langs_count() \
	((struct asm_lang_t*)&asm_langs_end - (struct asm_lang_t*)&asm_langs)

#define ASM_LANG __attribute__((used, section(".asm_langs"))) struct asm_lang_t

#endif