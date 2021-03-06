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
#ifdef autoadvance
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

char blocks[7][16] = {
  {0, 1, 0, 0,
   0, 1, 0, 0,
   0, 1, 1, 0,
   0, 0, 0, 0},
  {0, 0, 2, 0,
   0, 0, 2, 0,
   0, 2, 2, 0,
   0, 0, 0, 0},
  {0, 0, 0, 0,
   0, 3, 0, 0,
   3, 3, 3, 0,
   0, 0, 0, 0},
  {0, 0, 4, 0,
   0, 4, 4, 0,
   0, 4, 0, 0,
   0, 0, 0, 0},
  {0, 5, 0, 0,
   0, 5, 5, 0,
   0, 0, 5, 0,
   0, 0, 0, 0},
  {0, 1, 0, 0,
   0, 1, 0, 0,
   0, 1, 0, 0,
   0, 1, 0, 0},
  {0, 0, 0, 0,
   0, 4, 4, 0,
   0, 4, 4, 0,
   0, 0, 0, 0}
};


char gameArea[19 * 10];
char playerBlock[16];
char nextPlayerBlock[16];
int score = 0;
int playerPosX = 3;
int playerPosY = 10;
float currentSpeed = 0.1;
char sweeps[4];

enum {
  RESET = 0,
  PLAYING,
  CHECK_SWEEPS,
  SWEEP_LINES,
  GAME_OVER
} typedef GAME_STATE;

GAME_STATE gameState = 0;


char scoreText[7];
void updateScore() {
  // sprintf(scoreText, "%d", score);
  // Serial.print("Score: ");Serial.print(score, DEC);Serial.print(", as text: ");Serial.print(scoreText);
  // char debugs[10];
  // for (int i = 0; i < 7; i++) {
  //   char ch = scoreText[i];
  //   char spriteIndex = (ch == 32) ? 0 : (ch - 30) > 0 ? (ch - 30) : 0;
  //   setCellSprite(22 + i, 1, spriteIndex);
  // }
  // Serial.println();
}

void updatePlayableArea() {
	// playable area 10x19
	for (int i = 0; i < 19 * 10; i ++) {
    	int x = i % 10;
    	int y = i / 10;
    	int spriteIndex = gameArea[i];

    	if (x >= playerPosX && x < playerPosX + 4 && y >= playerPosY && y < playerPosY + 4) {
      		int playerSprite = playerBlock[((y - playerPosY) << 2) + x - playerPosX];
      		spriteIndex = (playerSprite != 0) ? playerSprite : spriteIndex;
	      	set_cell_sprite(x + 10, y, spriteIndex);
      	} else {
	      	set_cell_sprite(x + 10, y, spriteIndex);
      	}
    }
}

void updateNextBlock() {
  int xOffset = 23, yOffset = 4;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      set_cell_sprite(xOffset + j, yOffset + i, nextPlayerBlock[i * 4 + j]);
    }
  }
}

void updateGameArea() {
  // draw bounds
  if (gameState == GAME_OVER) {
    int xOffset = 12;
    // for (int i = 0; i < 6; i++) {
    //   setCellSprite(xOffset + i, 6, 11+ i);
    // }
    // for (int i = 0; i < 6; i++) {
    //   setCellSprite(xOffset + i, 7, 28+ i);
    // }
  } else {
    for (int i = 0; i< 19; i++) {
      // set_cell_sprite(9, i, 7);
      // set_cell_sprite(20, i, 8);
    }
    // for (int i = 10; i< 20; i++) {
    //   set_cell_sprite(i, 19, 9);
    // }
    // set_cell_sprite(9, 19, 10);
    //set_cell_sprite(20, 19, 11);
  
    updatePlayableArea();
    updateNextBlock();
    updateScore();
  }
}

