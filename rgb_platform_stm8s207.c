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
#include "eep.h"
#include "i2c_bb.h"
#include "i2c_ds3231_bb.h"

char   *revision_nr = "0.31\n"; // RGB Platform SW revision number
extern uint8_t atascii[128][8]; // Atari XL Font
extern char rs232_inbuf[];
bool   dst_active = false; // true = Daylight Saving Time active
Time   dt;                 // Struct with time and date values, updated every sec.

//-------------------------------------------------------------------------
// Array which holds the Red, Green & Blue bits for every RGB-LED.
// Rows range from 0 (bottom-row) to MAX_Y
// Bit 0 is the 
// rgb_buf[0] is row 0 (bottom-row), rows range from 0..MAX_Y
// Bit 2: Red, Bit 1: Green, Bit 0: Blue, see defines in pixel.h file
//-------------------------------------------------------------------------
uint16_t rgb_bufr[MAX_Y] = {0}; // The actual status of the red leds
uint16_t rgb_bufg[MAX_Y] = {0}; // The actual status of the green leds
uint16_t rgb_bufb[MAX_Y] = {0}; // The actual status of the blue leds

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
uint8_t lk_status;     // Lichtkrant status [LK1, LK2, LK1SHL, LK2SHL].

const char dows[8][10]   = {"","Maandag","Dinsdag","Woensdag","Donderdag","Vrijdag","Zaterdag","Zondag"};
const char months[13][10] = {"","Januari","Februari","Maart"    ,"April"  ,"Mei"     ,"Juni",
                                "Juli"   ,"Augustus","September","Oktober","November","December"};

/*-------------------------------------------------------------------------
Purpose   : This is the lichtkrant task. It displays text on two rows,
            independently from each other. Text is displayed horizontally.
			The top-row runs twice as fast as the bottom row, top-row shows
			date, time and temperature, bottom row shows a custom text.
            Variables: see list of global variables.
Returns  : -
-------------------------------------------------------------------------*/
void lichtkrant(void)
{
    static bool lk2run = false;
    
    BG_LEDb = 1;               // Time-measurement
    lichtkrant1();             // top row
    if (lk2run) lichtkrant2(); // bottom row
    lk2run = !lk2run;          // twice as slow as top row
    BG_LEDb = 0;               // Stop time-measurement
} // lichtkrant()

/*-------------------------------------------------------------------------
Purpose   : This is the top-level lichtkrant task. It uses the top row.
            Text is displayed horizontally.
            Variables: see list of global variables.
Returns  : -
-------------------------------------------------------------------------*/
void lichtkrant1(void)
{
    uint8_t maxch = MAX_CHAR_Y;
    uint8_t i, cy, col, chi, bit, slen;
    //char    s2[40]; // Used for printing date and time
    //int16_t temp;   // DS3231 temperature
    //Time    p;      // DS3231 date and time

/* 	ds3231_gettime(&p); // get date and time from RTC
	strcpy(lk1,dows[p.dow & 0x07]);
	sprintf(s2," %02d-%02d-%d, %02d:%02d:%02d ",p.date,p.mon,p.year,p.hour,p.min,p.sec); 
	strcat(lk1,s2);
	if (dst_active) strcat(lk1,"winter");
	else            strcat(lk1,"zomer");
	sprintf(s2,"tijd, temperatuur = %d.",temp>>2);
	strcat(lk1,s2);
    switch (temp & 0x03)
	{
		 case 0: strcat(lk1,"00"); break;
		 case 1: strcat(lk1,"25"); break;
		 case 2: strcat(lk1,"50"); break;
		 case 3: strcat(lk1,"75"); break;
	} // switch
	strcat(lk1,"    ");
 */    
    slen = strlen(lk1);
    switch (row1_std)
    {
    case 1: //Init., place 4 characters
        for (i = 0; i < maxch; i++)
        {   //        x   y    ch            col        orientation
            printChar(SCREEN,8,i<<3,lk1[maxch-1-i],lk1c[maxch-1-i],HOR);
        }
        if (slen > maxch)
        {
            cur_row1_idx = 4; // points to new character
            row1_bit     = 7; // start with bit 7
            row1_std     = 2; // goto next state
        } // if
        break;
    case 2: //SHL1 entire playfield and add new character
        if (lk_status & LK1)
        {
            lk_status &= ~LK1; // reset flag
            row1_std    = 1;
        }
        else
        {
            for (cy = MAX_Y-1; cy > 0; cy--)
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
                     setPixel(SCREEN,8+bit,0,col);
                else setPixel(SCREEN,8+bit,0,BLACK);
            } // for
            if (--row1_bit < 0)
            {
                row1_bit = 7; // start with bit 7
                if (++cur_row1_idx >= slen)
                {
                    cur_row1_idx = 0; // points to beginning of text
                    for (i = 0; i < slen; i++)
                    {   // change colors after 1 full run.
                        lk1c[i]++;
                        if      (lk1c[i] > WHITE)  lk1c[i] = CYAN;
                        else if (lk1c[i] == BLACK) lk1c[i] = MAGENTA;
                    } // for i
                } // if
            } // else
        } // else
        break;
    } // switch (row1_std)
} // lichtkrant1() 

