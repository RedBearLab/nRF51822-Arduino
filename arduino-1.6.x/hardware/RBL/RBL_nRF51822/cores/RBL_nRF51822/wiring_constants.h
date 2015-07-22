
#ifndef WIRING_CONSTANTS_H_
#define WIRING_CONSTANTS_H_

#define VARIANT_MCK         		 16000000UL	

#define clockCyclesPerMicrosecond()  ( VARIANT_MCK / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )


#define INPUT			0x00
#define OUTPUT			0x01
#define INPUT_PULLUP 	0x02

#define LOW 			0x00
#define HIGH 			0x01
#define CHANGE 			0x02
#define FALLING 		0x03
#define RISING 			0x04


#define true 			0x1
#define false 			0x0

#define PI 				3.1415926535897932384626433832795
#define HALF_PI 		1.5707963267948966192313216916398
#define TWO_PI 			6.283185307179586476925286766559
#define DEG_TO_RAD 		0.017453292519943295769236907684886
#define RAD_TO_DEG 		57.295779513082320876798154814105


#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif // min

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif // max

#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))


#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

#define bit(b) (1UL << (b))

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

typedef unsigned int word;
typedef uint8_t boolean ;
typedef uint8_t byte ;

enum BitOrder {
	LSBFIRST = 0,
	MSBFIRST = 1
};

#endif

