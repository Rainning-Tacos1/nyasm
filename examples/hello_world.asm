@code i386

org = 0x7c00
@org org

start:
mov si, message

print_loop:
lodsb
or al, al
jz done

mov ah, 0x0E
mov bh, 0
mov bl, 0x07
int 0x10

jmp print_loop

done:
hlt

message:
@string 'Hello, World!\0'

@assert ($ - org) <= 446
@align (org + 510)

@word 0xAA55