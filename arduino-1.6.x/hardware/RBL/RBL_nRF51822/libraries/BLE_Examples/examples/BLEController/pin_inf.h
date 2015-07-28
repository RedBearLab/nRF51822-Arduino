
#ifndef PIN_INF_H_
#define PIN_INF_H_


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

