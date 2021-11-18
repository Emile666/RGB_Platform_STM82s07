/*==================================================================
  File Name: tetris.c
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This file contains the functions for the Tetris game.
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
#include "tetris.h"

extern playfield_color rgb_bufr; // Buffered version of the red leds
extern playfield_color rgb_bufg; // Buffered version of the green leds
extern playfield_color rgb_bufb; // Buffered version of the blue leds
extern uint8_t  atascii[128][8]; // Atari XL Font

int8_t   shift = 0;		// | Variables for
uint8_t  direction = 1;	        // | text output
uint8_t  nextShape[3] = {0x00}; // First 3 bits from LSB are shape, next 3 are color
uint8_t  level, count;          // current level, frame count
uint16_t score;                 // current score

uint8_t  shape;    // Type of current Tetris block
uint8_t  rotation; // Orientation of the current Tetris block
int8_t   x;        // x position of the current Tetris block
int8_t   y;        // y position of the current Tetris block

uint8_t  screen       = 0; // Index in Tetris screens
uint8_t  gameFlags    = 0; // Tetris Game-Flags
uint8_t  joystick     = 0; // The debounced status of the joystick buttons
uint8_t  old_joystick = 0; // Previous value of joystick variable

/*-------------------------------------------------------------------------
 Purpose   : This function handles the input from the joystick buttons.
             The following inputs are possible:
             UP        : Rotate the Tetris block clockwise
             DOWN      : Enable Fast-Drop of the Tetris block
             RIGHT     : Steer Tetris block one to the right
             LEFT      : Steer Tetris block one to the left
             LEFT+DOWN : Start new Tetris game
             RIGHT+DOWN: Pause Tetris game
  Variables: -
  Returns  : -
  -------------------------------------------------------------------------*/
void tetrisInputs(void)
{
    joystick = (PF_IDR & STICK_ALL); // Joystick is connected to PORTF: PF7..PF3
    if (((old_joystick ^ joystick) & (STICK_LEFT | STICK_DOWN)) == (STICK_LEFT | STICK_DOWN))
    {	// LEFT & DOWN button pressed at the same time?
        if ((joystick & (STICK_LEFT | STICK_DOWN)) == (STICK_LEFT | STICK_DOWN))
        { // LEFT & DOWN buttons are pressed
            gameFlags |= (1<<CLEAR_SHIFT);
            screen = 0; // start new game
        } // if
    } // if
    if (((old_joystick ^ joystick) & (STICK_RIGHT | STICK_DOWN)) == (STICK_RIGHT | STICK_DOWN))
    {	// RIGHT & DOWN button pressed at the same time?
        if ((joystick & (STICK_RIGHT | STICK_DOWN)) == (STICK_RIGHT | STICK_DOWN))
        { // RIGHT & DOWN buttons are pressed
            gameFlags |= (1<<CLEAR_SHIFT);
            switch (screen)
            {
                case 0:  screen++;   break; // if in new game screen, start new game
                case 1:  screen++;   break; // if in game screen, pause the game
                case 2:  screen--;   break; // if in pause screen, resume the game
                case 3:  screen = 0; break; // if in game over screen, go to new game screen
                default: screen = 0; break; // if anything else (in case somehow screen becomes > 3, go to new game screen
            } // switch
        } // if
    } // if
    if (((old_joystick ^ joystick) & STICK_UP) == STICK_UP)
    {
        if (joystick & STICK_UP)
        { // UP button is pressed
            if (++rotation > 3)	rotation = 0;
            CheckX(&x, shape, rotation);
        } // if
    } // if
    if (((old_joystick ^ joystick) & STICK_LEFT) == STICK_LEFT)
    {
        if (joystick & STICK_LEFT)
        { // LEFT button is pressed:
            if (canMoveLeft(x, y, shape, rotation))
                x--; // Shift shape to the left
        } // if
    } // if
    if (((old_joystick ^ joystick) & STICK_DOWN) == STICK_DOWN)
    {
        if (joystick & STICK_DOWN)
        { // DOWN button is pressed
            gameFlags |= (1<<FAST_DROP); // Set Fast_drop flag
        } // if
        else
        { // DOWN button is released
            gameFlags &= ~(1<<FAST_DROP); // Remove Fast Drop flag
        } // else
    } // if
    if (((old_joystick ^ joystick) & STICK_RIGHT) == STICK_RIGHT)
    {
        if (joystick & STICK_RIGHT)
        { // RIGHT button is pressed:
            if (canMoveRight(x, y, shape, rotation))
                x++; // Shift shape to the right
        } // if
    } // if
    old_joystick = joystick; // save joystick value
} // tetrisInputs()

