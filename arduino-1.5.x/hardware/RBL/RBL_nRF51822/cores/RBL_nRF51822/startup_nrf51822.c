

#include "nrf51.h"
#include "startup_nrf51822.h"
#include "system_nrf51.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/* C library initialization */
void __libc_init_array(void);

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void Reset_Handler(void);
void Default_Handler(void);

/* Cortex-M0 core handlers */
void NMI_Handler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler  ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler     ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler    ( void ) __attribute__ ((weak, alias("Default_Handler")));

/* Peripherals handlers */
void POWER_CLOCK_IRQHandler ( void ) __attribute__ ((weak, alias("Default_Handler")));
void RADIO_IRQHandler       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQHandler       ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SPI0_TWI0_IRQHandler   ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_TWI1_IRQHandler   ( void ) __attribute__ ((weak, alias("Default_Handler")));
void GPIOTE_IRQHandler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_IRQHandler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_IRQHandler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TIMER2_IRQHandler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void RTC0_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void TEMP_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void RNG_IRQHandler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void ECB_IRQHandler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void CCM_AAR_IRQHandler     ( void ) __attribute__ ((weak, alias("Default_Handler")));
void WDT_IRQHandler         ( void ) __attribute__ ((weak, alias("Default_Handler")));
void RTC1_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void QDEC_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void WUCOMP_IRQHandler      ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SWI0_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SWI1_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SWI2_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SWI3_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SWI4_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));
void SWI5_IRQHandler        ( void ) __attribute__ ((weak, alias("Default_Handler")));

typedef struct
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M0 core handlers */
  void* pfnReset_Handler;
  void* pfnNMI_Handler;
  void* pfnHardFault_Handler;
  void* pfnReserved1_Handler;
  void* pfnReserved2_Handler;
  void* pfnReserved3_Handler;
  void* pfnReserved4_Handler;
  void* pfnReserved5_Handler;
  void* pfnReserved6_Handler;
  void* pfnReserved7_Handler;
  void* pfnSVC_Handler;
  void* pfnReserved8_Handler;
  void* pfnReserved9_Handler;
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Configurable interrupts */
  void* pfnPOWER_CLOCK_IRQHandler;    /* 0  Power Clock */
  void* pfnRADIO_IRQHandler;          /* 1  Radio Controller*/
  void* pfnUART0_IRQHandler;          /* 2  UART0 Controller */
  void* pfnSPI0_TWI0_IRQHandler;      /* 3  SPI0 & TWI0 Controller*/
  void* pfnSPI1_TWI1_IRQHandler;      /* 4  SPI1 & TWI1 Controller*/
  void* pfnReserved10_IRQHandler;     /* 5  Reserved */
  void* pfnGPIOTE_IRQHandler;         /* 6  GPIO */
  void* pfnADC_IRQHandler;            /* 7  ADC Controller */
  void* pfnTIMER0_IRQHandler;         /* 8  Timer Counter 0 */
  void* pfnTIMER1_IRQHandler;         /* 9  Timer Counter 1 */
  void* pfnTIMER2_IRQHandler;         /* 10 Timer Counter 2 */
  void* pfnRTC0_IRQHandler;           /* 11 Real Time Clock Controller 0 */
  void* pfnTEMP_IRQHandler;           /* 12 Temperature */
  void* pfnRNG_IRQHandler;            /* 13 Random Number Generator */
  void* pfnECB_IRQHandler;            /* 14 ECB */
  void* pfnCCM_AAR_IRQHandler;        /* 15 CCM_AAR */
  void* pfnWDT_IRQHandler;            /* 16 Watchdog Timer */
  void* pfnRTC1_IRQHandler;           /* 17 Real Time Clock Controller 1 */
  void* pfnQDEC_IRQHandler;           /* 18 QDEC */
  void* pfnWUCOMP_IRQHandler;         /* 19 WUCOMP */
  void* pfnSWI0_IRQHandler;           /* 20 Software Interrupt 0 */
  void* pfnSWI1_IRQHandler;           /* 21 Software Interrupt 1 */
  void* pfnSWI2_IRQHandler;           /* 22 Software Interrupt 2 */
  void* pfnSWI3_IRQHandler;           /* 23 Software Interrupt 3 */
  void* pfnSWI4_IRQHandler;           /* 24 Software Interrupt 4 */
  void* pfnSWI5_IRQHandler;           /* 25 Software Interrupt 5 */
  void* pfnReserved11_IRQHandler;     /* 26 Reserved */
  void* pfnReserved12_IRQHandler;     /* 27 Reserved */
  void* pfnReserved13_IRQHandler;     /* 28 Reserved */
  void* pfnReserved14_IRQHandler;     /* 29 Reserved */
  void* pfnReserved15_IRQHandler;     /* 30 Reserved */
  void* pfnReserved16_IRQHandler;     /* 31 Reserved */
} vector_table_t;

