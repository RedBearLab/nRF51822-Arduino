/*
  Copyright (c) 2012 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "binary.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

// Includes Atmel CMSIS

#include "nrf.h"
#include "startup_nrf51822.h"
#include "nrf_gpiote.h"
#include "nrf_gpio.h"
#include "wiring_constants.h"
#include "app_timer.h"
#include "app_error.h"

#define countof(x) (sizeof(x)/sizeof(x[0]))

#define VARIANT_MCK         16000000UL

#define clockCyclesPerMicrosecond() ( VARIANT_MCK / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

//PIN Define
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8
#define D9 9
#define D10 10
#define D11 11
#define D12 12
#define D13 13
#define D14 14
#define D15 15
#define D16 16
#define D17 17
#define D18 18
#define D19 19
#define D20 20
#define D21 21
#define D22 22
#define D23 23
#define D24 24

#define CS   D7
#define SCK  D16
#define MISO D17
#define MOSI D18

#define SDA0  D2
#define SCL0  D3

#define SDA1  D14
#define SCL1  D15

#define A0 D19
#define A1 D20
#define A2 D21
#define A3 D22
#define A4 D23
#define A5 D24

#define DEFAULT_RX_PIN D11
#define DERAULT_TX_PIN D9

#define TWI_SDA D14
#define TWI_SCL D15


#define TIMER_PRESCALE


void yield(void);

#include "delay.h"
#include "wiring_digital.h"
#include "wiring_analog.h"
#include "wiring_shift.h"
//#include "uart.h"

#include "pin_transform.h"

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

#ifdef __cplusplus
} // extern "C"


#include "WString.h"
#include "Tone.h"
#include "WMath.h"
#include "HardwareSerial.h"
#include "wiring_pulse.h"
#include "interrupt.h"
#include "wuart.h"

#endif // __cplusplus




#endif // Arduino_h
