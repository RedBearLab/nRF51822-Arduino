

#ifndef _TONE_H_
#define _TONE_H_


#include "Arduino.h"


extern void tone(uint32_t pin, uint16_t freq, uint32_t duration);
extern void tone(uint32_t pin, uint16_t freq);
extern void noTone(uint32_t pin);


#endif