/* Vector Table */
__attribute__ ((section(".vectors")))
const vector_table_t vector_table = {
    /* Configure Initial Stack Pointer, using linker-generated symbols */
    (void*) (&_estack),
    (void*) Reset_Handler,
    (void*) NMI_Handler,
    (void*) HardFault_Handler,
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) SVC_Handler,
    (void*) (0UL),           /* Reserved */
    (void*) (0UL),           /* Reserved */
    (void*) PendSV_Handler,
    (void*) SysTick_Handler,

    /* Configurable interrupts */
    (void*) POWER_CLOCK_IRQHandler,    /* 0  Power Clock */
    (void*) RADIO_IRQHandler,          /* 1  Radio Controller */
    (void*) UART0_IRQHandler,          /* 2  UART0 Controller */
    (void*) SPI0_TWI0_IRQHandler,      /* 3  SPI0 & TWI0 Controller */
    (void*) SPI1_TWI1_IRQHandler,      /* 4  SPI1 & TWI1 Controller */
    (void*) (0UL),                     /* 5  Reserved */
    (void*) GPIOTE_IRQHandler,         /* 6  GPIO */
    (void*) ADC_IRQHandler,            /* 7  ADC Controller */
    (void*) TIMER0_IRQHandler,         /* 8  Timer Counter 0 */
    (void*) TIMER1_IRQHandler,         /* 9  Timer Counter 1 */
    (void*) TIMER2_IRQHandler,         /* 10 Timer Counter 2 */
    (void*) RTC0_IRQHandler,           /* 11 Real Time Clock Controller 0 */
    (void*) TEMP_IRQHandler,           /* 12 Temperature */
    (void*) RNG_IRQHandler,            /* 13 Random Number Generator */
    (void*) ECB_IRQHandler,            /* 14 ECB */
    (void*) CCM_AAR_IRQHandler,        /* 15 CCM_AAR */
    (void*) WDT_IRQHandler,            /* 16 Watchdog Timer */
    (void*) RTC1_IRQHandler,           /* 17 Real Time Clock Controller 1 */
    (void*) QDEC_IRQHandler,           /* 18 QDEC */
    (void*) WUCOMP_IRQHandler,         /* 19 WUCOMP */
    (void*) SWI0_IRQHandler,           /* 20 Software Interrupt 0 */
    (void*) SWI1_IRQHandler,           /* 21 Software Interrupt 1 */
    (void*) SWI2_IRQHandler,           /* 22 Software Interrupt 2 */
    (void*) SWI3_IRQHandler,           /* 23 Software Interrupt 3 */
    (void*) SWI4_IRQHandler,           /* 24 Software Interrupt 4 */
    (void*) SWI5_IRQHandler,           /* 25 Software Interrupt 5 */
    (void*) (0UL),                     /* 26 Reserved */
    (void*) (0UL),                     /* 27 Reserved */
    (void*) (0UL),                     /* 28 Reserved */
    (void*) (0UL),                     /* 29 Reserved */
    (void*) (0UL),                     /* 30 Reserved */
    (void*) (0UL)                      /* 31 Reserved */
};

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
    uint32_t *pSrc, *pDest;

    /* Initialize the relocate segment */
    pSrc = &_etext;
    pDest = &_srelocate;

    if (pSrc != pDest) {
        for (; pDest < &_erelocate;) {
            *pDest++ = *pSrc++;
        }
    }

    /* Clear the zero segment */
    for (pDest = &_szero; pDest < &_ezero;) {
        *pDest++ = 0;
    }

    /* Initialize the C library */
    __libc_init_array();

    /* Call SystemInit */
    SystemInit();

    /* Branch to main function */
    main();

    /* Infinite loop */
    while (1);
}

#ifdef DYNAMIC_HANDLERS

void Default_Dynamic_Handler(void);

/* exception number mirrors vector table */
dynamic_handler_t exception_handlers[48] =
{
    0,                        /* No Exception - Executing Thread */
    Default_Dynamic_Handler,  /* Reset */
    Default_Dynamic_Handler,  /* NMI */
    Default_Dynamic_Handler,  /* Hard Fault */
    0,                        /* Reserved */
    0,                        /* Reserved */
    0,                        /* Reserved */
    0,                        /* Reserved */
    0,                        /* Reserved */
    0,                        /* Reserved */
    0,                        /* Reserved */
    Default_Dynamic_Handler,  /* SVC */
    0,                        /* Reserved */
    0,                        /* Reserved */
    Default_Dynamic_Handler,  /* PendSV */
    Default_Dynamic_Handler   /* SysTick */
};

/* interrupt number: negative for non-configuable interrupts, positive for configurable interrupts */
dynamic_handler_t *dynamic_handlers = &exception_handlers[16];

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Default_Handler(void)
{
  // call the dynamic exception handler registered for the exception number of the the active vector
  // field of the interrupt control and state register of the system control block
  exception_handlers[SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk]();
}

/**
 * \brief Default dynamic interrupt handler for unused IRQs.
 */
void Default_Dynamic_Handler(void)
{
    while (1)
      ;
}

#else

/**
 * \brief Default interrupt handler for unused IRQs.
 */
void Default_Handler(void)
{
    while (1)
      ;
}

#endif
