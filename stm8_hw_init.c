/*==================================================================
  File Name: stm8_hw_init.c
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This files initializes all the hardware peripherals 
             of the STM8S207R8 uC.
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
  along with this software. If not, see <http://www.gnu.org/licenses/>.
  ================================================================== */ 
#include "stm8_hw_init.h"
#include "scheduler.h"
#include "pixel.h"

extern uint32_t t2_millis;       // needed for delay_msec()
extern playfield_color rgb_ledr; // The actual status of the red leds
extern playfield_color rgb_ledg; // The actual status of the green leds
extern playfield_color rgb_ledb; // The actual status of the blue leds
extern bool vsync;               // Vertical-Sync

uint8_t x; // debug
uint8_t current_row = 0;         // Index which row in the playfield is enabled

//------------------------------------------------
// Buzzer variables
//------------------------------------------------
bool     bz_on = false;    // true = buzzer is enabled
bool     bz_dbl;           // true = generate 2nd beep
uint8_t  bz_std = BZ_OFF;  // std number
uint8_t  bz_rpt;           // buzzer repeat counter
uint8_t  bz_rpt_max;       // number of beeps to make
uint16_t bz_tmr;           // buzzer msec counter
uint8_t  bz_freq = FREQ_1KHZ;

/*------------------------------------------------------------------
  Purpose  : This is the buzzer routine which runs every msec. 
             (f=1 kHz). It is used by 1 kHz interrupt routine.
  Variables: freq: [FREQ_1KHz, FREQ_2KHz, FREQ_4KHz]
  Returns  : -
  ------------------------------------------------------------------*/
void buzzer(void)
{
    switch (bz_std)
    {
        case BZ_OFF:   BEEP_CSR_BEEPEN  = 0;       //  Turn off the beep.
                       BEEP_CSR_BEEPSEL = bz_freq; //  0=1 kHz, 1=2 kHz, 2=4 kHz
                       BEEP_CSR_BEEPDIV = 14;      //  Set beep divider to 129/8 kHz
                       if (bz_on) 
                       {
                               bz_tmr = 0;
                               bz_rpt = 0;
                               bz_dbl = false;
                               bz_std = BZ_ON;
                       } // if
                       break;
        case BZ_ON:    BEEP_CSR_BEEPEN = 1;    //  Turn-on the beep.
                       if (++bz_tmr > 50) 
                       {
                               bz_tmr = 0;
                               if (!bz_dbl)
                                    bz_std = BZ_SHORT;
                               else bz_std = BZ_BURST;
                       } // if
                       break;
        case BZ_SHORT: BEEP_CSR_BEEPEN  = 0;   //  Turn off the beep.
                       bz_dbl = true;
                       if (++bz_tmr >= 50)
                       {
                               bz_tmr = 0;
                               bz_std = BZ_ON;
                       } // if
                       break;		
        case BZ_BURST: BEEP_CSR_BEEPEN  = 0;   //  Turn off the beep.
                       if (++bz_tmr > 500)
                       {
                             bz_tmr = 0;  
                             bz_dbl = false;
                             if (++bz_rpt >= bz_rpt_max) 
                             {
                                      bz_on  = false;
                                      bz_std = BZ_OFF;
                             } // if
                             else bz_std = BZ_ON;
                       } // if
                       break;					   						 
    } // switch
} // buzzer()

/*------------------------------------------------------------------
  Purpose  : This is the Timer-interrupt routine for the Timer 2 
             Overflow handler which runs every msec. (f=1 kHz). 
             It is used by the task-scheduler.
  Variables: -
  Returns  : -
  ------------------------------------------------------------------*/
#pragma vector = TIM2_OVR_UIF_vector
__interrupt void TIM2_UPD_OVF_IRQHandler(void)
{
    IRQ_LEDb = 1;      // Start Time-measurement
    PB_ODR   = 0x00;   // Disable playfield (ROWENA), all rows off (PCB2-0, RSEL3-0)
    t2_millis++;       // update millisecond counter
    scheduler_isr();   // call the ISR routine for the task-scheduler
    
    uint16_t colmask = 0x0001; // start with bit 0 to send to shift-register
    for (uint8_t i = 0; i < SIZE_X; i++)
    {   // shift color bits to hardware shift-registers
        SDIN_Rb   = (rgb_ledr[current_row] & colmask) ? 1 : 0;
        SDIN_Gb   = (rgb_ledg[current_row] & colmask) ? 1 : 0;
        SDIN_Bb   = (rgb_ledb[current_row] & colmask) ? 1 : 0;
        SHCPb     = 1; // set clock for shift-register;
        colmask <<= 1; // select next bit
        SHCPb     = 0; // set clock to 0 again
    } // for i
    // Now clock bits from shift-registers to output-registers
    STCPb = 1; // set clock to 1
    //---------------------------------------------------------------
    // Select the next row: this varies from 0 to MAX_Y
    //---------------------------------------------------------------
    PB_ODR = (current_row & 0x7F); // Set PCB nr and ROW nr in hardware
    STCPb  = 0; // set clock to 0 again
    if (++current_row >= MAX_Y)
    {   // cycle through rows, enable one at a time
        current_row = 0;
        vsync       = true; // set Vsync flag
    } // if
    ROWENAb      = 1; // Enable playfield again
    IRQ_LEDb     = 0; // Stop Time-measurement
    TIM2_SR1_UIF = 0; // Reset the interrupt otherwise it will fire again straight away.
} // TIM2_UPD_OVF_IRQHandler()

/*-----------------------------------------------------------------------------
  Purpose  : This routine initialises the system clock to run at 24 MHz.
             It uses the external HSE oscillator. 
  Variables: clk: which oscillator to use: HSI (0xE1), HSE (0xB4) or LSI (0xD2)
  Returns  : which oscillator is active: HSI (0xE1), HSE (0xB4) or LSI (0xD2)
  ---------------------------------------------------------------------------*/
