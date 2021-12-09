/*==================================================================
  File Name: eep.c
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This files contains the EEPROM related functions 
             for the STM8 uC.
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
#include "eep.h"

/*-----------------------------------------------------------------------------
  Purpose  : This function reads a (8-bit) value from the STM8 EEPROM.
  Variables: eep_address: the index number within the EEPROM. An index number
                          is the n-th 16-bit variable within the EEPROM.
  Returns  : the (8-bit value)
  ---------------------------------------------------------------------------*/
uint8_t eep_read8(uint16_t eep_address)
{
    uint8_t data;
    
    char    *address = (char *)EEP_BASE_ADDR; // EEPROM base address.
    address += eep_address;                   // add offset
    data     = *address;                      // read byte
    return data;
} // eep_read8()

/*-----------------------------------------------------------------------------
  Purpose  : This function reads a (16-bit) value from the STM8 EEPROM.
  Variables: eep_address: the index number within the EEPROM. An index number
                          is the n-th 16-bit variable within the EEPROM.
  Returns  : the (16-bit value)
  ---------------------------------------------------------------------------*/
uint16_t eep_read16(uint16_t eep_address)
{
    uint16_t data;
    
    char    *address = (char *)EEP_BASE_ADDR; // EEPROM base address.
    address += eep_address;                   // add offset
    data     = *address++;                    // read MSB first
    data   <<= 8;                             // SHL 8
    data    |= *address;                      // read LSB
    return data;                              // Return result
} // eep_read16()

/*-----------------------------------------------------------------------------
  Purpose  : This function writes a (8-bit) value to the STM8 EEPROM.
  Variables: eep_address: the index number within the EEPROM. An index number
                          is the n-th 16-bit variable within the EEPROM.
             data       : 8-bit value to write to the EEPROM
  Returns  : -
  ---------------------------------------------------------------------------*/
void eep_write8(uint16_t eep_address,uint8_t data)
{
    char *address = (char *)EEP_BASE_ADDR; //  EEPROM base address.

    // Avoid unnecessary EEPROM writes
    if (data == eep_read8(eep_address)) return;

    address += eep_address; // add offset
    FLASH_DUKR = 0xAE;      // unlock EEPROM
    FLASH_DUKR = 0x56;
    while (!FLASH_IAPSR_DUL) ; // wait until EEPROM is unlocked
    *address   = (uint8_t)(data & 0xFF); // write it
    FLASH_IAPSR_DUL = 0;    // write-protect EEPROM again
} // eep_write8()

/*-----------------------------------------------------------------------------
  Purpose  : This function writes a (16-bit) value to the STM8 EEPROM.
  Variables: eep_address: the index number within the EEPROM. An index number
                          is the n-th 16-bit variable within the EEPROM.
             data       : 16-bit value to write to the EEPROM
  Returns  : -
  ---------------------------------------------------------------------------*/
void eep_write16(uint16_t eep_address,uint16_t data)
{
    char *address = (char *)EEP_BASE_ADDR; //  EEPROM base address.

    // Avoid unnecessary EEPROM writes
    if (data == eep_read16(eep_address)) return;

    address += eep_address; // add offset
    FLASH_DUKR = 0xAE;      // unlock EEPROM
    FLASH_DUKR = 0x56;
    while (!FLASH_IAPSR_DUL) ; // wait until EEPROM is unlocked
    *address++ = (uint8_t)((data >> 8) & 0xFF); // write MSB
    *address   = (uint8_t)(data & 0xFF);        // write LSB
    FLASH_IAPSR_DUL = 0;    // write-protect EEPROM again
} // eep_write16()

/*-----------------------------------------------------------------------------
  Purpose  : This function writes a string to the STM8 EEPROM.
  Variables: eep_address: the index number within the EEPROM.
             s          : string to write to eeprom
  Returns  : -
  ---------------------------------------------------------------------------*/
void eep_write_string(uint16_t eep_address, char *s)
{
    uint8_t i = 0;
    char *address = (char *)EEP_BASE_ADDR; //  EEPROM base address.

    address += eep_address; // add offset
    FLASH_DUKR = 0xAE;      // unlock EEPROM
    FLASH_DUKR = 0x56;
    while (!FLASH_IAPSR_DUL) ; // wait until EEPROM is unlocked
    while ((s[i] != '\0') && (i < 0x80))
    {
        *address++ = (uint8_t)(s[i++]); // write it
    } // while
    *address = '\0';        // terminate string in eeprom
    FLASH_IAPSR_DUL = 0;    // write-protect EEPROM again
} // eep_write_string()

/*-----------------------------------------------------------------------------
  Purpose  : This function reads a string from the STM8 EEPROM.
  Variables: eep_address: the index number within the EEPROM.
             s          : string read from EEPROM
  Returns  : -
  ---------------------------------------------------------------------------*/
void eep_read_string(uint16_t eep_address, char *s)
{
    uint8_t i = 0;
    char *address = (char *)EEP_BASE_ADDR; //  EEPROM base address.

    address += eep_address; // add offset
    while (((s[i] = *address++) != '\0') && (i < 0x80)) i++;
} // eep_read_string()


