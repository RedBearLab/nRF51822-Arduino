
#include "Arduino.h"
#include "wiring_interrupter.h"


PinHandle_t pinHandle[31];

void gpio_irq_handle_cb(uint32_t id, gpio_irq_event event)
{
	uint8_t 			pin_state;
	uint32_t 			cnt=0, cnt1=0;
	PinHandle_t *handler = (PinHandle_t *)id;
	
    switch (event) {
        case IRQ_RISE: 
			pin_state = ( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL );
			for(cnt=0; cnt<100000; cnt++) {
				if(cnt >= 99999) {
					return;
				}
				if(( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL ) != pin_state) {
					pin_state = ( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL );
					cnt1 = 0;
				}
				else{
					//wait about 1ms to make sure it's not voltage jitter
					if(cnt1++ > 5000) {
						break;
					}
				}
			}
			
			NVIC_ClearPendingIRQ(GPIOTE_IRQn);
			
			if( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL ) {
				if(handler->riseHandle != NULL)
					(handler->riseHandle)(); 
			}
			break;
        case IRQ_FALL: 
			pin_state = ( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL );
			for(cnt=0; cnt<100000; cnt++) {
				if(cnt >= 99999) {
					return;
				}
				if(( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL ) != pin_state) {
					pin_state = ( (NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL );
					cnt1 = 0;
				}
				else{
					//wait about 1ms to make sure it's not voltage jitter
					if(cnt1++ > 5000) {
						break;
					}
				}
			}
			
			NVIC_ClearPendingIRQ(GPIOTE_IRQn);
			
			if( ((NRF_GPIO->IN >> (uint8_t)handler->gpio_irq.ch ) & 1UL) == 0) {
				if(handler->fallHandle != NULL)
					(handler->fallHandle)(); 
			}
			break;
        case IRQ_NONE: break;
    }

}

void attachInterrupt(uint32_t pin, irqHandle_t handle, uint8_t mode)
{
	PinName nrf_pin;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);

	if(nrf_pin == (PinName)NC || handle == NULL )
		return;
		
	if(mode != CHANGE && mode != FALLING && mode != RISING )
		return;
	
	if(mode == CHANGE) {
		pinHandle[(uint32_t)nrf_pin].riseHandle = handle;
		pinHandle[(uint32_t)nrf_pin].fallHandle = handle;
	}
	else if(mode == RISING) {
		pinHandle[(uint32_t)nrf_pin].riseHandle = handle;
		pinHandle[(uint32_t)nrf_pin].fallHandle = NULL;
	}
	else {
		pinHandle[(uint32_t)nrf_pin].riseHandle = NULL;
		pinHandle[(uint32_t)nrf_pin].fallHandle = handle;	
	}
	
	gpio_irq_init(&pinHandle[(uint32_t)nrf_pin].gpio_irq, nrf_pin, gpio_irq_handle_cb, (uint32_t)&pinHandle[(uint32_t)nrf_pin]);
	gpio_init_in(&pinHandle[(uint32_t)nrf_pin].gpio, nrf_pin);

	if(mode == CHANGE) {
		gpio_irq_set(&pinHandle[(uint32_t)nrf_pin].gpio_irq, IRQ_RISE, 1);
		gpio_irq_set(&pinHandle[(uint32_t)nrf_pin].gpio_irq, IRQ_FALL, 1);
	}
	else if(mode == RISING) {
		gpio_irq_set(&pinHandle[(uint32_t)nrf_pin].gpio_irq, IRQ_RISE, 1);
	}
	else {
		gpio_irq_set(&pinHandle[(uint32_t)nrf_pin].gpio_irq, IRQ_FALL, 1);
	}	
}


void detachInterrupt(uint32_t pin )
{
	PinName nrf_pin;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	
	gpio_irq_free(&pinHandle[(uint32_t)nrf_pin].gpio_irq);
	memset(&pinHandle[(uint32_t)nrf_pin], 0x00, sizeof(PinHandle_t));
	pinHandle[(uint32_t)nrf_pin].riseHandle = NULL;
	pinHandle[(uint32_t)nrf_pin].fallHandle = NULL;	
}



















