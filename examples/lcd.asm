@code _6502

@org 0x8000

@include "examples/lcd_lib.asm"

RESET = $
reset:
	
ldx !0xff
txs
 
LCD_INIT()

ldx !0

print:
lda message,x
beq loop
PRINT_CHAR()
inx
jmp print

loop:
jmp loop

message:
@string "Hello, world!\0"

// Vectors
@align 0xfffc
@word RESET
@word 0x0000