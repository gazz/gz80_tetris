#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bios.h"
#include "vga.h"

// #define compile_sprites

#ifdef compile_sprites
#include "sprites.h"

void update_sprites_impl() {
	// colors
	char current_sprite[120];
	for (int i = 2; i < 8; i++) {
		memcpy(current_sprite, sprites, 120);
		for (int j = 0; j < 120; j++) {
			if (current_sprite[j] == 0x2) {
				current_sprite[j] = i;
			} 
			else if (current_sprite[j] == 0xa) {
				current_sprite[j] = i + 8;	
			}
		}
		update_sprite(i - 1, current_sprite);
	}
	update_sprite(8, &sprites[120]);
	update_sprite(9, &sprites[120 * 2]);
	update_sprite(10, &sprites[120 * 3]);
	update_sprite(11, &sprites[120 * 4]);
	update_sprite(12, &sprites[120 * 5]);
}
#endif

void update_sprites() {
#ifdef compile_sprites
	update_sprites_impl();
#endif
}

#define LEFT 0
#define RIGHT 29
#define TOP 0
#define BOTTOM 19
#define MILLIS_PER_STEP 200
short x = LEFT, y = TOP, dir_x = 1, dir_y = 1;
int millis_since_last_step = 0;
short sprite_color = 1; 
void bounce(int elapsed_millis, char scancode) {
	short new_col = rand() % 5 + 1;;
	short old_x = x, old_y = y;
	switch(scancode) {
		case 107: x -= 1; break;
		case 116: x += 1; break;
		case 117: y -= 1; break;
		case 114: y += 1; break;
		case 41: sprite_color = new_col; break;
	}

	millis_since_last_step += elapsed_millis;
#ifdef autoadcance
	// calculate 
	if (millis_since_last_step > MILLIS_PER_STEP) {
		// clear old
		if (x <= LEFT && dir_x < 0) {
			dir_x = 1;
			// sprite_color = new_col;
		}
		if (x >= RIGHT && dir_x > 0) {
			dir_x = -1;
			// sprite_color = new_col;
		}
		if (y <= TOP && dir_y < 0) {
			dir_y = 1;
			// sprite_color = new_col;
		}
		if (y >= BOTTOM && dir_y > 0) {
			dir_y = -1;
			// sprite_color = new_col;
		}
		x = x + dir_x * (millis_since_last_step / MILLIS_PER_STEP);
		y = y + dir_y * (millis_since_last_step / MILLIS_PER_STEP);
		millis_since_last_step = 0;

	}
#endif	

	if (x != old_x || y !=old_y) set_cell_sprite(old_x, old_y, 0);

	set_cell_sprite(x, y, sprite_color);
}

char out_text[30] = "Yoohooo!";

int main(void) {

	clear_screen();

	initpio2();

	clear_vga_screen();

	update_sprites();

	clear_vga_screen();

	int last_millis = get_raw_millis();
	// run loop
	while (1) {
		int new_millis = get_raw_millis();
		int elapsed_millis = new_millis - last_millis;
		last_millis = new_millis;


		int raw_millis = new_millis;
		int millis = get_millis();
		char minutes = get_minutes();
		char seconds = get_seconds();

		char scancode = get_ps2_scancode();

 		if (scancode != 107 && scancode != 116 && scancode != 117 && scancode != 114 && scancode != 41) {
			char *out_text_ptr = out_text;
			digit_to_char_pad(out_text_ptr, minutes, true);
			out_text[2] = ':';
			out_text_ptr += 3;
			digit_to_char_pad(out_text_ptr, seconds, true);
			out_text[5] = ':';
			out_text_ptr += 3;
			digit_to_char_pad(out_text_ptr, millis, true);
			out_vga_text(0, 0, out_text);

			out_vga_text(0, 1, "Scancode:      ");
			digit_to_char_pad(out_text, get_ps2_scancode(), true);
			out_vga_text(10, 1, out_text);
		}
		
		bounce(elapsed_millis, scancode);
		// sleep(50);
	}

	return 0;
}

