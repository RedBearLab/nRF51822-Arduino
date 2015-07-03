
#define ARDUINO_MAIN

#include "Arduino.h"

int __attribute__ ((weak)) main( void )
{
	setup();
	
	while(1)
	{
		loop();
	}
	return 0;
}