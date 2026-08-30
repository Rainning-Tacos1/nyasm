// vim: set ft=fasm:

// Notes about code style:
//   * I tend place extra labels to describe code a bit more
//   * Commented out instructions (especially jumps) are there to aid
//     refactoring. Having these comments allows to quickly uncomment and move
//     code around. Of course this is very optimized code so care must be taken
//     to ensure registers are containing expected values when moving code
//     around.

@code i386

debug = 0
mbr = 1
BOOT_SIGNATURE_SIZE = 2

// No point in assuming 0x7C00, when other possibilities must be handled
@org 0

DIR_RIGHT       = 0b00000000
DIR_LEFT        = 0b01100000
DIR_DOWN        = 0b00100000
DIR_UP          = 0b01000000
DIR_MASK        = DIR_RIGHT | DIR_LEFT | DIR_DOWN | DIR_UP

DIR_STOP_FLAG   = 0b10000000
DIR_STOP_MASK   = DIR_STOP_FLAG
DIR_FLAGS_MASK  = DIR_MASK | DIR_STOP_MASK
@if debug == 1
    // reduce LENGTH data, to trigger edge cases more often
    LENGTH_MASK     = 0b00000011
@else
    LENGTH_MASK     = ~DIR_FLAGS_MASK

SNAKE_HEAD      = 0x0E00 | 64 // '@'
SNAKE_BODY      = 0x0E00 | 79 // 'O'
VIDEO_MEMORY    = 0xb800
BORDER          = 0x3000
FOOD            = 0x0a00 | 42 // '*'
VOID            = 0x0000

BUTTON_UP       = 0x48
BUTTON_RIGHT    = 0x4d
BUTTON_DOWN     = 0x50
BUTTON_LEFT     = 0x4b
BUTTON_SPACE    = 0x39

SCREEN_WIDTH = 80
SCREEN_HEIGHT = 25
BYTES_PER_CHAR = 2

RAND_STATE_ADDR = 0xfffb
HIGH_SCORE_ADDR = 0xfff9
SCORE_ADDR = 0xfff7

INITIAL_SNAKE_X = 20
INITIAL_SNAKE_Y = 12

@if debug == 1
    INITIAL_SNAKE_SEGEMENT = DIR_LEFT | DIR_STOP_FLAG | 0b00000011
@else
    INITIAL_SNAKE_SEGEMENT = DIR_LEFT | DIR_STOP_FLAG | 4

SCORE_ZERO_CHAR = 0x3f30
FOOD_PEACES = 8

THROTTLE = 4


_start = $
start:
// initialize stack
mov sp, 0xffff
mov ax, cs
mov ss, ax

// Get initial IP value
call save_ip_1
_save_ip_1 = $
save_ip_1:
pop bp
tmp = _save_ip_1 - _start
sub bp, tmp // adjustment to get Initial IP value

// Initialize rand_state
@byte[2] [0x0F, 0x31] // rdtsc
push eax

xor ax, ax
push ax // high score
push ax // score

push cs
pop ds

// es stores video mem segment
push VIDEO_MEMORY
pop es

_game_over = $
game_over:
mov sp, SCORE_ADDR
pop ax // score
pop bx // high score
cmp ax, bx
jbe keep_hi_score_1
push ax // Update high score
jmp reset_1
_keep_hi_score_1 = $
keep_hi_score_1:
push bx // Push high score back
_reset_1 = $
reset_1:

// paint everything with BORDER
cld
tmp = SCREEN_WIDTH * SCREEN_WIDTH
mov cx, tmp
mov ax, BORDER
xor di, di
rep stosw
// clear playground
xchg ax, cx // ax = 0
tmp = SCREEN_HEIGHT - 2
mov cx, tmp
tmp = (SCREEN_WIDTH + 1) * BYTES_PER_CHAR
mov di, tmp
_clear_line_1 = $
clear_line_1:
push cx
tmp = SCREEN_WIDTH - 2
mov cx, tmp
rep stosw
tmp = 2 * BYTES_PER_CHAR
add di, tmp
pop cx
loop clear_line_1

push ax // ax==0 clear score after game_over (0)

// hide blinking cursor
mov cx, 0x2000
inc ah
int 0x10

// initial snake head position
tmp = (INITIAL_SNAKE_Y * SCREEN_WIDTH + INITIAL_SNAKE_X) * BYTES_PER_CHAR
push word tmp

// Rules for the mail loop:
// si must point to snake head (snake_data)
// di must point to tail segment of snake
mov si, snake_data
mov di, si // at the beginning there is only one segment
mov byte ptr [bp + si], INITIAL_SNAKE_SEGEMENT


tmp = FOOD_PEACES - 1
mov cx, tmp
_place_food_1 = $
place_food_1:
call place_food
loop place_food_1
_place_more_food = $
place_more_food:
call place_food
_main_loop = $
main_loop:


