

#include "Arduino.h"


uint32_t pulseIn( uint32_t pin, uint32_t state, uint32_t timeout )
{
	uint32_t width = 0, nrf_pin; 
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin < 31)
	{
		uint32_t numloops = 0;
		uint32_t maxloops = microsecondsToClockCycles(timeout) / 2;

		// wait for any previous pulse to end
		while (((NRF_GPIO->IN >> nrf_pin) & 1UL) == state)
			if (numloops++ == maxloops)
				return 0;

		// wait for the pulse to start
		while (((NRF_GPIO->IN >> nrf_pin) & 1UL) != state)
			if (numloops++ == maxloops)
				return 0;

		// wait for the pulse to stop
		while (((NRF_GPIO->IN >> nrf_pin) & 1UL) == state) 
		{
			if (numloops++ == maxloops)
				return 0;
			width++;
		}
		
		return clockCyclesToMicroseconds(width * 17 + 16);
	}
	else 
	{
		return 0;
	}
}