/*-------------------------------------------------------------------------
Purpose   : This is the bottom-level lichtkrant task. It uses the bottom row.
            Text is displayed horizontally.
            Variables: see list of global variables.
Returns  : -
-------------------------------------------------------------------------*/
void lichtkrant2(void)
{
    uint8_t maxch = MAX_CHAR_Y;
    uint8_t i, cy, col, chi, bit, slen;

    slen = strlen(lk2);
    switch (row2_std)
    {
    case 1: //Init., place maxch characters
        for (i = 0; i < maxch; i++)
        {
            printChar(SCREEN,0,i<<3,lk2[maxch-1-i],lk2c[maxch-1-i],HOR);
        }
        if (slen > maxch)
        {
            cur_row2_idx = 4; // points to new character
            row2_bit     = 7; // start with bit 7
            row2_std     = 2; // goto next state
        } // if
        break;
    case 2: //SHL1 entire playfield and add new character
        if (lk_status & LK2)
        {
            lk_status &= ~LK2; // reset flag
            row2_std    = 1;
        }
        else
        {
            for (cy = MAX_Y-1; cy > 0; cy--)
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
                     setPixel(SCREEN,bit,0,col);
                else setPixel(SCREEN,bit,0,BLACK);
            } // for
            if (--row2_bit < 0)
            {
                row2_bit = 7; // start with bits 7
                if (++cur_row2_idx >= slen)
                {
                    cur_row2_idx = 0; // points to beginning of text
                    for (i = 0; i < slen; i++)
                    {   // change colors after 1 full run.
                        lk2c[i]++;
                        if      (lk2c[i] > WHITE)  lk2c[i] = CYAN;
                        else if (lk2c[i] == BLACK) lk2c[i] = MAGENTA;
                    } // for i
                } // if
            } // else
        } // else
        break;
    } // switch (row2_std)
} // lichtkrant2() 

/*-----------------------------------------------------------------------------
  Purpose: This function colors the text-input for display as a lichtkrant.
           The first letter of a word gets another color, every word also
           gets another color.
  Variables: 
      s   : the string that contains the text to color
      scol: the string contaings the colors
  Returns : -
  ---------------------------------------------------------------------------*/
void color_text_input(char *s, uint8_t *scol)
{
    uint8_t col = BLUE;
    uint8_t cap = true;
    
    for (uint8_t i = 0; i < strlen(s); i++)
    {
        if (cap == true)
        {
            cap = false;
            if (col != WHITE) 
                 scol[i] = ~col & 0x07; // invert color of first letter
            else scol[i] = BLUE;
        } // if
        else scol[i] = col;
        if (s[i] == ' ')
        {
            col++;
            if (col > WHITE) col = BLUE;
            cap = true;
        } // if
    } // for i   
} // color_text_input()

