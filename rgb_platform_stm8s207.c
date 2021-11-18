/*==================================================================
  File Name: rgb_platform_stm8s207.c
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This is the main file for brew_stm8s207.c
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
#include "rgb_platform_stm8s207.h"
#include "pixel.h"
#include "tetris.h"

extern uint8_t atascii[128][8]; // Atari XL Font
extern char rs232_inbuf[];
char *revision_nr = "0.30\n"; // RGB Platform SW revision number

//-------------------------------------------------------------------------
// Array which holds the Red, Green & Blue bits for every RGB-LED.
// Bit 2: Red, Bit 1: Green, Bit 0: Blue, see defines in .h file
//-------------------------------------------------------------------------
playfield_color rgb_ledr = {0}; // The actual status of the red leds
playfield_color rgb_ledg = {0}; // The actual status of the green leds
playfield_color rgb_ledb = {0}; // The actual status of the blue leds
playfield_color rgb_bufr; // Buffered version of the red leds
playfield_color rgb_bufg; // Buffered version of the green leds
playfield_color rgb_bufb; // Buffered version of the blue leds

bool vsync = false;       // Vertical-Sync

//-------------------------------------------------------------------------
// Global variables for lichtkrant() function
//-------------------------------------------------------------------------
char    lk1[100] = "TEST"; // Text for top horizontal line
uint8_t lk1c[100]= {RED};  // Colour for every character in lk1[]
char    lk2[100];      // Text for bottom horizontal line
uint8_t lk2c[100];     // Colour for every character in lk2[]

uint8_t row1_std = 1;  // STD state for top row
uint8_t cur_row1_idx;  // points to char. being displayed
int8_t  row1_bit;      // points to bit nr. to display
uint8_t row2_std = 1;  // STD state for bottom row
uint8_t cur_row2_idx;  // points to char. being displayed
int8_t  row2_bit;      // points to bit nr. to display
uint8_t lk_changed;    // [LK1, LK2]. Flag that text is changed 

/*-------------------------------------------------------------------------
Purpose   : This is the lichtkrant task. It displays text on two rows,
            independently from each other. Text is displayed horizontally.
            Variables: see list of global variables.
Returns  : -
-------------------------------------------------------------------------*/
void lichtkrant(void)
{
    uint8_t maxch = MAX_CHAR_Y;
    uint8_t i, cy, col, chi, bit, slen;
    
    slen = strlen(lk1);
    switch (row1_std)
    {
    case 1: //Init., place 4 characters
        for (i = 0; i < maxch; i++)
        {   //        x   y    ch            col        orientation
            printChar(8,i<<3,lk1[maxch-1-i],lk1c[maxch-1-i],HOR);
        }
        if (slen > maxch)
        {
            cur_row1_idx = 4; // points to new character
            row1_bit     = 7; // start with bit 7
            row1_std     = 2; // goto next state
        } // if
        break;
    case 2: //SHL1 entire playfield and add new character
        if (lk_changed & LK1)
        {
            lk_changed &= ~LK1; // reset flag
            row1_std    = 1;
        }
        else
        {
            for (cy = SIZE_Y-1; cy > 0; cy--)
            {
                 rgb_bufr[cy] &= 0x00FF; // clear bits 15-08
                 rgb_bufr[cy] |= (rgb_bufr[cy-1] & 0xFF00);
                 rgb_bufg[cy] &= 0x00FF; // clear bits 15-08
                 rgb_bufg[cy] |= (rgb_bufg[cy-1] & 0xFF00);
                 rgb_bufb[cy] &= 0x00FF; // clear bits 15-08
                 rgb_bufb[cy] |= (rgb_bufb[cy-1] & 0xFF00);
            } // for cy
            chi = (uint8_t)lk1[cur_row1_idx]; // get new character
            col = lk1c[cur_row1_idx];
            if (chi < 96) chi -= 32; // Convert from ASCII to internal Atari code
            for (bit = 0; bit < 8; bit++)
            {
                if (atascii[chi][7-bit] & (1<<(row1_bit)))
                     setPixel(8+bit,0,col);
                else setPixel(8+bit,0,BLACK);
            } // for
            if (--row1_bit < 0)
            {
                row1_bit = 7; // start with bit 7
                if (++cur_row1_idx >= slen)
                {
                    cur_row1_idx = 0; // points to beginning of text
                } // if
            } // else
        } // else
        break;
    } // switch (row1_std)
    
    slen = strlen(lk2);
    switch (row2_std)
    {
    case 1: //Init., place 4 characters
        for (i = 0; i < maxch; i++)
        {
            printChar(0,i<<3,lk2[maxch-1-i],lk2c[maxch-1-i],HOR);
        }
        if (slen > maxch)
        {
            cur_row2_idx = 4; // points to new character
            row2_bit     = 7; // start with bit 7
            row2_std     = 2; // goto next state
        } // if
        break;
    case 2: //SHL1 entire playfield and add new character
        if (lk_changed & LK2)
        {
            lk_changed &= ~LK2; // reset flag
            row2_std    = 1;
        }
        else
        {
            for (cy = SIZE_Y-1; cy > 0; cy--)
            {
                 rgb_bufr[cy] &= 0xFF00; // clear bits 07-00
                 rgb_bufr[cy] |= (rgb_bufr[cy-1] & 0x00FF);
                 rgb_bufg[cy] &= 0xFF00; // clear bits 07-00
                 rgb_bufg[cy] |= (rgb_bufg[cy-1] & 0x00FF);
                 rgb_bufb[cy] &= 0xFF00; // clear bits 07-00
                 rgb_bufb[cy] |= (rgb_bufb[cy-1] & 0x00FF);
            } // for cy
            chi = (uint8_t)lk2[cur_row2_idx]; // get new character
            col = lk2c[cur_row2_idx];         // get color of new character
            if (chi < 96) chi -= 32; // Convert from ASCII to internal Atari code
            for (bit = 0; bit < 8; bit++)
            {
                if (atascii[chi][7-bit] & (1<<(row2_bit)))
                     setPixel(bit,0,col);
                else setPixel(bit,0,BLACK);
            } // for
            if (--row2_bit < 0)
            {
                row2_bit = 7; // start with bits 7
                if (++cur_row2_idx >= slen)
                {
                    cur_row2_idx = 0; // points to beginning of text
                } // if
                if (slen <= maxch)
                {
                    row2_std = 1; // Did text become smaller?
                } // if
            } // else
        } // else
        break;
    } // switch (row2_std)
} // lichtkrant() 

