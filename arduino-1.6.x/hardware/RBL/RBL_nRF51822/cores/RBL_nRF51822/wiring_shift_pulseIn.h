
#ifndef WIRING_SHIFT_PULSEIN_H_
#define WIRING_SHIFT_PULSEIN_H_

uint32_t shiftIn(uint32_t DataPin, uint32_t ClockPin, BitOrder Order);
void shiftOut(uint32_t DataPin, uint32_t ClockPin, BitOrder Order, uint8_t value);
uint32_t pulseIn(uint32_t pin, uint8_t state, uint32_t timeout);


#endif