short try_move(const char area[10 * 19], char item_mask[16], char position[2], char move[2]) {
  int width = 10;
  int height = 19;
  int x = position[0];
  int y = position[1];
  int hor_move = move[0];
  int ver_move = move[1];
  short success = true;

  for (int i = 0; i < height + 1; i++) {
    for (int j = -1; j < width + 1; j++) {
      unsigned field_index = i * width + j;
      char field_val = (j < 0 || j == width || i == height) ? 1 : (area[field_index] ? 1 : 0);

      int mask_index = (j < x || j > x + 3 || i < y || i > y + 3) ? -1 : (j - x + ((i - y) << 2));
      int new_x = x + hor_move;
      int new_y = y + ver_move;
      int moved_mask_index = (j < new_x || j > new_x + 3 || i < new_y || i > new_y + 3) ? -1 : (j - new_x + ((i - new_y) << 2));

      field_val = (mask_index == -1) ? field_val : field_val + (item_mask[mask_index] ? 1 : 0);
      field_val = (moved_mask_index == -1 || item_mask[mask_index]) ? field_val : field_val + (item_mask[moved_mask_index] ? 1 : 0);

      if (field_val > 1) success = false;
    }
  }

  return success;
}


bool copyBlockToArea() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      char spriteValue = playerBlock[i * 4 + j];
      short worldX = j + playerPosX;
      short worldY = i + playerPosY;
      if (spriteValue) {
        if (worldY < 0) {
          // game over
          return false;
        } else if (worldX >= 0) {
          gameArea[worldX + worldY * 10] = spriteValue;        
        }
      }
    }    
  }
  return true;
}


void rotateBlock(const char inputBlock[16], char out[16]) {
  // all rotations are clockwise
  out[0] = inputBlock[12];  
  out[1] = inputBlock[8]; 
  out[2] = inputBlock[4]; 
  out[3] = inputBlock[0]; 

  out[4] = inputBlock[13];  
  out[5] = inputBlock[9]; 
  out[6] = inputBlock[5]; 
  out[7] = inputBlock[1]; 

  out[8] = inputBlock[14];  
  out[9] = inputBlock[10];  
  out[10] = inputBlock[6];  
  out[11] = inputBlock[2];  

  out[12] = inputBlock[15]; 
  out[13] = inputBlock[11]; 
  out[14] = inputBlock[7];  
  out[15] = inputBlock[3];  
}


bool checkForSweeps() {
  memset(sweeps, 0, 4);
  int sweepCounter = 0;
  for (int i = 0; i < 19; i++) {
    bool sweep = true;
//    Serial.println();
    for (int j = 0; j < 10; j++) {
      int index = i * 10 + j;
//      Serial.print(gameArea[index], DEC);Serial.print("\t");
      if (gameArea[index] == 0) { 
//        Serial.print("No sweep at: ");Serial.print(index); Serial.print(", value: "); Serial.print(gameArea[index], DEC); 
//        Serial.print(", sweep: "); Serial.print(sweep, DEC); Serial.print(", sweepCounter: "); Serial.print(sweepCounter, DEC);
        sweep = false;
        break;
      }
    }
    if (sweep) {
      sweeps[sweepCounter++] = i;
//      Serial.println();
//      Serial.print("sweep: "); Serial.print(sweep, DEC); Serial.print(", sweepCounter: "); Serial.print(sweepCounter, DEC);
    }
  }
//  Serial.print("Sweeps: ");Serial.println(sweepCounter);
  return sweepCounter != 0;
}


void removeLine(int rowIndex) {
  // move memory by 10 positions
  char tmpArea[10 * 19];
  memcpy(tmpArea + 10, gameArea, rowIndex * 10);

  // fill up top of the line
  memset(tmpArea, 0, 10);

  memcpy(gameArea, tmpArea, 10 * 19);
}


