@code i386

org = 0x7c00
@org org

start:
xor ebx, ebx
mov edx, 0x8000
mov cx, 0x16

tri10:
mov ebx, edx

tri15:
test edx, edx
je tri20
mov al, 42
test dl, 1
jne tri18
mov al, 32

tri18:
mov ah, 0x0E
int 0x10
shr edx, 1
jmp tri15

tri20:
mov al, 0x0D
mov ah, 0x0E
int 0x10
mov al, 0x0A
mov ah, 0x0E
int 0x10
shl ebx, 1
xor edx, ebx
shr ebx, 2
xor edx, ebx
dec ecx
cmp ecx, 6
jne tri10

@assert ($ - org) <= 446
@align (org + 510)
@word 0xAA55