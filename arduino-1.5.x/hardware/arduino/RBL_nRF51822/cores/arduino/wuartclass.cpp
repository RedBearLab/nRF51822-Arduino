

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "wuartclass.h"
#include "wuart.h"

/**********************************************************************
name :
function : 
**********************************************************************/
UARTClass::UARTClass( Buffer *pRx_buffer )
{
	rx_buffer = pRx_buffer;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UARTClass::begin( const uint32_t BaudRate )
{	//Use default pin, define on pins_arduino.h
	UART0_Start( BaudRate, DEFAULT_RX_PIN, DERAULT_TX_PIN );
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UARTClass::begin(const uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin)
{	
	uint32_t t_pin, r_pin;
	//transform arduino_pin to nrf51_pin
	t_pin = Pin_nRF51822_to_Arduino(tx_pin);
	r_pin = Pin_nRF51822_to_Arduino(rx_pin);
	
	UART0_Start( BaudRate, r_pin, t_pin );
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UARTClass::end( void )
{	//Stop UART and clear buff
	UART0_Stop();
	rx_buffer->rx_Head = rx_buffer->rx_Tail;
}
/**********************************************************************
name :
function : 
**********************************************************************/
int UARTClass::available( void )
{
	return (uint32_t)(SERIAL_BUFFER_MAX_SIZE + rx_buffer->rx_Head - rx_buffer->rx_Tail) % SERIAL_BUFFER_MAX_SIZE; 
}
/**********************************************************************
name :
function : 
**********************************************************************/
size_t UARTClass::write( const uint8_t c)
{
	UART0_TX( c );
}
/**********************************************************************
name :
function : 
**********************************************************************/
int UARTClass::read( void )
{	
	uint8_t dat;

	if(rx_buffer->rx_Head == rx_buffer->rx_Tail)
		return -1;
		
	dat = rx_buffer->RX_buff[rx_buffer->rx_Tail];
	rx_buffer->rx_Tail = (unsigned int)(rx_buffer->rx_Tail + 1) % SERIAL_BUFFER_MAX_SIZE;
	
	return dat;
}
/**********************************************************************
name :
function : read a data but not increase rx_buff_Tail
**********************************************************************/
int UARTClass::peek( void )
{
	if(rx_buffer->rx_Head == rx_buffer->rx_Tail)
		return -1;	
		
	return rx_buffer->RX_buff[rx_buffer->rx_Tail];
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UARTClass::flush( void )
{
	UART0_WaitTXFinish();
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UARTClass::irq_handler()
{
	uint8_t dat;
	
	if( UART0_ReadRXState() )
	{	
		UART0_ClearRXState();
		
		dat = UART0_ReadRXDate();
		
		if( UART0_CheckRXError() )
			return;
			
		rx_buffer->store_char( dat );	
	}
    if(UART_CallBack != NULL)
    {	//if you register UART_CallBack, when receiver data, will callback this function.
		//it is used by run with Low_Power, see example: SimpleChat.
        UART_CallBack();
    }
}

void UARTClass::irq_attach( uart_callback_t UART_CallBackFunc )
{
    UART_CallBack = UART_CallBackFunc;
}








