

#ifndef _WIRING_DIGITAL_
#define _WIRING_DIGITAL_

#ifdef __cplusplus
extern "C" {
#endif


extern void pinMode( uint32_t ulPin, uint32_t ulMode );
 
extern void digitalWrite( uint32_t ulPin, uint32_t ulVal );

extern int digitalRead( uint32_t ulPin );

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_DIGITAL_ */