/*-------------------------------------------------------------------------
 Purpose   : This function checks the x coordinate of a Tetris block after
 	     it has been rotated. To facilitate rotation, even when a wall
  	     blocks the rotation, the X coordinate is adjusted so that
 	     rotation remains possible.
  Variables: x        : the x position of the block in the source playfield
  	     y        : the y position of the block in the source playfield
  	     rotation : the new orientation of the Tetris block
  Returns  : -
  -------------------------------------------------------------------------*/
void CheckX(int8_t *x, int8_t shape, int8_t rotation)
{
    switch (shape)
    {
        case TYPE_I:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    if (*x > TETRIS_WALL_X-1) *x = TETRIS_WALL_X-1;
                    else if (*x < 0)          *x = 0;
                    break;
                case EAST:
                case WEST: 
                    if (*x > TETRIS_WALL_X-2) *x = TETRIS_WALL_X-2;
                    else if (*x < 2)          *x = 2;
                    break;
            } // switch
            break;
            
        case TYPE_J:
        case TYPE_L:
        case TYPE_T:
            switch (rotation)
            {
                case NORTH: 
                    if (*x > TETRIS_WALL_X-2) *x = TETRIS_WALL_X-2;
                    else if (*x < 0)          *x = 0;
                    break;
                case EAST:
                case WEST:  
                    if (*x > TETRIS_WALL_X-2) *x = TETRIS_WALL_X-2;
                    else if (*x < 1)          *x = 1;
                    break;
                case SOUTH: 
                    if (*x > TETRIS_WALL_X-1) *x = TETRIS_WALL_X-1;
                    else if (*x < 1)          *x = 1;
                    break;
            } // switch
            break;
            
        case TYPE_O:
            switch (rotation)
            {
                case NORTH:
                case EAST:
                case SOUTH:
                case WEST:  
                    if (*x > TETRIS_WALL_X-1) *x = TETRIS_WALL_X-1;
                    else if (*x < 1)          *x = 1;
                    break;
            } // switch
            break;
            
        case TYPE_Z:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    if (*x > TETRIS_WALL_X-2) *x = TETRIS_WALL_X-2;
                    else if (*x < 0)          *x = 0;
                    break;
                case EAST:
                case WEST: 
                    if (*x > TETRIS_WALL_X-2) *x = TETRIS_WALL_X-2;
                    else if (*x < 1)          *x = 1;
                break;
            } // switch
            break;
            
        case TYPE_S:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    if (*x > TETRIS_WALL_X-1) *x = TETRIS_WALL_X-1;
                    else if (*x < 1)          *x = 1;
                    break;
                case EAST:
                case WEST: 
                    if (*x > TETRIS_WALL_X-2) *x = TETRIS_WALL_X-2;
                    else if (*x < 1)          *x = 1;
                    break;
            } // switch
            break;
    } // switch
} // checkX()

/*-------------------------------------------------------------------------
 Purpose   : This function moves a block in the playfield one row down
  Variables: src_x : the x position of the block in the playfield
  	     src_y : the y position of the block in the playfield
  	     width : the width in the x-position of the block to copy
  	     height: the height in the y-position of the block to copy
  	     dest_x: the x position of the destination block
  	     dest_y: the y position of the destination block
  Returns  : -
  -------------------------------------------------------------------------*/
void downOneRow(int8_t src_x , int8_t src_y, uint8_t width, uint8_t height, int8_t dest_x, int8_t dest_y)
{
    int8_t  cx, cy;
    uint8_t col;
    
    for (cy = height-1; cy >= 0; cy--)
        for (cx = 0; cx < width; cx++)
        {
            col = getPixel(src_x + cx, src_y + cy);
            setPixel(dest_x + cx, dest_y + cy, col);
        } // for
} // downOneRow()

/*-------------------------------------------------------------------------
 Purpose   : This function checks if the Tetris block can move in the
 	     RIGHT direction. The right of the playfield is limited by
 	     the vertical line at x = TETRIS_WALL_X.
  Variables: field    : pointer to 2D playfield
  	     x        : the x position of the Tetris block [0..SIZE_X-1]
  	     y        : the y position of the Tetris block [0..SIZE_Y-1]
  	     shape    : the shape-type of the Tetris block
  	     rotation : the current rotation of the Tetris block
  Returns  : true = block can move to the right, false = block can NOT move
  -------------------------------------------------------------------------*/
