
#ifndef PIN_INF_H_
#define PIN_INF_H_

#include "Arduino.h"

#ifdef RBL_NRF51822  

#define TOTAL_ANALOG_PINS 3
#define TOTAL_PINS        24
#define VERSION_BLINK_PIN 13
#define IS_PIN_DIGITAL(p) ( (p) >= 2 && (p) < 24 )
#define IS_PIN_ANALOG(p)  ( (p) >= 19 && (p) < 24)
#define IS_PIN_PWM(p)     ( (p) == 3 || (p) == 5 || (p) == 6 )
#define IS_PIN_SERVO(p)   ( (p) > 2 && (p) < 12  )

#define PIN_TO_DIGITAL(p) (p)
#define PIN_TO_ANALOG(p)  ((p)-19)
#define PIN_TO_PWM(p)     PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)   (p)

#endif

#ifdef BLE_NANO

#define TOTAL_ANALOG_PINS 3
#define TOTAL_PINS        (14+1)
#define VERSION_BLINK_PIN 13
#define IS_PIN_DIGITAL(p) ( (p) >= 2 && (p) <= 14 )
#define IS_PIN_ANALOG(p)  ( (p) == 8 || (p) == 9 || (p) == 10 || (p) == 11 || (p) == 12 || (p) == 14) //A0~A5
#define IS_PIN_PWM(p)     ( (p) == 3 || (p) == 5 || (p) == 6 )
#define IS_PIN_SERVO(p)   ( (p) > 2 && (p) <= 7  )

#define PIN_TO_DIGITAL(p) (p)
#define PIN_TO_PWM(p)     PIN_TO_DIGITAL(p)
#define PIN_TO_SERVO(p)   (p)

#endif

#endif

