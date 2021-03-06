#include "vga.h"

void initpio2() {
	__asm
	PIO2AC = 0x22
	PIO2BC = 0x23
	; set ouput mode on PIO 
	ld a, #0xf	; b00001111 is control byte for output
	; b1 addresses pio in command mode
	out (#PIO2AC), a
	out (#PIO2BC), a
	__endasm;	
}

void clear_vga_screen() {
		// clear
	for (int i = 0; i < 20; i++) {
		for (int j = 0; j< 30; j++) {
			set_cell_sprite(j, i, 0);
		}
	}
}

void set_cell_sprite(char x, char y, char sprite) {
	// get rid of unreferenced argument
	(void)sprite;(void)x;(void)y;
	__asm
	; #include "../gz80_bios/bios_include.asm"
	jr set_cell_sprite

	VGA_DATA_ENABLE = 0b00000001
	VGA_CONTROL_ENABLE = 0b00000010
	VGA_WRITE_ENABLE = 0b00000100
	VGA_CHIP_ENABLE = 0b00001000
	PIO2A = 0x20
	PIO2B = 0x21

PIO2Out:
	; output to pio & wait until busy flag clears
	ld a, d
	out (#PIO2A), a
	ld a, e
	out (#PIO2B), a
	ret

set_cell_sprite:
	; a has sprite (0..128)
	; hl is onscreen sprite index (y * 30 + x)
	ld	iy, #2	; bypass return address
	add	iy, sp
	ld h, #0
	ld l, (iy)

	; calculate y offset by multiplying it by 30
	; y << 5
	ld h, #0
	ld l, 1(iy)
	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl
	add hl, hl

	; subtract y << 1
	ld a, 1(iy)
	rla
	ld b, #0
	ld c, a
	sbc hl, bc
	ld b, #0
	ld c, (iy)
	add hl, bc

	ld a, 2(iy)

	push af

	; instruction
	; 1) data enable goes low
	; 2) set data output bits to instruction 0x7
	; 3) control enable pin goes low
	; 4) write pin goes low
	; 5) chip enable pin goes low
	ld d, #0x07
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; 6) chip enable pin goes high
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE)
	call PIO2Out
	;call instr_wait

	; arg 1 : a >> 6 
	; 7) set data output bits to arg1
	; 8) chip enable pin goes low
	pop af
	push af
	rra
	rra
	rra
	rra
	rra
	rra 
	ld d, a
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; 9) chip enable pin goes high
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE)
	call PIO2Out
	;call instr_wait

	; arg 2 : (a << 2) + h
	; 10) set data output bits to arg2
	; 11) chip enable pin goes low
	pop af
	push af
	rla
	rla
	add a, h
	ld d, a
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; 12) chip enable pin goes high
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE)
	call PIO2Out
	;call instr_wait

	; arg 3 : l
	; 13) set data output bits to arg2
	; 14) chip enable pin goes low
	ld d, l
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; wrap up
	; 15) chip enable pin goes high
	; 16) write pin goes high
	; 17) control enable pin goes high
	; 18) data enable goes low
	ld e, #~0
	call PIO2Out
	;call instr_wait

	pop af
	__endasm;
}

int asciiToFontIndex(char ascii_char) {
  if (ascii_char <= 96 || (ascii_char >= 123 && ascii_char <= 126))
    return ascii_char - 32;
  
  if (ascii_char >= 97 && ascii_char <= 122)
    return ascii_char - 64;

//  return ascii_char < 96 ? ascii_char ;
  return 0;
}

void out_vga_text(int x, int y, char *text) {
	int next_char_index = x;
	char next_char = *text;
	while (next_char != '\0') {
		char char_to_sprite = asciiToFontIndex(next_char);
		set_cell_sprite(next_char_index++, y, char_to_sprite);
		next_char = *++text;
	}
}


void push_instruction_3(char instruction, char arg0, char arg1, char arg2) {
	(void)instruction;(void)arg0;(void)arg1;(void)arg2;
	__asm
	ld	iy, #2	; bypass return address
	add	iy, sp

	ld b, (iy)
	ld c, 1(iy)
	ld h, 2(iy)
	ld l, 3(iy)

	; instruction
	; 1) data enable goes low
	; 2) set data output bits to instruction 0x7
	; 3) control enable pin goes low
	; 4) write pin goes low
	; 5) chip enable pin goes low
	ld d, b
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; 6) chip enable pin goes high
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE)
	call PIO2Out
	;call instr_wait

	; arg 1
	; 7) set data output bits to arg1
	; 8) chip enable pin goes low
	ld d, c
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; 9) chip enable pin goes high
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE)
	call PIO2Out
	;call instr_wait

	; arg 2
	; 10) set data output bits to arg2
	; 11) chip enable pin goes low
	ld d, h
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; 12) chip enable pin goes high
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE)
	call PIO2Out
	;call instr_wait

	; arg 3
	; 13) set data output bits to arg3
	; 14) chip enable pin goes low
	ld d, l
	ld e, #~(VGA_CONTROL_ENABLE | VGA_DATA_ENABLE | VGA_WRITE_ENABLE | VGA_CHIP_ENABLE)
	call PIO2Out
	;call instr_wait

	; wrap up
	; 15) chip enable pin goes high
	; 16) write pin goes high
	; 17) control enable pin goes high
	; 18) data enable goes low
	ld e, #~0
	call PIO2Out
	;call instr_wait

	__endasm;
}

void update_sprite(unsigned char sprite_index, const unsigned char input[120]) {
  for (int i = 0; i < 120; i+=2) {
    unsigned char sprite_pixel = input[i];
    unsigned char sprite_pixel2 = input[i+1];
    unsigned char combined_pixels = ((sprite_pixel << 4) & 0xff) + (sprite_pixel2 & 0xf);
    unsigned char offset = i;
    unsigned char offsetX = offset % 10;
    unsigned char offsetY = offset / 10;

    unsigned char arg0 = combined_pixels; 
    unsigned char arg1 = ((offsetY << 4)& 0xff) + (offsetX & 0xf);
    unsigned char arg2 = (sprite_index & 0xff);
    
    push_instruction_3(0x8, arg0, arg1, arg2);
  }
}