_delay = $
delay:
_retry_1 = $
retry_1:
xor ax, ax
int 0x1a
shl ecx, 16
mov cx, dx
lea ebx, [ecx + THROTTLE]
_delay_1 = $
delay_1:
int 0x1a
// The simples/shortest way to handle day change.
// Worst case scenario: You play around midnight and game would freez for ~
// 2x normal
or al, al
jnz retry_1

shl ecx, 16
mov cx, dx

cmp ecx, ebx
jl delay_1


_handle_keys = $
handle_keys:
mov ah, 1
int 0x16
jz exit_1
xor ax, ax
int 0x16

@if debug == 1
    cmp ah, BUTTON_SPACE
    jne skip_debug_1
    int3
    _skip_debug_1 = $
    skip_debug_1:

cmp ah, BUTTON_UP
mov dl, DIR_DOWN
je prefix_head_1

cmp ah, BUTTON_RIGHT
mov dl, DIR_LEFT
je prefix_head_1

cmp ah, BUTTON_DOWN
mov dl, DIR_UP
je prefix_head_1

cmp ah, BUTTON_LEFT
mov dl, DIR_RIGHT
jne exit_1 // je .prefix_head; jmp .exit

_prefix_head_1 = $
prefix_head_1:
call prefix_head
_exit_1 = $
exit_1:


_move_body = $
move_body:
_move_neck_1 = $
move_neck_1:
mov dh, [bp + si]
mov dl, dh
and dh, LENGTH_MASK
cmp dh, LENGTH_MASK
jne skip_prefix_head_1

and dl, DIR_MASK
call prefix_head

_skip_prefix_head_1 = $
skip_prefix_head_1:
inc byte ptr [bp + si]

_move_tail_1 = $
move_tail_1:
dec byte ptr [bp + di]
mov dl, [bp + di]
and dl, LENGTH_MASK
or dl, dl
jnz exit_2

_cut_off_tail_1 = $
cut_off_tail_1:
dec di
mov dl, [bp + di]
or dl, DIR_STOP_FLAG
mov [bp + di], dl
_exit_2 = $
exit_2:


_update_position = $
update_position:
pop ax
_update_head_1 = $
update_head_1:
mov dl, byte ptr [bp + si] // expecting si to point to head of snake
and dl, DIR_MASK    // get direction

mov bx, BYTES_PER_CHAR
cmp dl, DIR_RIGHT
je moving_n_1
cmp dl, DIR_LEFT
je moving_p_1

tmp = SCREEN_WIDTH * BYTES_PER_CHAR
mov bx, tmp
cmp dl, DIR_UP
je moving_p_1
// cmp dl, DIR_DOWN
// je .moving_n

_moving_n_1 = $
moving_n_1:
sub ax, bx
jmp exit_3

_moving_p_1 = $
moving_p_1:
add ax, bx
// jmp .exit

_exit_3 = $
exit_3:
push ax



_detect_collision = $
detect_collision:
xchg bx, ax // we don't care about ax, but we need its value in bx
mov dx, es:[bx]

// emtpy space?
or dx, dx
lea ax, [bp + _main_loop]
jz exit_4


lea ax, [bp + _game_over]
cmp dx, FOOD
jne exit_4 // collided

// ate food
lea ax, [bp + _place_more_food]

mov dl, [bp+di]
test dl, LENGTH_MASK
jne extend_tail_1

mov dh, dl
xor dh, DIR_STOP_FLAG
xor dl, LENGTH_MASK
mov [bp+di], dx
inc di

_extend_tail_1 = $
extend_tail_1:
inc byte ptr [bp + di]
inc word ptr ss:[SCORE_ADDR] // update score
_exit_4 = $
exit_4:
push ax // ax contains return address

_draw = $
draw:
pusha

// Here I don't care much for di, but I must draw thus I will use di as
// pointer to video memory and restore it later on.
// Same with si, it will follow snake segments and be restored later on.

_draw_part_1 = $
draw_part_1:
xor ch, ch
mov cl, byte ptr [bp + si]
mov dl, cl
mov dh, cl

and dl, DIR_MASK    // get direction
and cl, LENGTH_MASK   // get distance
or cl, cl
jz next_move_1
mov ax, BYTES_PER_CHAR
cmp dl, DIR_RIGHT
mov di, SNAKE_BODY
je draw_p_1
cmp dl, DIR_LEFT
je draw_n_1

tmp = SCREEN_WIDTH * BYTES_PER_CHAR
mov ax, tmp
cmp dl, DIR_UP
je draw_n_1
// cmp dl, DIR_DOWN
// je .draw_p

_draw_p_1 = $
draw_p_1:
add bx, ax
mov word ptr es:[bx], di
loop draw_p_1
jmp next_move_1

_draw_n_1 = $
draw_n_1:
sub bx, ax
mov word ptr es:[bx], di
loop draw_n_1
// jmp .next_move

_next_move_1 = $
next_move_1:
inc si
test dh, DIR_STOP_FLAG
je draw_part_1

