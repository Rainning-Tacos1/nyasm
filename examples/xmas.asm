@code _6502

@org 0x8000

@include "examples/lcd_lib.asm"

RESET = $
reset:
 
LCD_INIT()

lda !0b01000000 // Set CGRAM Addr 
LCD_INSTRUCTION()

ldx !0x00

load:
cpx !0x40
beq here
lda CGRAM, x
PRINT_CHAR()
inx
jmp load

here:
lda !0b00000010
LCD_INSTRUCTION()

ldx !0x00

display:

cpx !0x50
beq loop
lda lcd, x
PRINT_CHAR()
inx
jmp display

jmp loop


loop:
jmp loop

@struct CGRAM
    @byte[8] data

@struct LCD
    @byte[80] data

@align 0xe0b0

CGRAM:
// CGRAM 1
@struct CGRAM CGRAM1
    data = [
        0b00000100,
        0b00001110,
        0b00001110,
        0b00000100,
        0b00001110,
        0b00011111,
        0b00000100,
        0b00000100
    ]

// CGRAM 2
@struct CGRAM CGRAM2
    data = [
        0b00001010,
        0b00010101,
        0b00001110,
        0b00011011,
        0b00011011,
        0b00000000,
        0b00011011,
        0b00011011
    ]
// CGRAM 3
@struct CGRAM CGRAM3
    data = [
        0b00000000,
        0b00000000,
        0b00011000,
        0b00011101,
        0b00011111,
        0b00001110,
        0b00001010,
        0b00011111
    ]
// CGRAM 4
@struct CGRAM CGRAM4
    data = [
        0b00000000,
        0b00000000,
        0b00000001,
        0b00000011,
        0b00011111,
        0b00001111,
        0b00001010,
        0b00001010
    ]
// CGRAM 5
@struct CGRAM CGRAM5
    data = [
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00010001,
        0b00001110,
        0b00000000,
        0b00000000
    ]
// CGRAM 6
@struct CGRAM CGRAM6
    data = [
        0b00001000,
        0b00001100,
        0b00001110,
        0b00011111,
        0b00001110,
        0b00001110,
        0b00001010,
        0b00001010
    ]
// CGRAM 7
@struct CGRAM CGRAM7
    data = [
        0b00011111,
        0b00000000,
        0b00000000,
        0b00011110,
        0b00011110,
        0b00000000,
        0b00000000,
        0b00011111
    ]
// CGRAM 8
@struct CGRAM CGRAM8
    data = [
        0b00010000,
        0b00000010,
        0b00001000,
        0b00000001,
        0b00000100,
        0b00010000,
        0b00000001,
        0b00001000
    ]

@word 0x0000

lcd:
@struct LCD LCD
    data = [
        0x07, 0x07, 0x07, 0x01, 0x07, 0x07, 0x07, 0x07, 0x01, 0x07, 0x07, 0x07, 0x02, 0x04, 0x03, 0x04, 0x03, 0x04, 0x03, 0x07,
        0x00, 0x05, 0x07, 0x07, 0x00, 0x00, 0x05, 0x05, 0x07, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x05, 0x05, 0x00,
        0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
        0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06
    ]

// Vectors

@align 0xfffc
@word RESET
@word 0x0000