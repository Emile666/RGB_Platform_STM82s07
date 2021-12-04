#ifndef _STM8_EEP_H
#define _STM8_EEP_H
/*==================================================================
  File Name: eep.h
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This is the header-file for eep.c
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
#include "stm8_hw_init.h"

// EEPROM base address within STM8 uC
#define EEP_BASE_ADDR (0x4000)

#define EEP_TEXT1          (0x0000) /* Text-string top-row */
#define EEP_TEXT2          (0x0080) /* Text-string bottom-row */
#define EEP_COL1           (0x0100) /* Colors for text-string top-row */
#define EEP_COL2           (0x0180) /* Colors for text-string bottom-row */

#define NO_INIT            (0xFF)
#define USE_ETH            (0x00)
#define USE_USB            (0xFF)

// Function prototypes
uint8_t  eep_read8(uint8_t eep_address);
uint16_t eep_read16(uint8_t eep_address);
void     eep_write8(uint8_t eep_address, uint8_t data);
void     eep_write16(uint8_t eep_address, uint16_t data);
void     eep_write_string(uint16_t eep_address,char *s);
void     eep_read_string(uint16_t eep_address,char *s);

#endif