_found_last_part_1 = $
found_last_part_1:
// We need to clear last character of snake tail on screen,
// and we also need to set ah to 0, to be able to call int 0x1a.
// So let's save 2B by clearing ax and using it to clear snake tail on
// screen and then use it to call int 0x1a.
// Here CX is zero since it just was zeroed out by loop
mov word ptr es:[bx], cx

@if debug == 1
    call dump_stack
    call dump_snake_data

_print_score_1 = $
print_score_1:
mov ax, ss:[SCORE_ADDR]
mov di, 10
call print_number
mov ax, ss:[HIGH_SCORE_ADDR]
tmp = (SCREEN_WIDTH - 5) * BYTES_PER_CHAR
mov di, tmp
call print_number
popa
mov word ptr es:[bx], SNAKE_HEAD

_end_of_mail_loop = $
end_of_mail_loop:
// IP was pushed to stack during execution of detect_collision
ret


_prefix_head = $
prefix_head:
inc di
// dl - value to prefix
// di - pointer to tail of snake
// si - pointer to head of snake
pusha
push es
push ds
pop es

mov cx, di
sub cx, si

add di, bp
mov si, di
dec si
std
rep movsb
mov byte ptr [di], dl

pop es
popa
ret

_place_food = $
place_food:
pusha
_rand_1 = $
rand_1:
// https://en.wikipedia.org/wiki/Xorshift
mov eax, ss:[RAND_STATE_ADDR]

// x ^= x << 13;
mov ebx, eax
shl ebx, 13
xor eax, ebx

// x ^= x >> 17;
mov ebx, eax
shr ebx, 17
xor eax, ebx

// x ^= x << 5;
mov ebx, eax
shl ebx, 5
xor eax, ebx

mov ss:[RAND_STATE_ADDR],eax

_get_pos_1 = $
get_pos_1:
// magic number calculated like:
// 255*255*255*255 / (80-2) / (25-2) and then rounded to lowest integer
mov ebx, 2356884
xor edx, edx
div ebx
tmp = SCREEN_WIDTH + 1
add ax, tmp
shl ax, 1 // ax = ax * 2
xchg ax, bx // I don't care about ax and this is shorter than mov bx, ax

cmp word ptr es:[bx], VOID
jne rand_1
mov word ptr es:[bx], FOOD

popa
ret

_print_number = $
print_number:
// ax - number to print
// di - pointer to last character of number on screen
_next_digit_1 = $
next_digit_1:
mov bx, 10
xor dx, dx
div bx
add dx, SCORE_ZERO_CHAR
mov es:[di], dx
dec di
dec di
or ax, ax
jnz next_digit_1
ret

@if mbr == 1
    // I want my data to be stored just before boot signature, in case
    // of MBR. I don't care that if mbr=0 that I'd still have MBR.
    // This is to be able to understand how much bytes I still have
    // left before hitting generous 512 byte limitation
    @repeat (512 - $ - BOOT_SIGNATURE_SIZE)
        @saveb[1]

_my_data = $
my_data:
_boot_signature = $
boot_signature:
@word 0xAA55
_eof = $
eof:


@if debug == 1
    _hex_table = $
    hex_table: 
    @string "0123456789ABCDEF"
    _dump_snake_data = $
    dump_snake_data:
    pusha
    cld

    tmp = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH * BYTES_PER_CHAR
    mov di, tmp
    push di
    mov ax, BORDER
    mov cx, SCREEN_WIDTH
    rep stosw
    pop di
    lea si, [bp+_snake_data]
    lea bx, [bp+_hex_table]

    _next_byte_1 = $
    next_byte_1:
    mov dl, [ds:si]
    inc si
    mov dh, dl
    push dx
    and dx, 0x0ff0
    push dx
    shr dl, 4
    mov al, dl
    xlatb
    stosw
    pop dx
    shr dx, 8
    mov al, dl
    xlatb
    stosw

    pop dx
    and dl, DIR_STOP_MASK
    cmp dl, DIR_STOP_FLAG
    jne next_byte_1

    popa
    ret

    _dump_stack = $
    dump_stack:
    pusha
    cld

    xor di, di
    mov ax, BORDER
    mov cx, SCREEN_WIDTH
    rep stosw
    lea bx, [bp+_hex_table]

    tmp = 8 * BYTES_PER_CHAR
    mov di, tmp
    tmp = 0xffff - 8 * 2
    mov cx, tmp
    sub cx, sp

    mov si, sp
    tmp = 8 * 2
    add si, tmp // Don't dump pusha of this method


    _next_byte_2 = $
    next_byte_2:
    mov dl, [ss:si]
    inc si
    mov dh, dl
    and dx, 0x0ff0
    push dx
    shr dl, 4
    mov al, dl
    xlatb
    stosw
    pop dx
    shr dx, 8
    mov al, dl
    xlatb
    stosw
    loop next_byte_2

    popa
    ret

_rand_state = $
_snake_data = _rand_state + 4
rand_state:
snake_data: