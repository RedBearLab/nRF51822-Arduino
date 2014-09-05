

#ifndef _WIRING_
#define _WIRING_

#ifdef __cplusplus
extern "C" {
#endif

extern void lfclk_config(void);
//extern void RTC1_Init(void);

extern uint32_t millis( void ) ;
extern uint32_t micros( void ) ;
extern void delay( uint32_t dwMs ) ;
extern void delayMicroseconds( uint32_t dwUs ) ;
extern void rtc_timer_init();
#ifdef __cplusplus
}
#endif

#endif 
