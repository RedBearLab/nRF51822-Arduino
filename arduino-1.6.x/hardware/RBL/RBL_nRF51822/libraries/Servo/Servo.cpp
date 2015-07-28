/*
 Servo.cpp - Interrupt driven Servo library for Arduino using 16 bit timers- Version 2
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

#include "Servo.h"


/**********************************************************************
declare : 
**********************************************************************/

#define usToTicks(_us)    (( clockCyclesPerMicrosecond()* _us) / 8)     		  // converts microseconds to tick (assumes prescale of 8)  // 12 Aug 2009
#define ticksToUs(_ticks) (( (unsigned)_ticks * 8)/ clockCyclesPerMicrosecond() ) // converts from ticks back to microseconds

#define TRIM_DURATION       3                               // compensation ticks to trim adjust for digitalWrite delays // 12 August 2009

static servo_t servos[MAX_SERVOS];                          // static array of servo structures
static volatile int8_t Channel[_Nbr_16timers ];             // counter for the servo being pulsed for each timer (or -1 if refresh interval)

static uint8_t ServoCount = 0;                                     // the total number of attached servos

// convenience macros
#define SERVO_INDEX_TO_TIMER(_servo_nbr) 	((timer16_Sequence_t)(_servo_nbr / SERVOS_PER_TIMER))   // returns the timer controlling this servo
#define SERVO_INDEX_TO_CHANNEL(_servo_nbr)  (_servo_nbr % SERVOS_PER_TIMER)       					// returns the index of the servo on this timer
#define SERVO_INDEX(_timer,_channel)  		((_timer*SERVOS_PER_TIMER) + _channel)     				// macro to access servo index by timer and channel
#define SERVO(_timer,_channel)  			(servos[SERVO_INDEX(_timer,_channel)])            		// macro to access servo class by timer and channel

#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min_ticks * 4)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max_ticks * 4)  // maximum value in uS for this servo 


extern "C" 
{
void TIMER1_IRQHandler() 
{	
	//Clean interrupt flag
	NRF_TIMER1->EVENTS_COMPARE[0] = 0;
	if(Channel[_timer1] < 0)
	{	// channel set to -1 indicated that refresh interval completed so reset the timer
		NRF_TIMER1->TASKS_CLEAR = 1;
		NRF_TIMER1->CC[0] = 0;
	}
	else 
	{
		if(SERVO_INDEX(_timer1,Channel[_timer1]) < ServoCount && SERVO(_timer1,Channel[_timer1]).Pin.isActive == true)
		{
			NRF_GPIO->OUTCLR = (1 << (uint32_t)(SERVO(_timer1,Channel[_timer1]).Pin.nbr) );
		}
	}
	// increment to the next channel
	Channel[_timer1]++;
	if(SERVO_INDEX(_timer1,Channel[_timer1]) < ServoCount && Channel[_timer1] < SERVOS_PER_TIMER)
	{
		NRF_TIMER1->CC[0] += SERVO(_timer1,Channel[_timer1]).ticks;
		// check if activated, if active, pulse it high
		if(SERVO(_timer1,Channel[_timer1]).Pin.isActive == true)
		{
			NRF_GPIO->OUTSET = ( 1 << (uint32_t)(SERVO(_timer1,Channel[_timer1]).Pin.nbr) );
		}
	}
	else
	{	
		// allow a few ticks to ensure the next OCR1A not missed
		if( NRF_TIMER1->CC[0] +4 < usToTicks(REFRESH_INTERVAL) )
		{
			NRF_TIMER1->CC[0] = (unsigned int)usToTicks(REFRESH_INTERVAL);
		}
		else
		{
			NRF_TIMER1->CC[0] += 4;
		}
		Channel[_timer1] = -1;
	}
}
}