bool canMoveRight(int8_t x, int8_t y, uint8_t shape, uint8_t rotation)
{
    bool px = false; // true = can move 1 position to the right
    switch (shape)
    {
        case TYPE_I:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    px = (x > TETRIS_WALL_X - 2) || getPixel(x+1,y)   || getPixel(x+1,y+1) || 
                         getPixel(x+1,y-1) || getPixel(x+1,y-2);
                    break;
                case EAST:
                case WEST:  
                    px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y);
                    break;
            } // switch
            break; // case TYPE_I
            
            case TYPE_L:
                switch (rotation)
                {
                    case NORTH: 
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+1,y) || 
                             getPixel(x+1,y+1) || getPixel(x+2,y-1);
                        break;
                    case EAST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x,y-1);
                        break;
                    case SOUTH: 
                        px = (x > TETRIS_WALL_X - 2) || getPixel(x+1,y) || 
                             getPixel(x+1,y+1) || getPixel(x+1,y-1);
                        break;
                    case WEST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x+2,y+1);
                        break;
                } // switch
                break; // case TYPE_L
                
            case TYPE_J:
                switch (rotation)
                {
                    case NORTH: 
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+1,y) || 
                             getPixel(x+2,y+1) || getPixel(x+1,y-1);
                        break;
                    case EAST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x+2,y-1);
                        break;
                    case SOUTH: 
                        px = (x > TETRIS_WALL_X - 2) || getPixel(x+1,y) || 
                             getPixel(x+1,y+1) || getPixel(x+1,y-1);
                        break;
                    case WEST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x,y+1);
                        break;
                } // switch
                break; // case TYPE_J
                
            case TYPE_O:		
                px = (x > TETRIS_WALL_X - 2) || getPixel(x+1,y) || getPixel(x+1,y-1);
                break; // case TYPE_O
                
            case TYPE_Z:
                switch (rotation)
                {
                    case NORTH:
                    case SOUTH: 
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || 
                             getPixel(x+2,y+1) || getPixel(x+1,y-1);
                        break;
                    case EAST:
                    case WEST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+1,y) || getPixel(x+2,y-1);
                        break;
                } // switch
                break; // case TYPE_Z
                
            case TYPE_S:
                switch (rotation)
                {
                    case NORTH:
                    case SOUTH: 
                        px = (x > TETRIS_WALL_X - 2) || getPixel(x+1,y) || 
                             getPixel(x+1,y-1) || getPixel(x,y+1);
                        break;
                    case EAST:
                    case WEST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x+1,y-1);
                        break;
                } // switch
                break; // case TYPE_S
                
            case TYPE_T:
                switch (rotation)
                {
                    case NORTH: 
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || 
                             getPixel(x+1,y+1) || getPixel(x+1,y-1);
                        break;
                    case EAST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x+1,y-1);
                        break;
                    case SOUTH: 
                        px = (x > TETRIS_WALL_X - 2) || getPixel(x+1,y) || 
                             getPixel(x+1,y+1) || getPixel(x+1,y-1);
                        break;
                    case WEST:  
                        px = (x > TETRIS_WALL_X - 3) || getPixel(x+2,y) || getPixel(x+1,y+1);
                        break;
                } // switch
                break; // case TYPE_T
    } // switch shape
    return !px; // if pixel found, then no move possible
} // canMoveRight()

/*-------------------------------------------------------------------------
 Purpose   : This function checks if the Tetris block can move in the
 	     LEFT direction. The left side of the playfield is limited by
 	     the left side of the playfield (x = 0)
  Variables: field    : pointer to 2D playfield
  	     x        : the x position of the Tetris block [0..SIZE_X-1]
  	     y        : the y position of the Tetris block [0..SIZE_Y-1]
  	     shape    : the shape-type of the Tetris block
  	     rotation : the current rotation of the Tetris block
  Returns  : true = block can move to the left, false = block can NOT move
  -------------------------------------------------------------------------*/