/*-------------------------------------------------------------------------
Purpose   : This is a test task. It lights the red, green and blue leds once
            every 2 seconds.
 Variables: -
Returns   : -
-------------------------------------------------------------------------*/
void test_playfield(void)
{
    static uint8_t cntr = RED;
    uint8_t i;
    
   BG_LEDb = 1;        // Time-measurement
   switch (cntr)
   {
        case RED:
            for (i = 0; i < MAX_Y; i++)
            {   // set bits 15..00 of row i
                 rgb_bufr[i]  = 0xFFFF; // set red bits
                 rgb_bufg[i]  = 0x0000; // clear green bits
                 rgb_bufb[i]  = 0x0000; // clear blue bits
            } // for i
            cntr = GREEN;
            break;
        case GREEN:
            for (i = 0; i < MAX_Y; i++)
            {   // set bits 15..00 of row i
                 rgb_bufr[i]  = 0x0000; // clear red bits
                 rgb_bufg[i]  = 0xFFFF; // set green bits
                 rgb_bufb[i]  = 0x0000; // clear blue bits
            } // for i
            cntr = BLUE;
            break;
        case BLUE:
            for (i = 0; i < MAX_Y; i++)
            {   // set bits 15..00 of row i
                 rgb_bufr[i]  = 0x0000; // clear red bits
                 rgb_bufg[i]  = 0x0000; // clear green bits
                 rgb_bufb[i]  = 0xFFFF; // set blue bits
            } // for i
            cntr = RED;
            break;
   } // switch
   BG_LEDb = 0; // Stop time-measurement
} // test_playfield()

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

/*------------------------------------------------------------------
  Purpose  : This function reads the four dip-switches and returns
             a number between 0 and 15.
  Variables: -
  Returns  : [0..15], 
  ------------------------------------------------------------------*/
uint8_t read_dip_switches(void)
{
    uint8_t dip_sw = PE_IDR & (SW_ALL); // Read dip-switches
    
    dip_sw >>= 4;            // dip-switches are in PE7..PE4
    dip_sw = ~dip_sw & 0x0F; // invert switches (low-active)
    return dip_sw;
} // read_dip_switches()

/*------------------------------------------------------------------------
Purpose  : This task is called every minute by pattern_task(). It checks 
           for a change from summer- to wintertime and vice-versa.
           To start DST: Find the last Sunday in March  : @2 AM advance clock to 3 AM.
           To stop DST : Find the last Sunday in October: @3 AM set clock back to 2 AM (only once!).
Variables: p: pointer to time-struct
Returns  : -
------------------------------------------------------------------------*/
void check_and_set_summertime(void)
{
    uint8_t        hr,day,lsun03,lsun10,dst_eep;
    static uint8_t advance_time = 0;
    static uint8_t revert_time  = 0;
    
    if (dt.mon == 3)
    {
        day    = ds3231_calc_dow(31,3,dt.year); // Find day-of-week for March 31th
        lsun03 = 31 - (day % 7);                // Find last Sunday in March
        switch (advance_time)
        {
        case 0: if ((dt.day == lsun03) && (dt.hour == 2) && (dt.min == 0))
                {   // At 2:00 AM advance time to 3 AM, check for one minute
                    advance_time = 1;
                } // if
                else if (dt.day < lsun03) dst_active = false;
                else if (dt.day > lsun03) dst_active = true;
                else if (dt.hour < 2)     dst_active = false;
                break;
        case 1: // Now advance time, do this only once
             ds3231_settime(3,0,dt.sec); // Set time to 3:00, leave secs the same
             advance_time = 2;
             dst_active   = true;
             eep_write8(EEP_DST_ACTIVE,0x01); // set DST in eeprom
             break;
        case 2: 
             if (dt.min > 0) advance_time = 0; // At 3:01:00 back to normal
             dst_active = true;
        break;
        } // switch
    } // if
    else if (dt.mon == 10)
    {
        day    = ds3231_calc_dow(31,10,dt.year); // Find day-of-week for October 31th
        lsun10 = 31 - (day % 7);                 // Find last Sunday in October
        switch (revert_time)
        {
        case 0: if ((dt.day == lsun10) && (dt.hour == 3) && (dt.min == 0))
        {   // At 3:00 AM revert time back to 2 AM, check for one minute
            revert_time = 1;
        } // if
        else if (dt.day > lsun10) dst_active = false;
        else if (dt.day < lsun10) dst_active = true;
        else if (dt.hour < 3)     dst_active = true;
        break;
        case 1: // Now revert time, do this only once
            ds3231_settime(2,0,dt.sec); // Set time back to 2:00, leave secs the same
            revert_time = 2;
            dst_active  = false;
            eep_write8(EEP_DST_ACTIVE,0x00); // reset DST in eeprom
            break;
        case 2: // make sure we passed 3 AM in order to prevent multiple reverts
            if (dt.hour > 3) revert_time = 0; // at 4:00:00 back to normal
            dst_active = false;
            break;
        } // switch
    } // else if
    else if ((dt.mon < 3) || (dt.mon > 10)) dst_active = false;
    else                                    dst_active = true;

    //------------------------------------------------------------------------
    // If, for some reason, the clock was powered-off during the change to
    // summer- or winter-time, the eeprom value differs from the actual 
    // dst_active value. If so, set the actual sommer- and winter-time.
    //------------------------------------------------------------------------
    dst_eep = eep_read8(EEP_DST_ACTIVE);
    if (dst_active && !dst_eep)
    {   // It is summer-time, but clock has not been advanced yet
        hr = (dt.hour >= 23) ? 0 : dt.hour + 1;
        ds3231_settime(hr,dt.min,dt.sec); // Set summer-time to 1 hour later
        eep_write8(EEP_DST_ACTIVE,0x01); // set DST in eeprom
    } // if
    else if (!dst_active && dst_eep)
    {   // It is winter-time, but clock has not been moved back yet
        hr = (dt.hour > 0) ? dt.hour - 1 : 23;
        ds3231_settime(hr,dt.min,dt.sec); // Set summer-time to 1 hour earlier
        eep_write8(EEP_DST_ACTIVE,0x00); // set DST in eeprom
    } // if
} // check_and_set_summertime()