/**********************************************************************
name :
function : 
**********************************************************************/	
static void InitISR_handler()
{
	__disable_irq();
	
	NRF_TIMER1->TASKS_STOP = 1;
	NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
	NRF_TIMER1->PRESCALER = 3; 
	NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
	NRF_TIMER1->TASKS_CLEAR = 1;
	NRF_TIMER1->CC[0] = (uint32_t)(VARIANT_MCK/8/1000); // 1ms
	NRF_TIMER1->EVENTS_COMPARE[0] = 0;
	NRF_TIMER1->INTENSET = (TIMER_INTENSET_COMPARE0_Set << TIMER_INTENSET_COMPARE0_Pos);	

	NVIC_SetPriority(TIMER1_IRQn, 1);
	NVIC_EnableIRQ(TIMER1_IRQn);

	__enable_irq();
	NRF_TIMER1->TASKS_START = 1;
}
/**********************************************************************
name :
function : 
**********************************************************************/	
static void initISR(timer16_Sequence_t timer)
{
    if (timer == _timer1)
	{	
		InitISR_handler();
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/	
static void finISR(timer16_Sequence_t timer)
{
	if (timer == _timer1)
		NRF_TIMER1->TASKS_STOP = 1;
}
/**********************************************************************
name :
function : 
**********************************************************************/	
static boolean isTimerActive(timer16_Sequence_t timer)
{
	for(uint8_t channel=0; channel<SERVOS_PER_TIMER; channel++)
	{
		if(SERVO(timer, channel).Pin.isActive == true)
		{
			return true;
		}
	}
	return false;
}

/****************** end of static functions ******************************/

/**********************************************************************
name :
function : 
**********************************************************************/	
Servo::Servo()
{
	if(ServoCount < MAX_SERVOS)
	{	// assign a servo index to this instance
		this->servoIndex = ServoCount++;
		servos[this->servoIndex].ticks = usToTicks(DEFAULT_PULSE_WIDTH);
	}
	else
	{
		this->servoIndex = INVALID_SERVO;
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/	
uint8_t Servo::attach(uint32_t pin)
{
	return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t Servo::attach(uint32_t pin, uint16_t min, uint16_t max)
{
	PinName nrf_pin;
	timer16_Sequence_t timer;
	
	if(this->servoIndex < MAX_SERVOS)
	{	
		// set servo pin to output
		pinMode(pin, OUTPUT);     

		nrf_pin = Pin_nRF51822_to_Arduino(pin);
		
		servos[this->servoIndex].Pin.nbr = nrf_pin;
		//resolution of min/max is 4 uS
		this->min_ticks  = (MIN_PULSE_WIDTH - min)/4; 
		this->max_ticks  = (MAX_PULSE_WIDTH - max)/4;
		// initialize the timer if it has not already been initialized
		timer = SERVO_INDEX_TO_TIMER(servoIndex);
		if (isTimerActive(timer) == false) 
		{	
			initISR(timer);
		}
		// this must be set after the check for isTimerActive
		servos[this->servoIndex].Pin.isActive = true;  
	}
	return this->servoIndex;	
}
/**********************************************************************
name :
function : 
**********************************************************************/
void Servo::detach()
{
	timer16_Sequence_t timer;

	servos[this->servoIndex].Pin.isActive = false;
	timer = SERVO_INDEX_TO_TIMER(servoIndex);
	if(isTimerActive(timer) == false) 
	{
		finISR(timer);
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void Servo::write(uint16_t value)
{
	if (value < MIN_PULSE_WIDTH)
	{	// treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
		if (value < 0)
			value = 0;
		else if (value > 180)
			value = 180;

		value = map(value, 0, 180, SERVO_MIN(), SERVO_MAX());
	}
	this->writeMicroseconds(value);	
}
/**********************************************************************
name :
function : 
**********************************************************************/
void Servo::writeMicroseconds(uint16_t value)
{
	uint8_t channel;
	
	channel = this->servoIndex;
	if(channel < MAX_SERVOS)
	{
		if( value < SERVO_MIN() )          // ensure pulse width is valid
			value = SERVO_MIN();
		else if( value > SERVO_MAX() )
			value = SERVO_MAX();   
    
		value = value - TRIM_DURATION;
		value = usToTicks(value);  // convert to ticks after compensating for interrupt overhead - 12 Aug 2009

		//uint8_t oldSREG = SREG;
		//cli();
		servos[channel].ticks = value;  
		//SREG = oldSREG;   
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint16_t Servo::read() // return the value as degrees
{
	return  map( this->readMicroseconds()+1, SERVO_MIN(), SERVO_MAX(), 0, 180);     
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint16_t Servo::readMicroseconds()
{
	uint16_t pulsewidth;
	if( this->servoIndex != INVALID_SERVO )
		pulsewidth = ticksToUs(servos[this->servoIndex].ticks)  + TRIM_DURATION ;   // 12 aug 2009
	else 
		pulsewidth  = 0;

	return pulsewidth;   
}
/**********************************************************************
name :
function : 
**********************************************************************/
bool Servo::attached()
{
	return servos[this->servoIndex].Pin.isActive;
}


