bool canMoveLeft(int8_t x, int8_t y, uint8_t shape, uint8_t rotation)
{
    bool px = false; // // true = can move 1 position to the left

    switch (shape)
    {
        case TYPE_I:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    px = getPixel(x-1,y)   || getPixel(x-1,y+1) || 
                         getPixel(x-1,y-1) || getPixel(x-1,y-2);
                    break;
                case EAST:
                case WEST:  
                    px = getPixel(x-3,y);
                    break;
            } // switch
            break; // case TYPE_I
            
        case TYPE_L:
            switch (rotation)
            {
                case NORTH: 
                    px = getPixel(x-1,y) || getPixel(x-1,y+1) || getPixel(x-1,y-1);
                    break;
                case EAST:  
                    px = getPixel(x-2,y) || getPixel(x-2,y-1);
                    break;
                case SOUTH: 
                    px = getPixel(x-1,y) || getPixel(x-2,y+1) || getPixel(x-1,y-1);
                    break;
                case WEST:  
                    px = getPixel(x-2,y) || getPixel(x,y+1);
                    break;
            } // switch
            break; // case TYPE_L
            
        case TYPE_J:
            switch (rotation)
            {
                case NORTH: 
                    px = getPixel(x-1,y) || getPixel(x-1,y+1) || getPixel(x-1,y-1);
                    break;
                case EAST:  
                    px = getPixel(x-2,y) || getPixel(x,y-1);
                    break;
                case SOUTH: 
                    px = getPixel(x-1,y) || getPixel(x-1,y+1) || getPixel(x-2,y-1);
                    break;
                case WEST:  
                    px = getPixel(x-2,y) || getPixel(x-2,y+1);
                    break;
            } // switch
            break; // case TYPE_J
            
        case TYPE_O:
            px = getPixel(x-2,y) || getPixel(x-2,y-1);
            break; // case TYPE_O
            
        case TYPE_Z:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    px = getPixel(x-1,y) || getPixel(x,y+1) || getPixel(x-1,y-1);
                    break;
                case EAST:
                case WEST:  
                    px = getPixel(x-2,y) || getPixel(x-1,y-1);
                    break;
            } // switch
            break; // case TYPE_Z
            
        case TYPE_S:
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    px = getPixel(x-2,y) || getPixel(x-1,y-1) || getPixel(x-2,y+1);
                    break;
                case EAST:
                case WEST:  
                    px = getPixel(x-1,y) || getPixel(x-2,y-1);
                    break;
            } // switch
            break; // case TYPE_S
            
        case TYPE_T:
            switch (rotation)
            {
                case NORTH: 
                    px = getPixel(x-1,y) || getPixel(x-1,y+1) || getPixel(x-1,y-1);
                    break;
                case EAST:  
                    px = getPixel(x-2,y) || getPixel(x-1,y-1);
                    break;
                case SOUTH: 
                    px = getPixel(x-2,y) || getPixel(x-1,y+1) || getPixel(x-1,y-1);
                    break;
                case WEST:  
                    px = getPixel( x-2, y) || getPixel( x-1, y+1);
                    break;
            } // switch
            break; // case TYPE_T
    } // switch shape
    return !px; // if pixel found, then no move possible
} // canMoveLeft()

/*-------------------------------------------------------------------------
 Purpose   : This function checks if the Tetris block should be placed on
             the playfield, because there is no more space free to move
             down. If the Tetris block is placed outside the playfield
             (the initial position), this function returns a 0 indicating
             that the block can move further down.
  Variables: field    : pointer to 2D playfield
  	  	  	 x        : the x position of the Tetris block [0..SIZE_X-1]
  	  	  	 y        : the y position of the Tetris block [0..SIZE_Y-1]
  	  	  	 shape    : the shape-type of the Tetris block
  	  	  	 rotation : the current rotation of the Tetris block
  Returns  : true = block should be placed ; false = block can move further down
  -------------------------------------------------------------------------*/
