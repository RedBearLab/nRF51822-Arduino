
#include "Arduino.h"
#include "wiring_shift_pulseIn.h"


uint32_t shiftIn(uint32_t DataPin, uint32_t ClockPin, BitOrder Order)
{
	uint8_t value;
	uint8_t index;
	
	value = 0;
	for(index=0; index<8; index++){
		digitalWrite(ClockPin, HIGH);
		if(Order == LSBFIRST){
			value |= (digitalRead(DataPin) << index);
		}
		else {
			value |= (digitalRead(DataPin) << (7-index));
		}
		digitalWrite(ClockPin, LOW);
	}
	
	return value;
}

void shiftOut(uint32_t DataPin, uint32_t ClockPin, BitOrder Order, uint8_t value)
{
	uint8_t index;
	
	for(index=0; index<8; index++){
		if(Order == LSBFIRST){
			digitalWrite(DataPin, !!(value&(1<<index)));
		}
		else {
			digitalWrite(DataPin, !!(value&(1<<(7-index))));
		}
	}
	digitalWrite(ClockPin, HIGH);
	digitalWrite(ClockPin, LOW);
}


uint32_t pulseIn(uint32_t pin, uint8_t state, uint32_t timeout)
{
	uint32_t width, numloops;
	uint32_t maxloops = microsecondsToClockCycles(timeout) / 2;

	width = 0;
	numloops = 0;
	// wait for any previous pulse to end
	while( digitalRead(pin) == state){
		if(numloops++ == maxloops)
			return 0;
	}
	// wait for the pulse to start
	while( digitalRead(pin) != state){
		if(numloops++ == maxloops)
			return 0;
	}	
	// wait for the pulse to stop
	while( digitalRead(pin) == state){
		if(numloops++ == maxloops)
			return 0;
		width++;
	}	
	
	return clockCyclesToMicroseconds(width * 17 + 16);
}