uint8_t initialise_system_clock(uint8_t clk)
{
    CLK_ECKR       = 0;           //  Reset the External Clock Register.
    CLK_ECKR_HSEEN = 1;           //  Enable the HSE.
    while (CLK_ECKR_HSERDY == 0); //  Wait for the HSE to be ready for use.
    CLK_CKDIVR     = 0;           //  Ensure the clocks are running at full speed.
 
    // The datasheet lists that the max. ADC clock is equal to 6 MHz (4 MHz when on 3.3V).
    // Because fMASTER is now at 24 MHz, we need to set the ADC-prescaler to 6.
    ADC_CR1_SPSEL  = 0x03;        //  Set prescaler to 6, fADC = 4 MHz
    CLK_SWIMCCR    = 0;           //  Set SWIM to run at clock / 2.
    while (CLK_SWCR_SWBSY != 0);  //  Pause while the clock switch is busy.

    //  Enable switching for 24 MHz
    if (clk == HSE) CLK_SWCR_SWEN  = 1;
    CLK_SWR        = 0xB4;        //  Use HSE as the clock source.
    while (CLK_SWCR_SWIF == 0);   //  Target clock source not ready  
    CLK_SWCR       = 0;           //  Reset the clock switch control register.
    CLK_SWCR_SWEN  = 1;           //  Enable switching.
    while (CLK_SWCR_SWBSY != 0);  //  Pause while the clock switch is busy.
    return CLK_CMSR;              //  Return which oscillator is active
} // initialise_system_clock()

/*-----------------------------------------------------------------------------
  Purpose  : This routine initialises Timer 2 to generate a 1 kHz interrupt.
             16 MHz: 16 MHz / (  16 *  1000) = 1000 Hz (1000 = 0x03E8)
             24 MHz: 24 MHz / (  16 *  1500) = 1000 Hz (1500 = 0x05DC)
  Variables: -
  Returns  : -
  ---------------------------------------------------------------------------*/
void setup_timers(uint8_t clk)
{
    // Set Timer 2 for an interrupt frequency of 1 kHz
    // 16 MHz: TIM2_ARRH=0x03, TIM2_ARRL=0xE8 (0x03E8 = 1000)
    // 24 MHz: TIM2_ARRH=0x05, TIM2_ARRL=0xDC (0x05DC = 1500)
    TIM2_PSCR    = 0x04;  //  Prescaler = 16
    if (clk == HSE)
    {   // external HSE oscillator
        TIM2_ARRH    = 0x05;  //  High byte of 1500 for 24 MHz
        TIM2_ARRL    = 0xDC;  //  Low  byte of 1500 for 24 MHz
    } // if
    else
    {   // internal HSI oscillator
        TIM2_ARRH    = 0x03;  //  High byte of 1000 for 16 MHz
        TIM2_ARRL    = 0xE8;  //  Low  byte of 1000 for 16 MHz
    } // else
    TIM2_IER_UIE = 1;     //  Enable the update interrupts
    TIM2_CR1_CEN = 1;     //  Finally enable the timer
} // setup_timers()

/*-----------------------------------------------------------------------------
  Purpose  : This routine initialises all the GPIO pins of the STM8 uC.
  Variables: -
  Returns  : -
  ---------------------------------------------------------------------------*/
void setup_gpio_ports(void)
{
    //-----------------------------
    // PORT A defines
    //-----------------------------
    //  RX1 & TX1 are initialized by the UART module
    
    //-----------------------------
    // PORT B defines
    //-----------------------------
    PB_DDR     |=   ROWENA | PCBSEL | ROWSEL;   // Set as output
    PB_CR1     |=   ROWENA | PCBSEL | ROWSEL;   // Set to push-pull
    PB_ODR     &=  ~(ROWENA | PCBSEL | ROWSEL); // All outputs 0 at power-up

    //-----------------------------
    // PORT C defines
    //-----------------------------
    PC_DDR     |=  (SHCP | STCP | SDIN_R | SDIN_G | SDIN_B); // Set as outputs
    PC_CR1     |=  (SHCP | STCP | SDIN_R | SDIN_G | SDIN_B); // Set to Push-Pull
    PC_ODR     &= ~(SHCP | STCP | SDIN_R | SDIN_G | SDIN_B); // Outputs are OFF

    //-----------------------------
    // PORT D defines
    //-----------------------------
    // RX2 & TX2 are initialized by the UART module
    // SWIM is initialized by the JTAG module
    
    //-----------------------------
    // PORT E defines
    //-----------------------------
    PE_ODR     |=  (SCL0 | SDA0); // Must be set here, or I2C will not work
    PE_DDR     |=  (SCL0 | SDA0); // Set as outputs
    PE_CR1     |=  (SCL0 | SDA0); // Set to push-pull
    PE_DDR     &=  ~(SW3 | SW2 | SW1 | SW0); // Set as inputs
    PE_CR1     |=   (SW3 | SW2 | SW1 | SW0); // Enable pull-up resistors
    
    //-----------------------------
    // PORT F defines
    //-----------------------------
    PF_DDR     &=  ~STICK_ALL; // Set as inputs
    PF_CR1     |=   STICK_ALL; // Enable pull-up resistors
    
    //-----------------------------
    // PORT G defines
    //-----------------------------
    PG_DDR     |=  (IRQ_LED | BG_LED); // Set as outputs
    PG_CR1     |=  (IRQ_LED | BG_LED); // Set to push-pull
    PG_ODR     &= ~(IRQ_LED | BG_LED); // disable leds at power-up
} // setup_gpio_ports()
