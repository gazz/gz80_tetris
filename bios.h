// sdcc / asm interop reference: https://gist.github.com/Konamiman/af5645b9998c802753023cf1be8a2970

#define true 1
#define false 0

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

/*
DigitU16_ToASCII	equ $06BD
DigitU8_ToASCII		equ $0694
DigitU8_ToASCII_3digit	equ $0686
*/

void DigitU8_ToASCII(char *str_buf, char number) {
	(void)str_buf;(void)number;
	__asm
	ld iy, #2
	add	iy, sp

	ld l, (iy)
	ld h, 1(iy)
	ld a, 2(iy)
	call #0x694
	__endasm;
}

void digit_to_char_pad(char *str_buf, unsigned int digit, char pad) {
	char temp_out[7];
	char *temp_out_ptr = temp_out;
	int tenthou = digit / 10000;
	int thou_hundreds = (digit % 10000) / 100;
	int tens_singles = digit % 100;
	if (tenthou > 0) {
		DigitU8_ToASCII(temp_out_ptr, tenthou);
		temp_out_ptr += 2;
	}
	if (thou_hundreds > 0) {
		DigitU8_ToASCII(temp_out_ptr, thou_hundreds);
		temp_out_ptr += 2;
	}
	DigitU8_ToASCII(temp_out_ptr, tens_singles);
	temp_out_ptr = temp_out;
	int bytes_left = 7;
	if (!pad) {
		while (*temp_out_ptr == '0' && *(temp_out_ptr + 1) != '\0') {
			bytes_left--;
			temp_out_ptr++;
		}		
	}
	memcpy(str_buf, temp_out_ptr, bytes_left);
}

void digit_to_char(char *str_buf, unsigned int digit) {
	digit_to_char_pad(str_buf, digit, 0);
}


char get_ps2_scancode() __naked {
	__asm
	scancode = 0x2042
	SERB = 0x9
	SERBC = 0xb

	in a, (SERBC)
	bit 0, a
	jr z, no_new_char

	in a, (SERB)
	ld l, a
	jr done_ps2_scan
no_new_char:
	ld l, #0
done_ps2_scan:
	ret
	__endasm;
}

