

#include "Arduino.h"
#include "wiring_digital.h"


void pinMode(uint32_t pin, uint32_t mode)
{	
	PinName			nrf_pin;
	gpio_t			gpio;
	PinMode			pin_mode;
	PinDirection	direction;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	MBED_ASSERT(nrf_pin != (PinName)NC);
		
	gpio.pin = nrf_pin;
	
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

void digitalWrite(uint32_t pin, uint32_t value)
{
	PinName			nrf_pin;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	MBED_ASSERT(nrf_pin != (PinName)NC);
	if(value)
		NRF_GPIO->OUTSET = (1ul << nrf_pin);
	else
		NRF_GPIO->OUTCLR = (1ul << nrf_pin);
}


uint32_t digitalRead(uint32_t pin)
{
	PinName			nrf_pin;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	MBED_ASSERT(nrf_pin != (PinName)NC);
	
	return ( (NRF_GPIO->IN >> nrf_pin) & 1UL );
}



