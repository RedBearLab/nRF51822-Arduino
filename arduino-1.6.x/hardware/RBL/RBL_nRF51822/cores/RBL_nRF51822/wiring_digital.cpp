

#include "Arduino.h"
#include "wiring_digital.h"


void pinMode(PinName pin, uint32_t mode)
{	
	gpio_t			gpio;
	PinMode			pin_mode;
	PinDirection	direction;
	
	MBED_ASSERT(pin != (PinName)NC);
		
	gpio.pin = pin;
	
	switch(mode)
	{
		case INPUT : direction = PIN_INPUT; pin_mode = PullNone; break;
		case OUTPUT : direction = PIN_OUTPUT; pin_mode = PullNone; break;
		case INPUT_PULLUP : direction = PIN_INPUT; pin_mode = PullUp; break;
		default : return;
	}
	gpio_dir(&gpio, direction);
	gpio_mode(&gpio, pin_mode);
}

void digitalWrite(PinName pin, uint32_t value)
{
	MBED_ASSERT(pin != (PinName)NC);
	if(value)
		NRF_GPIO->OUTSET = (1ul << pin);
	else
		NRF_GPIO->OUTCLR = (1ul << pin);
}

uint32_t digitalRead(PinName pin)
{
	MBED_ASSERT(pin != (PinName)NC);
	return ( (NRF_GPIO->IN >> pin) & 1UL );
}