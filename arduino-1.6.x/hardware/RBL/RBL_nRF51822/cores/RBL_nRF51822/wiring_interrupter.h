
#ifndef WIRING_INTERRUPTER_H_
#define WIRING_INTERRUPTER_H_


typedef void (*irqHandle_t)(void);


typedef struct{
	
	gpio_irq_t	 gpio_irq;
	gpio_t		 gpio;
	irqHandle_t	 riseHandle;
	irqHandle_t	 fallHandle;
	
}PinHandle_t;


void attachInterrupt(PinName pin, irqHandle_t handle, uint8_t mode);
void detachInterrupt(PinName pin );


#endif


