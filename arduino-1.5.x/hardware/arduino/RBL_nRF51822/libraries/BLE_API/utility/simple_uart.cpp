/* Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdint.h>

#include "nrf.h"
#include "simple_uart.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

#define UART_DEBUG 0

uint8_t simple_uart_get(void)
{
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    // Wait for RXD data to be received
  }
  
  NRF_UART0->EVENTS_RXDRDY = 0;
  return (uint8_t)NRF_UART0->RXD;
}

bool simple_uart_get_with_timeout(int32_t timeout_ms, uint8_t *rx_data)
{
  bool ret = true;
  
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    if (timeout_ms-- >= 0)
    {
      // wait in 1ms chunk before checking for status
      nrf_delay_us(1000);
    }
    else
    {
      ret = false;
      break;
    }
  }  // Wait for RXD data to be received

  if (timeout_ms >= 0)
  {
    // clear the event and set rx_data with received byte
      NRF_UART0->EVENTS_RXDRDY = 0;
      *rx_data = (uint8_t)NRF_UART0->RXD;
  }

  return ret;
}

void simple_uart_put(uint8_t cr)
{
  NRF_UART0->TXD = (uint8_t)cr;

  while (NRF_UART0->EVENTS_TXDRDY!=1)
  {
    // Wait for TXD data to be sent
  }

  NRF_UART0->EVENTS_TXDRDY=0;
}

void simple_uart_putstring(char *str)
{
#if UART_DEBUG
  uint_fast8_t i = 0;
  uint8_t ch = str[i++];
  while (ch != '\0')
  {
    simple_uart_put(ch);
    ch = str[i++];
  }
#endif
}

void simple_uart_config(  uint8_t rts_pin_number,
                          uint8_t txd_pin_number,
                          uint8_t cts_pin_number,
                          uint8_t rxd_pin_number,
                          bool    hwfc)
{
/** @snippet [Configure UART RX and TX pin] */
  nrf_gpio_cfg_output(txd_pin_number);
  nrf_gpio_cfg_input(rxd_pin_number, NRF_GPIO_PIN_NOPULL);  

  NRF_UART0->PSELTXD = txd_pin_number;
  NRF_UART0->PSELRXD = rxd_pin_number;
/** @snippet [Configure UART RX and TX pin] */
  if (hwfc)
  {
    nrf_gpio_cfg_output(rts_pin_number);
    nrf_gpio_cfg_input(cts_pin_number, NRF_GPIO_PIN_NOPULL);
    NRF_UART0->PSELCTS = cts_pin_number;
    NRF_UART0->PSELRTS = rts_pin_number;
    NRF_UART0->CONFIG  = (UART_CONFIG_HWFC_Enabled << UART_CONFIG_HWFC_Pos);
  }

  NRF_UART0->BAUDRATE         = (UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos);
  NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
  NRF_UART0->TASKS_STARTTX    = 1;
  NRF_UART0->TASKS_STARTRX    = 1;
  NRF_UART0->EVENTS_RXDRDY    = 0;
}

void simple_uart_printHEX(uint32_t value)
{
#if UART_DEBUG
	const uint8_t ASCII[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	if(value <= 0xFF)
	{
		simple_uart_put('0');
		simple_uart_put('x');
		simple_uart_put(ASCII[value>>4]);
		simple_uart_put(ASCII[value & 0x0f]);
	}
	else if(value <= 0xFFFF)
	{
		simple_uart_put('0');
		simple_uart_put('x');
		simple_uart_put(ASCII[(value>>12) & 0xf]);
		simple_uart_put(ASCII[(value>>8) & 0xf]);
		simple_uart_put(ASCII[(value>>4 ) & 0xf]);
		simple_uart_put(ASCII[value & 0x0f]);
	}
	else if(value <= 0xFFFFFF)
	{
		simple_uart_put('0');
		simple_uart_put('x');
		simple_uart_put(ASCII[(value>>20) & 0xf]);
		simple_uart_put(ASCII[(value>>16) & 0xf]);
		simple_uart_put(ASCII[(value>>12) & 0xf]);
		simple_uart_put(ASCII[(value>>8 ) & 0xf]);
		simple_uart_put(ASCII[(value>>4 ) & 0xf]);
		simple_uart_put(ASCII[value & 0x0f]);
	}
	else if(value <= 0xFFFFFFFF)
	{
		simple_uart_put('0');
		simple_uart_put('x');
		simple_uart_put(ASCII[(value>>28) & 0xf]);
		simple_uart_put(ASCII[(value>>24) & 0xf]);
		simple_uart_put(ASCII[(value>>20) & 0xf]);
		simple_uart_put(ASCII[(value>>16) & 0xf]);
		simple_uart_put(ASCII[(value>>12) & 0xf]);
		simple_uart_put(ASCII[(value>>8 ) & 0xf]);
		simple_uart_put(ASCII[(value>>4 ) & 0xf]);
		simple_uart_put(ASCII[value & 0x0f]);
	}
	
	simple_uart_put('\r');
	simple_uart_put('\n');
#endif
}

void simple_uart_print(char *str, uint8_t newline)
{
#if UART_DEBUG
	simple_uart_putstring(str);
#endif	
}


// µÝ¹éµ÷ÓÃ just for simple_uart_printf
static void printn(unsigned int n, unsigned int b)
{
	static char *ntab = "0123456789ABCDEF";
	unsigned int a, m;
		
	if (n / b)
	{
		a = n / b;
		printn(a, b);  
	}
	m = n % b;
	simple_uart_put(ntab[m]);
}

void simple_uart_printf(char *fmt, ...)
{
#if UART_DEBUG
    char c;
    unsigned int *adx = (unsigned int*)(void*)&fmt + 1;
    
_loop:
    while((c = *fmt++) != '%')
	{
        if (c == '\0') return;
        simple_uart_put(c);
    }
    c = *fmt++;
    if (c == 'd' || c == 'l')
	{
        printn(*adx, 10);     //print HEX value
    }
    if (c == 'o')
	{
        printn(*adx, 8);
    }
	if(c == 'x')
	{
		simple_uart_put('0');
		simple_uart_put('x');	
		printn(*adx, 16);
	}
    if (c == 's')
	{
		simple_uart_putstring((char*)*adx);
    }
    if (c == 'c')
    {
        simple_uart_put(*adx);
    }
    adx++;
goto _loop;
	
#endif		
}