/*------------------------------------------------------------------
  Purpose  : This function prints a welcome message to the serial
             port together with the current CVS revision number.
  Variables: 
        ver: string with version info
  Returns  : -
  ------------------------------------------------------------------*/
void print_revision_nr(void)
{
    uart1_printf("RGB Platform STM8S207R8, rev. ");
    uart1_printf(revision_nr);
} // print_ebrew_revision()

/*-------------------------------------------------------------------------
  Purpose   : This functions copies the buffer built by the program into
              the rgb_led buffer, that is used to control the LEDs.
              Since a LED is enabled when the bit is 0, all bits are
              inverted when copied.
  Variables : -
  Returns   : -
  -------------------------------------------------------------------------*/
void copy_playfield(void)
{
    __disable_interrupt();
    for (uint8_t i = 0; i < MAX_Y; i++)
    {
        rgb_ledr[i] = ~rgb_bufr[i];
        rgb_ledg[i] = ~rgb_bufg[i];
        rgb_ledb[i] = ~rgb_bufb[i];
    } // for i
    __enable_interrupt(); 
} // copy_playfield() 

/*------------------------------------------------------------------
  Purpose  : This is the main() function for the RGB platform.
  Variables: -
  Returns  : should never return
  ------------------------------------------------------------------*/
int main(void)
{
    char    s[35];     // Needed for uart_printf() and sprintf()
    uint8_t clk;       // which clock is active
    uint8_t dip_sw;    // status of dip-switches
    
    __disable_interrupt();
    // For 24 MHz, set ST-LINK->Option Bytes...->Flash_Wait_states to 1
    clk = initialise_system_clock(HSE); // Set system-clock to 24 MHz
    uart1_init(clk);            // UART1 init. to 115200,8,N,1
    uart3_init(clk);            // UART3 init. to 115200,8,N,1
    setup_timers(clk);          // Set Timer 2 to 1 kHz
    setup_gpio_ports();         // Init. needed output-ports
    i2c_init_bb(I2C_CH0);       // Init. I2C bus 0 for bit-banging
    dip_sw = PE_IDR & (SW_ALL); // Read dip-switches
    
    // Initialize all the tasks for the RGB Platform
    add_task(lichtkrant, "lkrant", 100, 100); // Lichtkrant task
    add_task(tetrisMain, "tetris", 150, 500); // Tetris game
    disable_task("tetris"); // disable Tetris for now
    
    __enable_interrupt(); // set global interrupt enable, start task-scheduler
	
    print_revision_nr();  // print revision nr to UART 1
    sprintf(s,"CLK: 0x%X ",clk);
    uart1_printf(s);
    if      (clk == HSI) uart1_printf("HSI\n");
    else if (clk == LSI) uart1_printf("LSI\n");
    else if (clk == HSE) uart1_printf("HSE\n");
    sprintf(s,"DIP-SW: 0x%X\n",dip_sw);
    uart1_printf(s); // print status of dip-switches
    
    while (true)
    {   // main loop
        dispatch_tasks(); // run the task-scheduler
        switch (rs232_command_handler()) // run command handler continuously
        {
            case ERR_CMD: uart1_printf("Command Error\n"); 
                          break;
            case ERR_NUM: sprintf(s,"Number Error (%s)\n",rs232_inbuf);
                          uart1_printf(s);  
                          break;
            case ERR_I2C: break; // do not print anything 
            default     : break;
        } // switch
        if (vsync)
        {
            copy_playfield();
            vsync = false; // reset vertical sync
    	} // if
    } // while()
} // main()