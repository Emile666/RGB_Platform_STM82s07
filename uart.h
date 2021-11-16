#ifndef _STM8_UART_H
#define _STM8_UART_H
/*==================================================================
  File Name: uart.h
  Author   : Emile
  ------------------------------------------------------------------
  Purpose  : This is the header-file for uart.c
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

#define UART_BUFLEN (10)
#define TX_BUF_SIZE (30)
#define RX_BUF_SIZE (10)

// UART1: Used for general communication
void    uart1_init(uint8_t clk);
void    uart1_printf(char *s);
bool    uart1_kbhit(void);
uint8_t uart1_getc(void);
void    uart1_putc(uint8_t ch);

// UART3: Used for ESP8266 communication
void    uart3_init(uint8_t clk);
void    uart3_putc(uint8_t ch);
bool    uart3_kbhit(void);
uint8_t uart3_getc(void);
void    uart3_putc(uint8_t ch);
#endif