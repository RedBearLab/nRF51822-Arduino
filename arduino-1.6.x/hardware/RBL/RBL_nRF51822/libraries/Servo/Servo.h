/*
  Servo.h - Interrupt driven Servo library for Arduino using 16 bit timers- Version 2
  Copyright (c) 2009 Michael Margolis.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* 
  
  A servo is activated by creating an instance of the Servo class passing the desired pin to the attach() method.
  The servos are pulsed in the background using the value most recently written using the write() method

  Note that analogWrite of PWM on pins associated with the timer are disabled when the first servo is attached.
  Timers are seized as needed in groups of 12 servos - 24 servos use two timers, 48 servos will use four.
  The sequence used to sieze timers is defined in timers.h

  The methods are:

   Servo - Class for manipulating servo motors connected to Arduino pins.

   attach(pin )  - Attaches a servo motor to an i/o pin.
   attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
   default min is 544, max is 2400  
 
   write()     - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
   writeMicroseconds() - Sets the servo pulse width in microseconds 
   read()      - Gets the last written servo pulse width as an angle between 0 and 180. 
   readMicroseconds()   - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
   attached()  - Returns true if there is a servo attached. 
   detach()    - Stops an attached servos from pulsing its i/o pin. 
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <inttypes.h>
#include "Arduino.h"

//you could set how many timers can be used, which timer start.
//for example, now you have three timers to be used, first use timer2.
//typedef enum { _timer2, _timer1, _timer0, _Nbr_16timers } timer16_Sequence_t;
typedef enum { _timer1, _Nbr_16timers } timer16_Sequence_t;


#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo  544
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo   2400
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL    20000     // minumim time to refresh servos in microseconds 

#define SERVOS_PER_TIMER       10     // the maximum number of servos controlled by one timer 
#define MAX_SERVOS   		   (_Nbr_16timers  * SERVOS_PER_TIMER)	//the maximum servos

#define INVALID_SERVO         255     // flag indicating an invalid servo index


typedef struct  {
  PinName nbr        :6 ;             // a pin number from 0 to 31
  uint8_t isActive   :1 ;             // true if this channel is enabled, pin not pulsed if false 
} ServoPin_t   ;  

typedef struct {
  ServoPin_t Pin;
  uint16_t ticks;
} servo_t;



class Servo
{
	private:
	
		uint8_t  servoIndex;
		uint16_t min_ticks;
		uint16_t max_ticks;
		
	public:
		Servo();
		uint8_t attach(uint32_t pin);
		uint8_t attach(uint32_t pin, uint16_t min, uint16_t max);
		void detach();
		
		void write(uint16_t value);
		void writeMicroseconds(uint16_t value);
		
		uint16_t read();
		uint16_t readMicroseconds();
		
		bool attached();  	// return true if this servo is attached, otherwise false
};



#endif



