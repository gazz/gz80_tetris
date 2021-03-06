#include <string.h>
#include <stdio.h>
#include "bios.h"
#include "vga.h"
/*
#include "sprites.h"

void update_sprites() {
  char current_sprite[120];
  for (int i = 0; i < 11; i++) {
    memcpy(current_sprite, sprites + i * 120, 120);
    update_sprite(i, current_sprite);
    set_cell_sprite(i, 0, i);
  }
}
*/


int main(void) {

	clear_screen();

	initpio2();

	//update_sprites();

	clear_vga_screen();

	// set_cell_sprite(1, 1, 3);
	char out_text[30];
	// sprintf(out_text, "uptime: %d:%d.%d", get_minutes(), get_seconds(), get_milis());

	sprintf(out_text, "long[%d], int[%d], char[%d]", sizeof(long), sizeof(int), sizeof(char));
	out_vga_text(0, 0, out_text);


	while (1) {
		unsigned int raw_millis = get_raw_millis();
		unsigned int millis = get_millis();
		char minutes = get_minutes();
		char seconds = get_seconds();
		sprintf(out_text, "C: %02d:%02d.%04d", minutes, seconds, millis);
		out_vga_text(0, 1, out_text);

		sprintf(out_text, "[%05d]", raw_millis);
		out_vga_text(0, 2, out_text);

		sleep(100);		
	}

	return 0;
}

