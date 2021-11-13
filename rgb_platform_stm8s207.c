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
#include "stm8_hw_init.h"

extern char rs232_inbuf[];
char *revision_nr = "0.30\n"; // RGB Platform SW revision number

//-------------------------------------------------------------------------
// Array which holds the Red, Green & Blue bits for every RGB-LED.
// Bit 2: Red, Bit 1: Green, Bit 0: Blue
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
char    lk1[100];      // Text for top horizontal line
uint8_t lk1c[100];     // Colour for every character in lk1[]
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
    short int maxch = MAX_CHAR_Y;
    short int i, cx, cy, colour, chi, bit;
    
    switch (row1_std)
    {
    case 1: //Init., place 4 characters
        for (i = 0; i < maxch; i++)
        {
            //PrintChar(rgb_buf,8,i<<3,lk1[maxch-1-i],lk1c[maxch-1-i],HOR);
        }
        if (strlen(lk1) > maxch)
        {
            cur_row1_idx = 4; // points to new character
            row1_bit     = 7; // start with bits 7
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
                for (cx = 8; cx < SIZE_X; cx++)
                {
                    //colour = GetPixel(rgb_buf, cx, cy-1);
                    //SetPixel(rgb_buf, cx, cy, colour);
                } // for
            chi = (short int)lk1[cur_row1_idx]; // get new character
            colour = lk1c[cur_row1_idx];
            if (chi < 96) chi -= 32; // Convert from ASCII to internal Atari code
            for (bit = 0; bit < 8; bit++)
            {
//                if (atascii[chi][7-bit] & (1<<(row1_bit)))
//                    SetPixel(rgb_buf,8+bit,0,colour);
//                else SetPixel(rgb_buf,8+bit,0,EMPTY);
            } // for
            if (--row1_bit < 0)
            {
                row1_bit = 7; // start with bits 7
                if (++cur_row1_idx >= strlen(lk1))
                {
                    cur_row1_idx = 0; // points to beginning of text
                } // if
            } // else
        } // else
        break;
    } // switch (row1_std)
    
    switch (row2_std)
    {
    case 1: //Init., place 4 characters
        for (i = 0; i < maxch; i++)
        {
            //PrintChar(rgb_buf,0,i<<3,lk2[maxch-1-i],lk2c[maxch-1-i],HOR);
        }
        if (strlen(lk2) > maxch)
        {
            cur_row2_idx = 4; // points to new character
            row2_bit     = 7; // start with bits 7
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
                for (cx = 0; cx < SIZE_X-8; cx++)
                {
                    //colour = GetPixel(rgb_buf, cx, cy-1);
                    //SetPixel(rgb_buf, cx, cy, colour);
                } // for
            chi = (short int)lk2[cur_row2_idx]; // get new character
            colour = lk2c[cur_row2_idx];
            if (chi < 96) chi -= 32; // Convert from ASCII to internal Atari code
            for (bit = 0; bit < 8; bit++)
            {
//                if (atascii[chi][7-bit] & (1<<(row2_bit)))
//                    SetPixel(rgb_buf,bit,0,colour);
//                else SetPixel(rgb_buf,bit,0,EMPTY);
            } // for
            if (--row2_bit < 0)
            {
                row2_bit = 7; // start with bits 7
                if (++cur_row2_idx >= strlen(lk2))
                {
                    cur_row2_idx = 0; // points to beginning of text
                } // if
                if (strlen(lk2)<=maxch)
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
    uart_printf("RGB Platform STM8S207R8, rev. ");
    uart_printf(revision_nr);
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
    
    __disable_interrupt();
    // For 24 MHz, set ST-LINK->Option Bytes...->Flash_Wait_states to 1
    clk = initialise_system_clock(HSE); // Set system-clock to 24 MHz
    uart_init(clk);            // UART init. to 115200,8,N,1
    setup_timers(clk);         // Set Timer 2 to 1 kHz and timer1 and timer3 for PWM output
    setup_gpio_ports();        // Init. needed output-ports for LED and keys
    i2c_init_bb(I2C_CH0);      // Init. I2C bus 0 for bit-banging
	
    // Initialize all the tasks for the RGB Platform
    //add_task(lm35_task ,"lm35_task" , 30, 2000); // Process Temperature from LM35 sensor
	
    __enable_interrupt();       // set global interrupt enable, start task-scheduler
	
    print_revision_nr();
    sprintf(s,"CLK: 0x%X ",clk);
    uart_printf(s);
    if      (clk == HSI) uart_printf("HSI\n");
    else if (clk == LSI) uart_printf("LSI\n");
    else if (clk == HSE) uart_printf("HSE\n");

    while (true)
    {   // main loop
        dispatch_tasks(); // run the task-scheduler
        switch (rs232_command_handler()) // run command handler continuously
        {
            case ERR_CMD: uart_printf("Command Error\n"); 
                          break;
            case ERR_NUM: sprintf(s,"Number Error (%s)\n",rs232_inbuf);
                          uart_printf(s);  
                          break;
            case ERR_I2C: break; // do not print anything 
            default     : break;
        } // switch
        if (vsync)
        {
            //lichtkrant();
            copy_playfield();
            vsync = false; // reset vertical sync
    	} // if
    } // while()
} // main()