/*-----------------------------------------------------------------------------
  Purpose  : This routine reads the date and time info from the DS3231 RTC and
             stores this info into the global variables seconds, minutes and
             hours.
  Variables: 
    seconds: global variable [0..59]
    minutes: global variable [0..59]
      hours: global variable [0..23]
  Returns  : -
  ---------------------------------------------------------------------------*/
void clock_task(void)
{
    static bool one = true;
    
    char s[25];
    
    ds3231_gettime(&dt);
    check_and_set_summertime();
    sprintf(lk2,"%s %d %s %d %02d:%02d:%02d %s ",dows[dt.dow&0x07],
               dt.day , months[dt.mon], dt.year,
               dt.hour, dt.min, dt.sec, dst_active ? "Zomertijd" : "Wintertijd");
    uint16_t temp = ds3231_gettemp();
    sprintf(s,"%d.",temp>>2);
    strcat(lk2,s);
    sprintf(s,"%d Graden Celsius  ",25*(temp & 0x03));
    strcat(lk2,s);
    if (one)
    {
        one = false;
        lk_status |= LK2;
        color_text_input(lk2,lk2c);
    } // if
} // clock_task()

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
    clk = initialise_system_clock(HSE); // Set system-clock to 24 MHz
    uart1_init(clk);                    // UART1 init. to 115200,8,N,1
    uart3_init(clk);                    // UART3 init. to 115200,8,N,1
    setup_timers(clk,FREQ_4KHZ);        // Set Timer 2 for interrupt frequency
    setup_gpio_ports();                 // Init. needed output-ports
    i2c_init_bb(I2C_CH0);               // Init. I2C bus 0 for bit-banging
    dip_sw = read_dip_switches();       // Read dip-switches
    
    // Initialize all the tasks for the RGB Platform
    scheduler_init(); // init. task-scheduler
    switch (dip_sw)
    {
        case 1 : add_task(tetrisMain    , "tetris", 150,  50); break; // Tetris game
        case 15: add_task(test_playfield, "test"  , 175,2000); break; // Test
       default : add_task(lichtkrant    , "lkrant", 100,  50);        // Lichtkrant
                 add_task(clock_task    , "rtc"   ,  75,1000); break; // read date & time from DS3231
                 break;
    } // switch
    
    __enable_interrupt(); // set global interrupt enable, start task-scheduler
	
    print_revision_nr();  // print revision nr to UART 1
    sprintf(s,"CLK: 0x%X ",clk);
    uart1_printf(s);
    if      (clk == HSI) uart1_printf("HSI\n");
    else if (clk == LSI) uart1_printf("LSI\n");
    else if (clk == HSE) uart1_printf("HSE\n");
    sprintf(s,"DIP-SW: 0x%X\n",dip_sw);
    uart1_printf(s); // print status of dip-switches
    set_buzzer(FREQ_4KHZ,1);
    eep_read_string(EEP_TEXT1,lk1);         // read top-row of lichtkrant
    eep_read_string(EEP_COL1,(char*)lk1c);  // read colors of top-row
    eep_read_string(EEP_TEXT2,lk2);         // read bottom-row of lichtkrant
    eep_read_string(EEP_COL2,(char *)lk2c); // read colors of bottom-row
    
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
    } // while()
} // main()