/*==================================================================
   File Name: i2c_bb.h
   Author   : Emile
  ------------------------------------------------------------------
  Purpose   : This is the header-file for i2c_bb.c
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
#ifndef _I2C_BB_H
#define _I2C_BB_H

#include "stm8_hw_init.h"
#include "delay.h"
      
//----------------------------
// I2C defines
//----------------------------
#define I2C_ACK     (0)
#define I2C_NACK    (1)
#define I2C_ERROR   (2)
#define I2C_WRITE   (0)
#define I2C_READ    (1)
#define I2C_RETRIES (3)

enum I2C_CH
{
    I2C_CH0 = 0, /* I2C channel 0, used for communication with DS3231 */
    I2C_CH1 = 1, /* I2C channel 1, not used */
    I2C_CH2 = 2  /* I2C channel 2, not used */
}; // enum

//-----------------------------------------------------------------
// The LM92 sign bit is normally bit 12. The value read from the
// LM92 is SHL3. Therefore the sign bit is at bit 15
// Same for the Full Scale value, normally 2^12 SHL3 = 2^15.
//-----------------------------------------------------------------
#define LM92_SIGNb     (0x8000)
#define LM92_FS        (32768)
#define LM92_ERR       (0x4000)

// List of possible LM92 addresses
#define LM92_0_BASE   (0x90)
#define LM92_1_BASE   (0x92)
#define LM92_2_BASE   (0x94)
#define LM92_3_BASE   (0x96)

#define DS2482_THLT_BASE  (0x30)
#define DS2482_TBOIL_BASE (0x32)
#define DS2482_TCFC_BASE  (0x34)
#define DS2482_TMLT_BASE  (0x36)

// DS2482 Configuration Register
// Standard speed (1WS==0), Strong Pullup disabled (SPU==0), Active Pullup enabled (APU==1)
#define DS2482_CONFIG         (0xE1)
#define DS2482_OW_POLL_LIMIT  (200)

// DS2482 commands
#define CMD_DRST   0xF0
#define CMD_WCFG   0xD2
#define CMD_CHSL   0xC3
#define CMD_SRP    0xE1
#define CMD_1WRS   0xB4
#define CMD_1WWB   0xA5
#define CMD_1WRB   0x96
#define CMD_1WSB   0x87
#define CMD_1WT    0x78

// DS2482 status bits 
#define STATUS_1WB  0x01
#define STATUS_PPD  0x02
#define STATUS_SD   0x04
#define STATUS_LL   0x08
#define STATUS_RST  0x10
#define STATUS_SBR  0x20
#define STATUS_TSB  0x40
#define STATUS_DIR  0x80

// I2C channel 0 is used for all I2C communications
#define SCL0_in    (PE_DDR &= ~SCL0) 			  /* Set SCL to input */
#define SCL0_out   {PE_DDR |=  SCL0; PE_CR1 |=  SCL0;}    /* Set SCL to push-pull output */
#define SCL0_0     {PE_ODR &= ~SCL0; i2c_delay_5usec(1);} /* Set SCL to 0 */
#define SCL0_1     {PE_ODR |=  SCL0; i2c_delay_5usec(1);} /* Set SCL to 1 */
#define SCL0_rd    (PE_IDR &   SCL0) 			  /* Read from SCL */
#define SDA0_in    (PE_DDR &= ~SDA0) 			  /* Set SDA to input */
#define SDA0_out   {PE_DDR |=  SDA0; PE_CR1 |=  SDA0;}    /* Set SDA to push-pull output */
#define SDA0_0     (PE_ODR &= ~SDA0) 			  /* Set SDA to 0 */
#define SDA0_1     (PE_ODR |=  SDA0) 			  /* Set SDA to 1 */
#define SDA0_rd    (PE_IDR &   SDA0) 			  /* Read from SDA */

// I2C channel 1 is not used
#define SCL1_in    (PG_DDR &= ~SCL1) 			  /* Set SCL to input */
#define SCL1_out   {PG_DDR |=  SCL1; PG_CR1 |=  SCL1;}    /* Set SCL to push-pull output */
#define SCL1_0     {PG_ODR &= ~SCL1; i2c_delay_5usec(1);} /* Set SCL to 0 */
#define SCL1_1     {PG_ODR |=  SCL1; i2c_delay_5usec(1);} /* Set SCL to 1 */
#define SCL1_rd    (PG_IDR &   SCL1) 			  /* Read from SCL */
#define SDA1_in    (PG_DDR &= ~SDA1) 			  /* Set SDA to input */
#define SDA1_out   {PG_DDR |=  SDA1; PG_CR1 |=  SDA1;}    /* Set SDA to push-pull output */
#define SDA1_0     (PG_ODR &= ~SDA1) 			  /* Set SDA to 0 */
#define SDA1_1     (PG_ODR |=  SDA1) 			  /* Set SDA to 1 */
#define SDA1_rd    (PG_IDR &   SDA1) 			  /* Read from SDA */