bool shouldPlace(int8_t x, int8_t y, uint8_t shape, uint8_t rotation)
{
    bool retv = false;
    
    if ((x >= 0) && (x < SIZE_X) && (y < SIZE_Y))
    {
        switch (shape)
        {
            case TYPE_I:
                switch (rotation)
                {
                    case NORTH:
                    case SOUTH: 
                        retv = ((y < 3) || getPixel(x,y-3)); 
                        break;
                    case EAST:
                    case WEST:	
                        retv = ((y < 1) || getPixel(x  ,y-1) || getPixel(x+1,y-1) ||
                                           getPixel(x-1,y-1) || getPixel(x-2,y-1));
                        break;
                } // rotation
                break; // TYPE_I
                
            case TYPE_L:
                switch (rotation)
                {
                    case NORTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y-2));
                        break;
                    case EAST:  
                        retv = ((y < 2) || getPixel(x,y-1) || getPixel(x+1,y-1) || getPixel(x-1,y-2)); 
                        break;
                    case SOUTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x-1,y));
                        break;
                    case WEST:	
                        retv = ((y < 1) || getPixel(x,y-1) || getPixel(x+1,y-1) || getPixel(x-1,y-1));
                        break;
                } // rotation
                break; // TYPE_L
                
            case TYPE_J:
                switch (rotation)
                {
                    case NORTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y));
                        break;
                    case EAST:  
                        retv = ((y < 2) || getPixel(x,y-1) || getPixel(x+1,y-2) || getPixel(x-1,y-1));
                        break;
                    case SOUTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x-1,y-2));
                        break;
                    case WEST:	
                        retv = ((y < 1) || getPixel(x,y-1) || getPixel(x+1,y-1) || getPixel(x-1,y-1));
                        break;
                } // rotation
                break; // TYPE_J
                
            case TYPE_O: // is independent of rotation
                retv = ((y < 2) || getPixel(x, y-2) || getPixel(x-1,y-2));
                break; // TYPE_O
                
            case TYPE_Z:
                switch (rotation)
                {
                    case NORTH:
                    case SOUTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y-1));
                        break;
                    case EAST:
                    case WEST:	
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y-2) || getPixel(x-1,y-1));
                        break;
                } // rotation
                break; // TYPE_Z
                
            case TYPE_S:
                switch (rotation)
                {
                    case NORTH:
                    case SOUTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x-1,y-1));
                        break;
                    case EAST:
                    case WEST:	
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y-1) || getPixel(x-1,y-2));
                        break;
                } // rotation
                break; // TYPE_Z
                
            case TYPE_T:
                switch (rotation)
                {
                    case NORTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y-1));
                        break;
                    case EAST:  
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x+1,y-1) || getPixel(x-1,y-1));
                        break;
                    case SOUTH: 
                        retv = ((y < 2) || getPixel(x,y-2) || getPixel(x-1,y-1));
                        break;
                    case WEST:	
                        retv = ((y < 1) || getPixel(x,y-1) || getPixel(x+1,y-1) || getPixel(x-1,y-1));
                        break;
                } // rotation
                break; // TYPE_T
        } // switch (shape)
    } // if
    return retv; // true = Tetris block can NOT move further down
} // ShouldPlace()

/*-------------------------------------------------------------------------
 Purpose   : This function draws a Tetris block with the specified colour.
  Variables: p        : a pointer to the 2D playfield
  	  	  	 x        : the x position of the Tetris block [0..SIZE_X-1]
  	  	  	 y        : the y position of the Tetris block [0..SIZE_Y-1]
  	  	  	 shape    : the shape-type of the Tetris block
  	  	  	 rotation : the current rotation of the Tetris block
  Returns  : 1 = block can move ; 0 = block can NOT move
  -------------------------------------------------------------------------*/
