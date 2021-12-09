#ifndef _RGB_PIXEL_H_
#define _RGB_PIXEL_H_
/*==================================================================
  File Name: pixel.h
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This is the header-file for pixel.c
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
#include <stdint.h>
#include "stm8_hw_init.h"

// List of directions
#define HOR    (false) /* Horizontal orientation for printChar() */
#define VERT   (true)  /* Vertical   orientation for printChar() */
#define LK1    (0x01)  /* 1 = lichtkrant 1 text is changed */
#define LK2    (0x02)  /* 1 = lichtkrant 2 text is changed */

#define SCREEN (0) /* Main screen for RGB-matrices */
#define FIELD  (1) /* Tetris playfield */
      
// List of colours used
#define EMPTY   (0x00)
#define BLACK   (EMPTY)
#define BLUE    (0x01)
#define GREEN   (0x02)
#define RED     (0x04)
#define YELLOW  (GREEN| RED)
#define MAGENTA (BLUE | RED)
#define CYAN    (BLUE | GREEN)
#define WHITE   (BLUE | GREEN | RED)

void    clearScreen(bool screen);
void    setPixel(bool screen, int8_t x, int8_t y, uint8_t col);
uint8_t getPixel(bool screen, int8_t x, int8_t y);
void    printChar(bool screen, int8_t x, int8_t y, uint8_t ch, uint8_t col, bool hv);
void    printSmallChar(bool screen, int8_t x, int8_t y, uint8_t ch, uint8_t col, bool hv);
void    drawLine(bool screen, int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col);

#endif