// I2C channel 2 is not used
#define SCL2_in    (PG_DDR &= ~SCL2) 			  /* Set SCL to input */
#define SCL2_out   {PG_DDR |=  SCL2; PG_CR1 |=  SCL2;}    /* Set SCL to push-pull output */
#define SCL2_0     {PG_ODR &= ~SCL2; i2c_delay_5usec(1);} /* Set SCL to 0 */
#define SCL2_1     {PG_ODR |=  SCL2; i2c_delay_5usec(1);} /* Set SCL to 1 */
#define SCL2_rd    (PG_IDR &   SCL2) 			  /* Read from SCL */
#define SDA2_in    (PG_DDR &= ~SDA2) 			  /* Set SDA to input */
#define SDA2_out   {PG_DDR |=  SDA2; PG_CR1 |=  SDA2;}    /* Set SDA to push-pull output */
#define SDA2_0     (PG_ODR &= ~SDA2) 			  /* Set SDA to 0 */
#define SDA2_1     (PG_ODR |=  SDA2) 			  /* Set SDA to 1 */
#define SDA2_rd    (PG_IDR &   SDA2) 			  /* Read from SDA */

/*-----------------------------------------------------------------------------
  Purpose  : This function creates a 5 usec delay (approximately) without
             using a timer or an interrupt.
  Variables: --
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void i2c_delay_5usec(uint16_t x)
{
    uint16_t j;
    uint8_t  i;
      
    for (j = 0; j < x; j++)
    {
        for (i = 0; i < 120; i++) ; // 120 * 41.7 nsec (24 MHz) = 5 usec.
    } // for j
} // i2c_delay_5usec()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SCL line to input
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void scl_in(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SCL2_in; break;
        case I2C_CH1: SCL1_in; break;
        default     : SCL0_in; break;
    } // switch
} // scl_in()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SCL line to output
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void scl_out(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SCL2_out; break;
        case I2C_CH1: SCL1_out; break;
        default     : SCL0_out; break;
    } // switch
} // scl_out()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SCL line to 0
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void scl_0(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SCL2_0; break;
        case I2C_CH1: SCL1_0; break;
        default     : SCL0_0; break;
    } // switch
} // scl_0()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SCL line to 1
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void scl_1(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SCL2_1; break;
        case I2C_CH1: SCL1_1; break;
        default     : SCL0_1; break;
    } // switch
} // scl_1()

/*-----------------------------------------------------------------------------
  Purpose  : Reads from the SCL line
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : true,false
  ---------------------------------------------------------------------------*/
static inline uint8_t scl_read(enum I2C_CH ch)
{ 
    uint8_t ret;
    
    switch (ch)
    {
        case I2C_CH2: ret = SCL2_rd; break;
        case I2C_CH1: ret = SCL1_rd; break;
        default     : ret = SCL0_rd; break;
    } // switch
    return ret;
} // scl_read()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SDA line to input
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void sda_in(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SDA2_in; break;
        case I2C_CH1: SDA1_in; break;
        default     : SDA0_in; break;
    } // switch
} // sda_in()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SDA line to output
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void sda_out(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SDA2_out; break;
        case I2C_CH1: SDA1_out; break;
        default     : SDA0_out; break;
    } // switch
} // sda_out()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SDA line to 0
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void sda_0(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SDA2_0; break;
        case I2C_CH1: SDA1_0; break;
        default     : SDA0_0; break;
    } // switch
} // sda_0()

/*-----------------------------------------------------------------------------
  Purpose  : Sets the SDA line to 1
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : -
  ---------------------------------------------------------------------------*/
static inline void sda_1(enum I2C_CH ch)
{ 
    switch (ch)
    {
        case I2C_CH2: SDA2_1; break;
        case I2C_CH1: SDA1_1; break;
        default     : SDA0_1; break;
    } // switch
} // sda_1()

/*-----------------------------------------------------------------------------
  Purpose  : Reads from the SDA line
  Variables: ch: I2C channel number [I2C_CH0,I2C_CH1,I2C_CH2]
  Returns  : true,false
  ---------------------------------------------------------------------------*/
static inline uint8_t sda_read(enum I2C_CH ch)
{ 
    uint8_t ret;
    
    switch (ch)
    {
        case I2C_CH2: ret = SDA2_rd; break;
        case I2C_CH1: ret = SDA1_rd; break;
        default     : ret = SDA0_rd; break;
    } // switch
    return ret;
} // sda_read()

//----------------------------
// I2C-peripheral routines
//----------------------------
uint8_t i2c_reset_bus(enum I2C_CH ch);                  // Reset I2C-bus after a lock-up 
void    i2c_init_bb(enum I2C_CH ch);                    // Initializes the I2C Interface. Needs to be called only once
uint8_t i2c_start_bb(enum I2C_CH ch, uint8_t addr);     // Issues a start condition and sends address and transfer direction
uint8_t i2c_rep_start_bb(enum I2C_CH ch, uint8_t addr); // Issues a repeated start condition and sends address and transfer direction
void    i2c_stop_bb(enum I2C_CH ch);                    // Terminates the data transfer and releases the I2C bus
uint8_t i2c_write_bb(enum I2C_CH ch, uint8_t data);     // Send one byte to I2C device, returns ACK or NAK
uint8_t i2c_read_bb(enum I2C_CH ch, uint8_t ack);       // Read one byte from I2C device and calls i2c_stop_bb()

int16_t lm92_read(enum I2C_CH ch, uint8_t *err);
bool    ds2482_reset(enum I2C_CH ch, uint8_t addr);
bool    ds2482_write_config(enum I2C_CH ch, uint8_t addr);
bool    ds2482_detect(enum I2C_CH ch, uint8_t addr);
uint8_t ds2482_search_triplet(enum I2C_CH ch, uint8_t search_direction, uint8_t addr);

#endif
