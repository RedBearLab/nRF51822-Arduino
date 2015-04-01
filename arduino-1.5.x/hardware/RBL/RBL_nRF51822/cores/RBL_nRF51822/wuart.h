
#ifndef _WUART_H_
#define _WUART_H_

#include "Arduino.h"

#ifdef __cplusplus
#include "wuartclass.h"
#endif


typedef enum
{
	UART0_NotStart,
	UART0_BeforeFirstTX,
	UART0_AfterFirstTX,
	
}UART0_States;

extern uint8_t UART0_ReadRXState();
extern void UART0_ClearRXState();
extern uint8_t UART0_ReadRXDate();
extern uint8_t UART0_CheckRXError();
extern void UART0_WaitTXFinish();

extern void UART0_Start(uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin );
extern void UART0_Stop();
extern void UART0_TX(uint8_t dat);
extern uint8_t UART0_RX();

extern UART0_States UART0_State;

#ifdef __cplusplus
extern UARTClass Serial;
#endif



#endif