void drawShape(int8_t x, int8_t y, uint8_t shape, uint8_t rotation)
{
    uint8_t col; // fixed colour per Tetris block type

    switch (shape)
    {
        case TYPE_I:
            col = COLOUR_TYPE_I;
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    setPixel(x,y+1,col); 
                    setPixel(x,y-1,col);   
                    setPixel(x,y-2,col);
                    break;
                case EAST:
                case WEST:  
                    setPixel(x+1,y,col); 
                    setPixel(x-1,y,col);   
                    setPixel(x-2,y,col);
                    break;
            } // switch
            break; // TYPE_I
            
        case TYPE_L:
            col = COLOUR_TYPE_L;
            switch (rotation)
            {
                case NORTH: 
                    setPixel(x  ,y+1,col); 
                    setPixel(x  ,y-1,col);   
                    setPixel(x+1,y-1,col);
                    break;
                case EAST:	
                    setPixel(x+1,y  ,col); 
                    setPixel(x-1,y  ,col);   
                    setPixel(x-1,y-1,col);
                    break;
                case SOUTH: 
                    setPixel(x  ,y+1,col); 
                    setPixel(x-1,y+1,col); 
                    setPixel(x  ,y-1,col);
                    break;
                case WEST:	
                    setPixel(x+1,y  ,col); 
                    setPixel(x-1,y  ,col);   
                    setPixel(x+1,y+1,col);
                    break;
            } // switch
            break; // TYPE_L
            
        case TYPE_J:
            col = COLOUR_TYPE_J;
            switch (rotation)
            {
                case NORTH: 
                    setPixel(x  ,y+1,col); 
                    setPixel(x+1,y+1,col); 
                    setPixel(x  ,y-1,col);
                break;
                case EAST:  
                    setPixel(x-1,y  ,col); 
                    setPixel(x+1,y  ,col);   
                    setPixel(x+1,y-1,col);
                break;
                case SOUTH: 
                    setPixel(x  ,y+1,col); 
                    setPixel(x  ,y-1,col);   
                    setPixel(x-1,y-1,col);
                break;
                case WEST:  
                    setPixel(x-1,y  ,col); 
                    setPixel(x+1,y  ,col);   
                    setPixel(x-1,y+1,col);
                break;
            } // switch
            break; // TYPE_J
            
        case TYPE_O:
            col = COLOUR_TYPE_O;
            setPixel(x-1,y  ,col); 
            setPixel(x  ,y-1,col);   
            setPixel(x-1,y-1,col);
            break; // TYPE_O
            
        case TYPE_Z:
            col = COLOUR_TYPE_Z;
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    setPixel(x+1,y  ,col); 
                    setPixel(x+1,y+1,col); 
                    setPixel(x  ,y-1,col);
                    break;
                case EAST:
                case WEST:  
                    setPixel(x-1,y  ,col); 
                    setPixel(x  ,y-1,col);   
                    setPixel(x+1,y-1,col);
                    break;
            } // switch
            break; // TYPE_Z
            
        case TYPE_S:
            col = COLOUR_TYPE_S;
            switch (rotation)
            {
                case NORTH:
                case SOUTH: 
                    setPixel(x-1,y  ,col); 
                    setPixel(x-1,y+1,col); 
                    setPixel(x  ,y-1,col);
                    break;
                case EAST:
                case WEST:  
                    setPixel(x+1,y  ,col); 
                    setPixel(x  ,y-1,col);   
                    setPixel(x-1,y-1,col);
                    break;
            } // switch
            break; // TYPE_S
            
        case TYPE_T:
            col = COLOUR_TYPE_T;
            switch (rotation)
            {
                case NORTH: 
                    setPixel(x+1,y  ,col); 
                    setPixel(x  ,y+1,col);   
                    setPixel(x  ,y-1,col);
                break;
                case EAST:  
                    setPixel(x+1,y  ,col); 
                    setPixel(x-1,y  ,col);   
                    setPixel(x  ,y-1,col);
                break;
                case SOUTH: 
                    setPixel(x-1,y  ,col); 
                    setPixel(x  ,y+1,col);   
                    setPixel(x  ,y-1,col);
                break;
                case WEST:  
                    setPixel(x-1,y  ,col); 
                    setPixel(x+1,y  ,col);   
                    setPixel(x  ,y+1,col);
                break;
            } // switch
            break; // TYPE_T
    } // switch (shape)
    setPixel(x,y,col); // always set base pixel of Tetris block
} // drawShape()

/*-------------------------------------------------------------------------
  Purpose   : the Tetris Game Screen
  Variables : -
  Returns   : -
  -------------------------------------------------------------------------*/
