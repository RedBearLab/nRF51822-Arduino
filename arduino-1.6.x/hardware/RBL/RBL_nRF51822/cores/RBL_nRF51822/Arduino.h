

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/pgmspace.h>

#include "mbed.h"
#include "BLE_API.h"
#include "nRF51822_API.h"

#include "wiring_constants.h"
#include "wiring_digital.h"
#include "wiring_delay.h"

#include "wiring_buffer.h"
#include "wiring_serial.h"

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

extern UARTClass Serial1;

#endif