int doSweep(int counter) {
//  Serial.print("Doing sweep...");Serial.println(counter);
  int linesSweeped = 0;
  if (counter == 1) {
    // remove sweeped lines
//    char buf[120];
//    sprintf(buf, "removing sweeped lines: %d, %d, %d, %d", sweeps[0], sweeps[1], sweeps[2], sweeps[3]);
//    Serial.println(buf);
    for (int i = 0; i < 4; i++) {
      if (sweeps[i] > 0) {
        linesSweeped << 1;
        removeLine(sweeps[i]);
      }
    }
  } else {
    // TODO: animations
  }
  return linesSweeped;
}

void game_startup() {
  clear_vga_screen();

}

void reset_game() {
  clear_vga_screen();
  gameState = RESET;
  // updateGameArea();
}


void generateNextPlayerBlock() {
  memcpy(playerBlock, nextPlayerBlock, 16);
  memcpy(nextPlayerBlock, blocks[rand()%7], 16);
}


const int BASE_GAME_SPEED = 1;
int gameStateCounter = 0;

void gameAdvance(int milis, char scancode) {
  char pos[] = {playerPosX, playerPosY};
  short move[] = {0, 0};
  char buf[120];
  gameStateCounter--;

  switch(gameState) {
    case RESET: {
      memset(gameArea, 0, 19 * 10);
      generateNextPlayerBlock();
      generateNextPlayerBlock();
      playerPosX = 3;
      playerPosY = -4;      
      updateGameArea();
      gameState = PLAYING;
      gameStateCounter = 10;
      currentSpeed = 7;
      score = 0;
      break;
    };
    case PLAYING: {    
      if (gameStateCounter <= 1) {
         move[1] = 1;
         gameStateCounter = BASE_GAME_SPEED * currentSpeed;
      }
      else {
      	char tmp_text[10];
		digit_to_char_pad(tmp_text, scancode, true);
		out_vga_text(0, 0, tmp_text);
		if (scancode == 41) {
			char testRotate[16];
			rotateBlock(playerBlock, testRotate);
			if (try_move(gameArea, testRotate, pos, move)) {
				memcpy(playerBlock, testRotate, 16);
			}
		} 
		else {
			// move block
	  		switch(scancode) {
	  			// left
				case 107: move[0] = -1; break;
				// right
				case 116: move[0] = 1; break;
				// up 
				case 117: break;
				// down
				case 114: break;
				// space
				case 41: break;
			}
		}
	}
      
      bool legal_move = try_move(gameArea, playerBlock, pos, move);
      if (move[1] > 0) {
//        sprintf(buf, "Advancing game: [%d, %d], move legal?: %d", playerPosX, playerPosY, legal_move);
//        Serial.println(buf);
        if (legal_move) {
          playerPosY += 1;
        } else {
          // copy the block into main area and generate a new one
          if (!copyBlockToArea()) {
            gameState = GAME_OVER;
            gameStateCounter = 50;
          } else {
            gameState = CHECK_SWEEPS;
            generateNextPlayerBlock();
            playerPosX = 3;
            playerPosY = -4;      
          }        
        }
    //    dumpArea();    
      } else {
        // hor move
        if (legal_move) playerPosX += move[0];
      }
      break;
    };
    case CHECK_SWEEPS: {
      // see if there are any full rows worth removing
      gameStateCounter = checkForSweeps() ? 10 : 1;
      gameState = SWEEP_LINES;
      break;
    }
    case SWEEP_LINES: {
      if (gameStateCounter <= 0) gameState = PLAYING;
      else {
        score += 1 << doSweep(gameStateCounter);
        currentSpeed = 1 - score / 1000;
      }
      break;
    }    
    case GAME_OVER: {
      if (scancode == 41) {
        gameState = RESET;
        while (true);
      }
      break;
    };
  }

}



int main(void) {

	clear_screen();

	initpio2();

	clear_vga_screen();

	update_sprites();

	clear_vga_screen();

	reset_game();

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

/*
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
*/		
		digit_to_char_pad(out_text, scancode, true);
		out_vga_text(0, 0, "        ");

		gameAdvance(elapsed_millis, scancode);
		updateGameArea();
		// bounce(elapsed_millis, scancode);
	}

	return 0;
}