void tetrisGameScreen(void)
{
    int8_t tmpRow, tmpCol, fflag;

    if (gameFlags & (1<<NEW_GAME)) // Game just started
    {
        nextShape[0] = (RandomNumber() % 7); // | Fill in
        nextShape[1] = (RandomNumber() % 7); // | the shape
        nextShape[2] = (RandomNumber() % 7); // | stack
        score     = 0;
        count     = 0;
        //FillRect(field,0,0,SIZE_X,MAX_Y,EMPTY); // Clear entire RGB Platform
        shift     = 0;
        direction = 1;
        // unset new_game flag and all others, just in case
        gameFlags = (1<<NEW_SHAPE); // Generate new shape
    } // if

    level = (score / LEVEL_GAIN) + 1; // increase level every LEVEL_GAIN points
    if (level > MAX_LEVEL) level = MAX_LEVEL;

    if (gameFlags & (1<<PLACE_SHAPE)) // Check whether we need to save shape this frame
    {
        if (y > SIZE_Y-2) // No way to build higher
        {
            gameFlags |= (1<<CLEAR_SHIFT);
            screen = 3; // Game over
            return;     // Back to tetris_main()
        } // if
        gameFlags |= (1<<NEW_SHAPE);      // Need to start new shape
        drawShape(x, y, shape, rotation); // Place Tetris block on playfield
        score += 10; // add 10 points for every positioned shape

        // scan the whole gaming field to look for the rows that need to be erased
        for (tmpRow = 0; tmpRow < MAX_Y-1; tmpRow++)
        {
            fflag  = 1;// Hope for the best
            tmpCol = 0;
            while ((tmpCol < TETRIS_WALL_X) && fflag)
            {
                if (getPixel(tmpCol, tmpRow) == EMPTY)
                {
                    fflag = 0; // row is NOT completely full
                } // if
                tmpCol++;
            } // while tmpCol

            if (fflag) // If the flag is still up, we must've found a full row
            {
                drawLine(0, tmpRow, TETRIS_WALL_X-1, tmpRow, WHITE); // Fill in Found Row with white
                score += 100; // add 100 points for every deleted row
                count  = 0;
                gameFlags |= (1<<ROW_FOUND);
            } // if
        } // for
        gameFlags |=  (1<<NEW_SHAPE);
        gameFlags &= ~(1<<PLACE_SHAPE);
    } // if

    if (gameFlags & (1<<NEW_SHAPE)) // pop shape in line and generate new shape
    {
        y            = SIZE_Y+1; // start OUTSIDE of playfield
        x            = 6;        //
        rotation     = EAST;     // default rotation
        shape        = nextShape[0];
        nextShape[0] = nextShape[1];
        nextShape[1] = nextShape[2];
        nextShape[2] = RandomNumber() % 7;
        gameFlags &= ~(1<<NEW_SHAPE); // unset new_shape flag
    } // if

    drawLine(TETRIS_WALL_X, 0, TETRIS_WALL_X, SIZE_Y-1, WHITE); // Line separating gaming area from shape stack
    drawShape(SIZE_X-2, SIZE_Y- 2, nextShape[0], nextShape[0] == TYPE_I ? NORTH : EAST);	// |   Shapes
    drawShape(SIZE_X-2, SIZE_Y- 7, nextShape[1], nextShape[1] == TYPE_I ? NORTH : EAST);	// |    in a
    drawShape(SIZE_X-2, SIZE_Y-12, nextShape[2], nextShape[2] == TYPE_I ? NORTH : EAST);	// | Shape stack
    drawShape(x, y, shape, rotation); // Draw current playable shape

    if (gameFlags & (1<<ROW_FOUND))
    {
        if (count > (MAX_LEVEL - level)) // Full row stays white until we reach needed frame count
        {
            for (tmpRow=0; tmpRow<SIZE_Y; tmpRow++)
            {
                fflag  = 1;// Hope for the best
                tmpCol = 0;
                while ((tmpCol < TETRIS_WALL_X) && fflag)
                {
                    if (getPixel(tmpCol,tmpRow) == EMPTY)
                    {
                        fflag = 0; // row is NOT completely full
                    } // if
                    tmpCol++;
                } // while tmpCol

                if (fflag) // If the flag is still up, we must have found a full row
                {   // Shift gaming area 1 down
                    downOneRow(0, tmpRow+1, TETRIS_WALL_X, SIZE_Y-tmpRow-1, 0, tmpRow); // shift gaming area 1 down
                    tmpRow--; // since we just deleted a row, we need to update the row number
                } // if
            } // for
            gameFlags &= ~(1<<ROW_FOUND);
            count = 0;
        } // if
    } // if
    else if (((gameFlags & (1<<FAST_DROP)) && (count >1)) || ((!(gameFlags & (1<<FAST_DROP))) && (count > (MAX_LEVEL - level))))
    { 	// if we are dropping shape fast then wait till count > 1, else wait till it's bigger then MAX_LEVEL-level
        gameFlags |= (shouldPlace(x, y, shape, rotation) << PLACE_SHAPE); // Check pixels to see when the shape reaches bottom or a pile
        if (!(gameFlags & (1<<PLACE_SHAPE)))
                y--;
        if (gameFlags & (1<<FAST_DROP))
                score++; // add one point for every lowering with a fast drop
        count = 0;
    } // if
    count++;
} // tetrisGameScreen()

/*-------------------------------------------------------------------------
  Purpose   : the Tetris Menu Screen
  Variables : -
  Returns   : -
  -------------------------------------------------------------------------*/
void tetrisMenuScreen(void)
{
	gameFlags |= (1<<NEW_GAME);
	printChar( 2, shift+40, 'T', RED    ,VERT);
	printChar( 2, shift+32, 'E', GREEN  ,VERT);
	printChar( 2, shift+24, 'T', BLUE   ,VERT);
	printChar( 2, shift+16, 'R', MAGENTA,VERT);
	printChar( 2, shift+ 8, 'I', YELLOW ,VERT);
	printChar( 2, shift   , 'S', WHITE  ,VERT);
	drawShape(11, shift+4 , TYPE_T, EAST);
	drawShape(11, shift+12, TYPE_S, EAST);
	drawShape(11, shift+20, TYPE_Z, EAST);
	drawShape(11, shift+28, TYPE_J, EAST);
	drawShape(11, shift+36, TYPE_O, EAST);
	drawShape(11, shift+44, TYPE_L, EAST);

	if (direction) shift++;
	else           shift--;
	if ((shift >= 32) || (shift < -16))
		direction = !direction;
} // tetrisMenuScreen()

