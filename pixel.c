/*==================================================================
  File Name: pixel.c
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This file contains the RGB pixel routines
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
#include <stdlib.h>
#include "pixel.h"
#include "tetris.h"

extern playfield_color rgb_bufr; // Buffered version of the red leds
extern playfield_color rgb_bufg; // Buffered version of the green leds
extern playfield_color rgb_bufb; // Buffered version of the blue leds
extern uint16_t        fieldr[]; // Tetris playfield red leds
extern uint16_t        fieldg[]; // Tetris playfield green leds
extern uint16_t        fieldb[]; // Tetris playfield blue leds
extern uint8_t  atascii[128][8]; // Atari XL Font

/*-------------------------------------------------------------------------
 Purpose   : This function clears an entire screen.
  Variables: screen: [FIELD,SCREEN], Tetris playfield or main-screen
  Returns  : -
  -------------------------------------------------------------------------*/
void clearScreen(bool screen)
{
    if (screen == FIELD)
    {   // Tetris playfield
        for (uint8_t i = 0; i < TETRIS_MAX_Y; i++)
        {
            fieldr[i] = fieldg[i] = fieldb[i] = BLACK;
        } // for i
    } // if
    else
    {   // Main Screen
        for (uint8_t i = 0; i < MAX_Y; i++)
        {
            rgb_bufr[i] = rgb_bufg[i] = rgb_bufb[i] = BLACK;
        } // for i
    } // if
} // clearScreen()

/*-------------------------------------------------------------------------
 Purpose   : This function sets the color of a pixel in a screen.
  Variables: screen: [FIELD,SCREEN], Tetris playfield or main-screen
             x  : the x position of the pixel in the playfield
  	     y  : the y position of the pixel in the playfield
  	     col: the color code for the pixel
  Returns  : -
  -------------------------------------------------------------------------*/
void setPixel(bool screen, int8_t x, int8_t y, uint8_t col)
{
    if (screen == FIELD)
    {   // Tetris playfield
        if ((x >= 0) && (y >= 0) && (x < TETRIS_MAX_X) && (y < TETRIS_MAX_Y))
        {
            uint16_t bt = (1<<x);
            if ((col & RED)   ==  RED)   
                 fieldr[y]  |=  bt;
            else fieldr[y]  &= ~bt;
            if ((col & GREEN) ==  GREEN)   
                 fieldg[y]  |=  bt;
            else fieldg[y]  &= ~bt;
            if ((col & BLUE)  ==  BLUE)   
                 fieldb[y]  |=  bt;
            else fieldb[y]  &= ~bt;
        } // if
    } // if
    else
    {   // main Screen
        if ((x >= 0) && (y >= 0) && (x < SIZE_X) && (y < MAX_Y))
        {
            uint16_t bt = (1<<x);
            if ((col & RED)   ==  RED)   
                 rgb_bufr[y]  |=  bt;
            else rgb_bufr[y]  &= ~bt;
            if ((col & GREEN) ==  GREEN)   
                 rgb_bufg[y]  |=  bt;
            else rgb_bufg[y]  &= ~bt;
            if ((col & BLUE)  ==  BLUE)   
                 rgb_bufb[y]  |=  bt;
            else rgb_bufb[y]  &= ~bt;
        } // if        
    } // else
} // setPixel()

/*-------------------------------------------------------------------------
 Purpose   : This function returns the color of a pixel in a screen.
  Variables: screen: [FIELD,SCREEN], Tetris playfield or main-screen
             x  : the x position of the pixel in the playfield
  	     y  : the y position of the pixel in the playfield
  Returns  : the color code for the pixel
  -------------------------------------------------------------------------*/
