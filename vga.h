void initpio2();
void set_cell_sprite(char x, char y, char sprite);
void clear_vga_screen();
void out_vga_text(int x, int y, char *text);
void update_sprite(unsigned char sprite_index, const unsigned char input[120]);
void push_instruction_3(char instruction, char arg0, char arg1, char arg2);