/*-------------------------------------------------------------------------
  Purpose   : the Tetris Pause Screen
  Variables : -
  Returns   : -
  -------------------------------------------------------------------------*/
void tetrisPauseScreen(void)
{
	short int lvl1, lvl2;
	int tmpLevel = level;
        
	printChar(1,shift+29,'P', RED, VERT);
	printChar(1,shift+21,'a', RED, VERT);
	printChar(1,shift+14,'u', RED, VERT);
	printChar(1,shift+ 7,'s', RED, VERT);
	printChar(1,shift   ,'e', RED, VERT);

	printChar(9,shift+37,'L', GREEN, VERT);
	printChar(9,shift+29,'e', GREEN, VERT);
	printChar(9,shift+21,'v', GREEN, VERT);
	printChar(9,shift+14,'e', GREEN, VERT);
	printChar(9,shift+ 7,'l', GREEN, VERT);
	lvl2 = (tmpLevel / 10);
	tmpLevel -= lvl2*10;
	lvl1 = tmpLevel;

	printChar(9,shift  ,'0'+lvl2, YELLOW,VERT); // | Current
	printChar(9,shift-7,'0'+lvl1, YELLOW,VERT); // |  Level

	if (direction) shift++;
	else	       shift--;
	if ((shift >= 7) || (shift <= -29))
		direction = !direction;
} // tetrisPauseScreen()

/*-------------------------------------------------------------------------
  Purpose   : the Tetris Game-Over Screen
  Variables : -
  Returns   : -
  -------------------------------------------------------------------------*/
void tetrisGameOverScreen(void)
{
	short int ch1, ch2, ch3, ch4, ch5, ch6;
	int tmpScore = score;
        
	printChar(1,shift+56, 'G', RED,VERT);
	printChar(1,shift+48, 'A', RED,VERT);
	printChar(1,shift+40, 'M', RED,VERT);
	printChar(1,shift+32, 'E', RED,VERT);

	printChar(1,shift+24, 'O', RED,VERT);
	printChar(1,shift+16, 'V', RED,VERT);
	printChar(1,shift+ 8, 'E', RED,VERT);
	printChar(1,shift   , 'R', RED,VERT);
	ch6 = tmpScore / 100000; tmpScore -= ch6 * 100000;
	ch5 = tmpScore /  10000; tmpScore -= ch5 *  10000;
	ch4 = tmpScore /   1000; tmpScore -= ch4 *   1000;
	ch3 = tmpScore /    100; tmpScore -= ch3 *    100;
	ch2 = tmpScore /    10;  tmpScore -= ch2 *     10;
	ch1 = tmpScore;

	printChar(9,shift+48, '0'+ch6, YELLOW,VERT); // |
	printChar(9,shift+40, '0'+ch5, YELLOW,VERT); // | Final
	printChar(9,shift+32, '0'+ch4, YELLOW,VERT); // |
	printChar(9,shift+24, '0'+ch3, YELLOW,VERT); // |
	printChar(9,shift+16, '0'+ch2, YELLOW,VERT); // |	score
	printChar(9,shift+ 8, '0'+ch1, YELLOW,VERT); // |

	if (direction) shift++;
	else	       shift--;
	if ((shift >= 24) || (shift <= -56))
		direction = !direction;
} // tetrisGameOverScreen()

/*-------------------------------------------------------------------------
  Purpose   : Main entry-point for the Tetris Game
  Variables : -
  Returns   : -
  -------------------------------------------------------------------------*/
void tetrisMain(void)
{
    clearScreen();  // Clear the entire screen
    tetrisInputs(); // read joystick values and update game Flags

    if (gameFlags & (1<<CLEAR_SHIFT))
    {
        shift     = 0;
        direction = 1;
        gameFlags &= ~(1<<CLEAR_SHIFT);
    } // if
    switch (screen)
    {
        case 0:  // Menu Screen
                 tetrisMenuScreen();
                 break;
        case 1:  // Game screen
                 tetrisGameScreen();
                 break;
        case 2:  // pause screen
                 tetrisPauseScreen();
                 break;
        case 3:  // Game-Over screen
                 tetrisGameOverScreen();
                 break;
        default: screen = 0; // Menu Screen
                 break;
    } // switch
} // tetris_main(()
