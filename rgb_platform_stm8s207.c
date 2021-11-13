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