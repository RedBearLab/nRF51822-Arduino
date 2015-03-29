#ifndef __STARTUP_NRF51822_H
#define __STARTUP_NRF51822_H

#ifdef __cplusplus
extern "C" {
#endif

#define DYNAMIC_HANDLERS

/* Cortex-M0 core handlers */
void NMI_Handler        ( void );
void HardFault_Handler  ( void );
void SVC_Handler        ( void );
void PendSV_Handler     ( void );
void SysTick_Handler    ( void );

/* peripheral handlers */
void POWER_CLOCK_IRQHandler ( void );
void RADIO_IRQHandler       ( void );
void UART0_IRQHandler       ( void );
void SPI0_TWI0_IRQHandler   ( void );
void SPI1_TWI1_IRQHandler   ( void );
void GPIOTE_IRQHandler      ( void );
void ADC_IRQHandler         ( void );
void TIMER0_IRQHandler      ( void );
void TIMER1_IRQHandler      ( void );
void TIMER2_IRQHandler      ( void );
void RTC0_IRQHandler        ( void );
void TEMP_IRQHandler        ( void );
void RNG_IRQHandler         ( void );
void ECB_IRQHandler         ( void );
void CCM_AAR_IRQHandler     ( void );
void WDT_IRQHandler         ( void );
void RTC1_IRQHandler        ( void );
void QDEC_IRQHandler        ( void );
void WUCOMP_IRQHandler      ( void );
void SWI0_IRQHandler        ( void );
void SWI1_IRQHandler        ( void );
void SWI2_IRQHandler        ( void );
void SWI3_IRQHandler        ( void );
void SWI4_IRQHandler        ( void );
void SWI5_IRQHandler        ( void );

#ifdef DYNAMIC_HANDLERS

typedef void (*dynamic_handler_t)(void);
extern dynamic_handler_t *dynamic_handlers/*[32]*/;

#endif

#ifdef __cplusplus
}
#endif

#endif