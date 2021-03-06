// sdcc / asm interop reference: https://gist.github.com/Konamiman/af5645b9998c802753023cf1be8a2970

inline void clear_screen() {
	__asm
	call 0xf5
	__endasm;
}

inline int test_args(char sprite, char x, char y) {
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

inline int putchar(int ch) {
	(void)ch;
	__asm
	ld	iy, #2	; bypass return address
	add	iy, sp
	ld e, (iy)
	call 0x8e
	__endasm;
	return 0;
}

/*
T_MILIS            	equ $204C
T_MINUTES         	equ $2050
T_RAW_MILIS        	equ $204A
T_SECONDS          	equ $204E
*/

int get_raw_millis() __naked {
	__asm
	ld iy, #0x204A
	ld h, 1(iy)
	ld l, (iy)
	ret
	__endasm;
}

int get_millis() __naked {
	__asm
	ld iy, #0x204C
	ld h, 1(iy)
	ld l, (iy)
	ret
	__endasm;
}

char get_minutes() __naked {
	__asm
	ld iy, #0x2050
	ld l, (iy)
	ret
	__endasm;
}

char get_seconds() __naked {
	__asm
	ld iy, #0x204E
	ld l, (iy)
	ret
	__endasm;
}

inline void sleep(int millis) {
	int start_millis = get_raw_millis();
	int target_millis = start_millis + millis; 
	while (target_millis > get_raw_millis()) {
		__asm
		nop
		__endasm;
	}
}
