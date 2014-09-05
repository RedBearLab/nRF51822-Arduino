

#define ARDUINO_MAIN

#include "Arduino.h"

/*
 * \brief Main entry point of Arduino application
 */


int main( void )
{
	lfclk_config();
	//RTC1_Init();
	rtc_timer_init();
    setup();

    for (;;)
    {
        loop();
    }

    return 0;
}
