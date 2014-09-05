#ifndef __SYSTEM_NRF51_H
#define __SYSTEM_NRF51_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define __XTAL              (16000000UL)    /* Oscillator frequency */

extern uint32_t SystemCoreClock;    /*!< System Clock Frequency (Core Clock)  */

extern void SystemInit (void);

extern void SystemCoreClockUpdate (void);

#ifdef __cplusplus
}
#endif

#endif