#ifndef _STM8_HW_INIT_H
#define _STM8_HW_INIT_H
/*===================================================================================
  File Name: stm8_hw_init.h
  Author   : Emile
  ----------------------------------------------------------------------------------
  Purpose  : this is the header file for stm8_hw_init.c.
             It is based on RGB_platform_v03 PCB.
  ----------------------------------------------------------------------------------
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
  ----------------------------------------------------------------------------------
           Schematic of the connections to the STM8S207R8 MCU.
                                
      MCU pin-name            Function    |    MCU pin-name        Function
   ---------------------------------------|-----------------------------------------
   01 NRST                    <NRST>      | 64 PD7/TLI                -
   02 PA1/OSCIN               XTAL 24 MHz | 63 PD6/UART3_RX           RX2
   03 PA2/OSCOUT              XTAL 24 MHz | 62 PD5/UART3_TX           TX2
   04 VSSIO_1                 GND         | 61 PD4(HS)/TIM2_CH1[BEEP] -
   05 VSS                     GND         | 60 PD3(HS)/TIM2_CH2       -
   06 VCAP                    <VCAP>      | 59 PD2(HS)/TIM3_CH1       -
   07 VDD                     +5V         | 58 PD1(HS)/SWIM           SWIM
   08 VDDIO_1                 +5V         | 57 PD0(HS)/TIM3_CH2       -
   09 PA3/TIM2_CH3[TIME3_CH1] -           | 56 PE0(HS)/CLK_CCO        -
   10 PA4(HS)/UART1_RX        RX1         | 55 PE1/I2C_SCL            SCL0
   11 PA5(HS)/UART1_TX        TX1         | 54 PE2/I2C_SDA            SDA0
   12 PA6(HS)/UART1_CK        -           | 53 PE3/TIM1_BKIN          SCL1 (not used)
   13 PF7/AIN15               UP          | 52 PE4                    SDA1 (not used)
   14 PF6/AIN14               DOWN        | 51 PG7                    -
   15 PF5/AIN13               LEFT        | 50 PG6                    IRQ_LED
   16 PF4/AIN12               RIGHT       | 49 PG5                    BG_LED 
   ---------------------------------------|-----------------------------------------
   17 PF3/AIN11               OK          | 48 PI0                    -
   18 VREF+                   +5V filt.   | 47 PG4                    -
   19 VDDA                    +5V         | 46 PG3                    -
   20 VSSA                    GND         | 45 PG2                    -
   21 VREF-                   GND         | 44 PG1/CAN_RX             -
   22 PF0/AIN10               -           | 43 PG0/CAN_TX             -
   23 PB7/AIN7                ROWENA      | 42 PC7(HS)/SPI_MISO       -
   24 PB6/AIN6                PCB2        | 41 PC6(HS)/SPI_MOSI       -
   25 PB5/AIN5                PCB1        | 40 VDDIO_2                +5V
   26 PB4/AIN4                PCB0        | 39 VSSIO_2                GND
   27 PB3/AIN3                RSEL3       | 38 PC5(HS)/SPI_SCK        SHCP
   28 PB2/AIN2                RSEL2       | 37 PC4(HS)/TIM1_CH4       STCP
   29 PB1/AIN1                RSEL1       | 36 PC3(HS)/TIM1_CH3       SDIN_R
   30 PB0/AIN0                RSEL0       | 35 PC2(HS)/TIM1_CH2       SDIN_G
   31 PE7/AIN8                -           | 34 PC1(HS)/TIM1_CH1       SDIN_B
   32 PE6/AIN9            SDA2 (not used) | 33 PE5/SPI_NSS            SCL2 (not used)
   --------------------------------------------------------------------------------
   NOTE  : PORTF and PORTG pins do NOT have interrupt capability!
=================================================================================== */
#include <iostm8s207r8.h>
#include <stdint.h>
#include <stdbool.h>
#include <intrinsics.h> 

//---------------------------------------------------------------
// SIZE_X: the number of LEDs in the X-direction (left to right)
// SIZE_Y: the number of LEDs in the Y-direction (bottom to top)
//---------------------------------------------------------------
#define NR_OF_BOARDS (2)  /* The number of new PCBs connected in series */
#define SIZE_X       (16)
#define SIZE_Y       (16) /* New PCB is 16 rows, old PCB was 32 rows */

