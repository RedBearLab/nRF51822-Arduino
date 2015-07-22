
#ifndef WIRING_SHIFT_PULSEIN_H_
#define WIRING_SHIFT_PULSEIN_H_

uint32_t shiftIn(PinName DataPin, PinName ClockPin, BitOrder Order);
void shiftOut(PinName DataPin, PinName ClockPin, BitOrder Order, uint8_t value);
uint32_t pulseIn(PinName pin, uint8_t state, uint32_t timeout);


#endif

