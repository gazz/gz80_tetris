;--------------------------------------------------------
; File Created by SDCC : free open source ANSI-C Compiler
; Version 4.0.0 #11528 (Mac OS X x86_64)
;--------------------------------------------------------
	.module main
	.optsdcc -mz80
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _clear_screen
	.globl _printf
	.globl _hello
	.globl _b
	.globl _a
	.globl _putchar
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
_a::
	.ds 1
_b::
	.ds 1
_hello::
	.ds 8
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;main.c:5: void clear_screen() {
;	---------------------------------
; Function clear_screen
; ---------------------------------
_clear_screen::
;main.c:8: __endasm;
	call	0xf5
;main.c:9: }
	ret
;main.c:11: int putchar(int ch) {
;	---------------------------------
; Function putchar
; ---------------------------------
_putchar::
;main.c:17: __endasm;
	ld	iy, #2 ; bypass return address
	add	iy, sp
	ld	e, (iy)
	call	0x8e
;main.c:18: return 0;
	ld	hl, #0x0000
;main.c:19: }
	ret
;main.c:21: int main(void) {
;	---------------------------------
; Function main
; ---------------------------------
_main::
;main.c:23: clear_screen();
	call	_clear_screen
;main.c:27: printf("%d + %d = %d, %s\n", 2, 5, (2+5), hello);
	ld	hl, #_hello
	push	hl
	ld	hl, #0x0007
	push	hl
	ld	l, #0x05
	push	hl
	ld	l, #0x02
	push	hl
	ld	hl, #___str_0
	push	hl
	call	_printf
	ld	hl, #10
	add	hl, sp
	ld	sp, hl
;main.c:28: printf("%d + %d = %d, %s", 2, 5, (2+5), hello);
	ld	hl, #_hello
	push	hl
	ld	hl, #0x0007
	push	hl
	ld	l, #0x05
	push	hl
	ld	l, #0x02
	push	hl
	ld	hl, #___str_1
	push	hl
	call	_printf
	ld	hl, #10
	add	hl, sp
	ld	sp, hl
;main.c:34: while(1) {
00102$:
;main.c:35: a++;
	ld	hl, #_a+0
	inc	(hl)
;main.c:37: }
	jr	00102$
___str_0:
	.ascii "%d + %d = %d, %s"
	.db 0x0a
	.db 0x00
___str_1:
	.ascii "%d + %d = %d, %s"
	.db 0x00
	.area _CODE
	.area _INITIALIZER
__xinit__a:
	.db #0x0c	; 12
__xinit__b:
	.db #0x22	; 34
__xinit__hello:
	.ascii "yeehaa!"
	.db 0x00
	.area _CABS (ABS)
