#include <stdio.h>
unsigned char a = 12, b = 34;
char hello[];

void clear_screen() {
	__asm
	call 0xf5
	__endasm;
}

int putchar(int ch) {
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

	printf("\n");
	printf("math:%d + %d = %d\n", 2, 5, (2+5), hello);
	
	while(1) {
		a++;
	}
}

char hello[] = "yeehaa!";

