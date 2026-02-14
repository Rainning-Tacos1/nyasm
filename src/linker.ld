SECTIONS {
    .asm_langs : {
        asm_langs = . ;
        KEEP(*(.asm_langs));
        asm_langs_end = . ;
    }
} INSERT AFTER .data;