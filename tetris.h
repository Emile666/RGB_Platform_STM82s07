#ifndef TETRIS_H_
#define TETRIS_H_
/*==================================================================
  File Name: tetris.h
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This is the header-file for tetris.c
  ------------------------------------------------------------------
  This file is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this software.  If not, see <http://www.gnu.org/licenses/>.
  ================================================================== */ 
#include "random.h"
#include "stm8_hw_init.h"
#include "pixel.h"

//---------------------------------------------------------------------------
// MAX_LEVEL    : Max. level of a game. Warning! the higher the maximum level,
//                the slower shapes will fall at level 1
// LEVEL_GAIN   : Points needed to increase the level
// SIZE_X       : Size of playfield in x-direction
// SIZE_Y       : Size of playfield in y-direction
// MAX_CHAR_Y   : Nr. of possible characters horizontally
// TETRIS_WALL_X: The x-position of the Tetris wall
//---------------------------------------------------------------------------
#define MAX_LEVEL     (18)
#define LEVEL_GAIN    (1000)
#define TETRIS_WALL_X (SIZE_X-4)

#define NEW_SHAPE   (0) /* Bit0 - generate new shape */
#define FAST_DROP   (1)	/* Bit1 - fast shape drop */
#define PLACE_SHAPE (2) /* Bit2 - place shape */
#define ROW_FOUND   (4)	/* Bit4 - row to erase is found and set */
#define CLEAR_SHIFT (6) /* Bit6 - clear shift of the text for screen */
#define NEW_GAME    (7) /* Bit7 - prepare to start the new game */

// List of game Screens
#define SCREEN_MENU      (0)
#define SCREEN_GAME      (1)
#define SCREEN_PAUSED    (2)
#define SCREEN_GAME_OVER (3)

// List of possible orientations per block
#define NORTH (0)
#define EAST  (1)
#define SOUTH (2)
#define WEST  (3)

//----------------------------------------------------------------------------
// List of different blocks used in the game, and their orientation.
// The orientation EAST is default when placed in the game.
//
//  TYPE_O    TYPE_I   TYPE_I    TYPE_S   TYPE_S    TYPE_Z   TYPE_Z
//  1 ....    1 ....   1 ..X.    1 ....   1 .X..    1 ....   1 ...X
//  0 .XO.    0 XXOX   0 ..O.    0 ..OX   0 .XO.    0 .XO.   0 ..OX
// -1 .XX.   -1 ....  -1 ..X.   -1 .XX.  -1 ..X.   -1 ..XX  -1 ..X.
// -2 ....   -2 ....  -2 ..X.   -2 ....  -2 ....   -2 ....  -2 ....
//   -2101+    -2101+  -2101+     -2101+   -2101+    -2101+   -2101+
//   NORTH     EAST    NORTH      EAST     NORTH     EAST     NORTH
//   EAST      WEST    SOUTH      WEST     SOUTH     WEST     SOUTH
//   SOUTH
//   WEST
//
//  TYPE_L    TYPE_L   TYPE_L   TYPE_L    TYPE_J    TYPE_J   TYPE_J   TYPE_J
//  1 ....    1 ..X.   1 ...X   1 .XX.    1 ....    1 ..XX   1 .X..   1 ..X.
//  0 .XOX    0 ..O.   0 .XOX   0 ..O.    0 .XOX    0 ..O.   0 .XOX   0 ..O.
// -1 .X..   -1 ..XX  -1 ....  -1 ..X.   -1 ...X   -1 ..X.  -1 ....  -1 .XX.
// -2 ....   -2 ....  -2 ....  -2 ....   -2 ....   -2 ....  -2 ....  -2 ....
//   -2101+    -2101+  -2101+    -2101+    -2101+    -2101+   -2101+   -2101+
//   EAST      NORTH    WEST     SOUTH     EAST     NORTH     WEST     SOUTH
//
//  TYPE_T    TYPE_T   TYPE_T   TYPE_T
//  1 ....    1 ..X.   1 ..X.   1 ..X.
//  0 .XOX    0 ..OX   0 .XOX   0 .XO.
// -1 ..X.   -1 ..X.  -1 ....  -1 ..X.
// -2 ....   -2 ....  -2 ....  -2 ....
//   -2101+    -2101+  -2101+    -2101+
//   EAST      NORTH    WEST     SOUTH
//----------------------------------------------------------------------------
#define TYPE_I  (0)
#define TYPE_J  (1)
#define TYPE_L  (2)
#define TYPE_O  (3)
#define TYPE_S  (4)
#define TYPE_T  (5)
#define TYPE_Z  (6)

// List of colour for every block used in the game
#define COLOUR_TYPE_I  (RED)
#define COLOUR_TYPE_J  (YELLOW)
#define COLOUR_TYPE_L  (MAGENTA)
#define COLOUR_TYPE_O  (BLUE)
#define COLOUR_TYPE_S  (CYAN)
#define COLOUR_TYPE_T  (GREEN)
#define COLOUR_TYPE_Z  (RED)

void      tetrisInputs(void);
void      CheckX(int8_t *x, int8_t shape, int8_t rotation);
void      downOneRow(int8_t src_x , int8_t src_y, uint8_t width, uint8_t height, int8_t dest_x, int8_t dest_y);
bool      canMoveRight(int8_t x, int8_t y, uint8_t shape, uint8_t rotation);
bool      canMoveLeft(int8_t x, int8_t y, uint8_t shape, uint8_t rotation);
bool      shouldPlace(int8_t x, int8_t y, uint8_t shape, uint8_t rotation);
void      drawShape(int8_t x, int8_t y, uint8_t shape, uint8_t rotation);
void      tetrisMain(void);

#endif /* TETRIS_H_ */
