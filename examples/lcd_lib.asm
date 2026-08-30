PORTB = 0x6000
PORTA = 0x6001
DDRB = 0x6002
DDRA = 0x6003

E  = 0b10000000
RW = 0b01000000
RS = 0b00100000

RW_OR_E = (RW | E)
RS_OR_E = (RS | E)

@macro LCD_INSTRUCTION()
    sta PORTB
    lda !0         // Clear RS/RW/E bits
    sta PORTA
    lda !E         // Set E bit to send instruction
    sta PORTA
    lda !0         // Clear RS/RW/E bits
    sta PORTA


@macro PRINT_CHAR()
    sta PORTB
    lda !RS         // Set RS; Clear RW/E bits
    sta PORTA
    lda !RS_OR_E    // Set E bit to send instruction
    sta PORTA
    lda !RS         // Clear E bits
    sta PORTA

@macro LCD_INIT()
    lda !0b11111111 // Set all pins on port B to output
    sta DDRB
    lda !0b11100000 // Set top 3 pins on port A to output
    sta DDRA

    lda !0b00111000 // Set 8-bit mode; 2-line display; 5x8 font
    LCD_INSTRUCTION()
    lda !0b00001110 // Display on; cursor on; blink off
    LCD_INSTRUCTION()
    lda !0b00000110 // Increment and shift cursor; don't shift display
    LCD_INSTRUCTION()
    lda !0b00000001 // Clear display
    LCD_INSTRUCTION()