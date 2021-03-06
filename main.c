#include <stdio.h>
#include "vga.h"

void clear_screen() {
	__asm
	call 0xf5
	__endasm;
}

int test_args(char sprite, char x, char y) {
	(void)sprite;(void)x;(void)y;
	__asm
	ld e, #' '
	call 0x8e

	ld	iy, #2	; bypass return address
	add	iy, sp
	
	ld e, (iy)
	call 0x8e

	ld e, #' '
	call 0x8e

	ld e, 1(iy)
	call 0x8e

	ld e, #' '
	call 0x8e

	ld e, 2(iy)
	call 0x8e

	__endasm;
	return 0;
}

int putchar(int ch) {
	(void)ch;
	__asm
	ld	iy, #2	; bypass return address
	add	iy, sp
	ld e, (iy)
	call 0x8e
	__endasm;
	return 0;
}



int main(void) {

	clear_screen();

	initpio2();

	clear_vga_screen();

	set_cell_sprite(1, 1, 3);

	out_vga_text(3, 5, "YEEHAA!!!");


	// int i = 16000; 
	// while (1)
	// 	i++;

	return 0;
}