#define MAX_Y        (NR_OF_BOARDS * SIZE_Y)
#define MAX_CHAR_Y   (MAX_Y>>3)

//-----------------------------
// PORT A defines
//-----------------------------
//  RX1 & TX1 are initialized by the UART module

//-----------------------------
// PORT B defines
//-----------------------------
#define ROWENA      (0x80)  /* PB7 */
#define PCB2        (0x40)  /* PB6 */
#define PCB1        (0x20)  /* PB5 */
#define PCB0        (0x10)  /* PB4 */
#define RSEL3       (0x08)  /* PB3 */
#define RSEL2       (0x04)  /* PB2 */
#define RSEL1       (0x02)  /* PB1 */
#define RSEL0       (0x01)  /* PB0 */

#define PCBSEL      (PCB2 | PCB1 | PCB0)
#define ROWSEL      (RSEL3 | RSEL2 | RSEL1 | RSEL0)

#define ROWENAb     (PC_ODR_ODR7)

//-----------------------------
// PORT C defines
//-----------------------------
#define SHCP        (0x20) /* PC5 */
#define STCP        (0x10) /* PC4 */
#define SDIN_R      (0x08) /* PC3 */
#define SDIN_G      (0x04) /* PC2 */       
#define SDIN_B      (0x02) /* PC1 */       

// use these defines to directly control the output-pins
#define SHCPb       (PC_ODR_ODR5) /* clock for HC595 shift-registers */
#define STCPb       (PC_ODR_ODR4) /* clock for HC595 output-registers */
#define SDIN_Rb     (PC_ODR_ODR3) /* Serial Data In Red color */
#define SDIN_Gb     (PC_ODR_ODR2) /* Serial Data In Green color */ 
#define SDIN_Bb     (PC_ODR_ODR1) /* Serial Data In Blue color */
      
//-----------------------------
// PORT D defines
//-----------------------------
// RX2 & TX2 are initialized by the UART module
// SWIM is initialized by the JTAG module

//-----------------------------
// PORT E defines
//-----------------------------
#define SDA2        (0x40) /* PE6 SDA2, not used */
#define SCL2        (0x20) /* PE5 SCL2, not used */
#define SDA1        (0x10) /* PE4 SDA1, not used */
#define SCL1        (0x08) /* PE3 SCL1, not used */
#define SDA0        (0x04) /* PE2 SDA0 */
#define SCL0        (0x02) /* PE1 SCL0 */

//-----------------------------
// PORT F defines
//-----------------------------
#define STICK_UP    (0x80) /* PF7 */
#define STICK_DOWN  (0x40) /* PF6 */       
#define STICK_LEFT  (0x20) /* PF5 */       
#define STICK_RIGHT (0x10) /* PF4 */
#define STICK_OK    (0x08) /* PF3 */
#define STICK_ALL   (STICK_UP | STICK_DOWN | STICK_LEFT | STICK_RIGHT | STICK_OK)
       
//-----------------------------
// PORT G defines
//-----------------------------
#define IRQ_LED     (0x40) /* PG6, shows interrupt status */       
#define BG_LED      (0x20) /* PG5, shows background process status */       

// use these defines to directly control the output-pins
#define IRQ_LEDb     (PG_ODR_ODR6)
#define BG_LEDb      (PG_ODR_ODR5)

//-----------------------------
// Buzer STD modes
//-----------------------------
#define BZ_OFF   (0)
#define BZ_ON    (1)
#define BZ_ON2   (2)
#define BZ_BURST (3)
#define BZ_SHORT (4)

#define FREQ_1KHZ (0)
#define FREQ_2KHZ (1)
#define FREQ_4KHZ (2)

//-----------------------------
// Defines for system clock
//-----------------------------
#define HSI (0xE1) /* internal 16 MHz oscillator */
#define LSI (0xD2) /* internal 128 kHz oscillator */
#define HSE (0xB4) /* external 24 MHz oscillator */

// Function prototypes
void     buzzer(void);
uint8_t  initialise_system_clock(uint8_t clk);
void     setup_timers(uint8_t clk);
void     setup_gpio_ports(void);
#endif // _STM8_HW_INIT_H