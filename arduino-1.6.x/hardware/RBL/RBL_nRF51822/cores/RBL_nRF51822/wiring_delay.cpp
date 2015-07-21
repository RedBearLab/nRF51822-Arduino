
#include "Arduino.h"
#include "wiring_delay.h"


void delayMicroseconds(uint32_t us)
{
	uint32_t start = us_ticker_read();
	while((us_ticker_read() - start) < (uint32_t)us);
}

void delay(uint32_t ms)
{
	delayMicroseconds(ms*1000);	
}

uint32_t micros( void )
{
	return us_ticker_read();
}

uint32_t millis( void )
{
	return ( us_ticker_read() / 1000);
}