uint8_t getPixel(bool screen, int8_t x, int8_t y)
{
    uint8_t  col = BLACK;
    uint16_t bt;
    
    if (screen == FIELD)
    {   // Tetris playfield
        if ((x >= 0) && (y >= 0) && (x < TETRIS_MAX_X) && (y < TETRIS_MAX_Y))
        {
            bt = (1<<x);
            if ((fieldr[y] & bt) == bt) col |= RED;
            if ((fieldg[y] & bt) == bt) col |= GREEN;
            if ((fieldb[y] & bt) == bt) col |= BLUE;
        } // if
        else col = WHITE; // color for a wall
    } // if
    else
    {   // main Screen
        if ((x >= 0) && (y >= 0) && (x < SIZE_X) && (y < MAX_Y))
        {
            bt = (1<<x);
            if ((rgb_bufr[y] & bt) == bt) col |= RED;
            if ((rgb_bufg[y] & bt) == bt) col |= GREEN;
            if ((rgb_bufb[y] & bt) == bt) col |= BLUE;
        } // if
        else col = WHITE; // color for a wall
    } // else
    return col;
} // getPixel()

/*-------------------------------------------------------------------------
 Purpose   : This function draws a character with the specified colour on a
             screen. The (x,y) coordinate is that of the lower-left pixel in vertical
 	     orientation, or the lower-right pixel in horizontal orientation.
  Variables: screen : [FIELD,SCREEN], Tetris playfield or main-screen
             x      : the x position where the char is printed [0..SIZE_X-1]
             y      : the y position where the char is printed [0..SIZE_Y-1]
             ch     : the character to display on the playfield
             colour : the specified colour for the Tetris block
             hv     : Horizontal (HOR) or Vertical (VERT) orientation
             Returns: -
  -------------------------------------------------------------------------*/
void printChar(bool screen, int8_t x, int8_t y, uint8_t ch, uint8_t col, bool hv)
{
    short int byte, data, bit, chi;
    
    chi = (short int)ch;
    if (chi < 96) chi -= 32; // Convert from ASCII to internal Atari code
    if ((x >= 0) && (y >= 0) && (x < SIZE_X) && (y < MAX_Y))
    {
        for (byte = 0; byte < 8; byte++)
        {
            data = atascii[chi][7-byte];
            for (bit = 0; bit < 8; bit++)
            {
                if (hv == VERT)
                {   // Vertical
                    if (data & (1<<(7-bit)))
                         setPixel(screen,x+bit,y+byte,col);
                    else setPixel(screen,x+bit,y+byte,BLACK);
                } // if
                else
                {   // Horizontal
                    if (data & (1<<(7-bit)))
                         setPixel(screen,x+byte,y+7-bit,col);
                    else setPixel(screen,x+byte,y+7-bit,BLACK);
                } // else
            } // for
        } // for
    } // if
} // printChar()

/*-------------------------------------------------------------------------
 Purpose   : This function draws a line on a screen
  Variables: screen: [FIELD,SCREEN], Tetris playfield or main-screen
             x0    : the x coordinate of the pixel to draw a line from
  	     y0    : the y coordinate of the pixel to draw a line from
  	     x1    : the x coordinate of the pixel to draw a line to
  	     y1    : the y coordinate of the pixel to draw a line to
  	    col    : the colour of the line to draw
  Returns  : -
  -------------------------------------------------------------------------*/
void drawLine(bool screen, int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t col)
{
    uint8_t steep = (abs(y1 - y0) > abs(x1 - x0));
    int8_t tx0 = x0;
    int8_t tx1 = x1;
    int8_t ty0 = y0;
    int8_t ty1 = y1;

    if (steep)
    {
        tx0 = y0; ty0 = x0;
        tx1 = y1; ty1 = x1;
    } // if
    if (x0 > x1)
    {
        tx0 = x1; tx1 = x0;
        ty0 = y1; ty1 = y0;
    } // if

    int8_t deltax = tx1 - tx0;
    int8_t deltay = abs(ty1 - ty0);
    int8_t error = deltax / 2;
    int8_t ystep;
    int8_t y = ty0;
    int8_t x;

    if (ty0 < ty1)
         ystep = 1;
    else ystep = -1;
    for (x = tx0; x <= tx1; x++)
    {
        if (steep) setPixel(screen, y, x, col);
        else 	   setPixel(screen, x, y, col);
        error -= deltay;
        if (error < 0)
        {
            y     += ystep;
            error += deltax;
        } // if
    } // for
} // drawLine()
