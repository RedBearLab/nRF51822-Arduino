

#ifndef _WIRING_
#define _WIRING_

#ifdef __cplusplus
extern "C" {
#endif

extern void lfclk_config(void);

extern uint32_t millis( void );
extern uint32_t micros( void );
extern void delay( uint32_t ms ) ;
extern void delayMicroseconds( uint32_t us );
extern void rtc_timer_init();

#ifdef __cplusplus
}
#endif

#endif 
