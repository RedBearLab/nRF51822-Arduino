

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/pgmspace.h>

#include "mbed.h"

#include "wiring_constants.h"
#include "wiring_digital.h"
#include "wiring_delay.h"

#include "wiring_buffer.h"
#include "wiring_serial.h"

#include "wiring_analog.h"
#include "WMath.h"

#include "wiring_shift_pulseIn.h"
#include "wiring_interrupter.h"

#include "pins_arduino.h"
#include "pin_transform.h"

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

extern UARTClass Serial;

#endif

