
#include "wuart.h"


/**********************************************************************
declared
**********************************************************************/
UART0_States UART0_State = UART0_NotStart;

Buffer rxbuffer;

UARTClass Serial( &rxbuffer );

static void UART0_handler( void );
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t UART0_ReadRXState()
{
	return NRF_UART0->EVENTS_RXDRDY;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UART0_ClearRXState()
{
	NRF_UART0->EVENTS_RXDRDY = 0;
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t UART0_ReadRXDate()
{
	return NRF_UART0->RXD;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UART0_WaitTXFinish()
{
	if(	UART0_State == UART0_AfterFirstTX )
	{
		while( !NRF_UART0->EVENTS_TXDRDY );
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
//void UART0_IRQHandler( void )
static void UART0_handler( void )
{	
	 Serial.irq_handler();
/*
	if( NRF_UART0->EVENTS_RXDRDY )
	{
		NRF_UART0->EVENTS_RXDRDY = 0;
		
		uint8_t dat = NRF_UART0->RXD;
		
		if( UART0_CheckRXError() )
			return;
			
		rx_buffer->store_char( dat );	
	}
*/
}
/**********************************************************************
name : void UART0_Start(uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin )
function : 
**********************************************************************/
void UART0_Start(uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin )
{
	if( UART0_State != UART0_NotStart )
	{
		return;
	}
	
    NRF_GPIO->PIN_CNF[tx_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                   | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                   | (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
	NRF_GPIO->PIN_CNF[rx_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
									| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                    | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                    | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                    | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);		

	NRF_UART0->PSELTXD = tx_pin;	
	NRF_UART0->PSELRXD = rx_pin;
	NRF_UART0->PSELRTS = 0xFFFFFFFF;
    NRF_UART0->PSELCTS = 0xFFFFFFFF;
	
	uint32_t baud;
	switch(BaudRate)
	{
		case 1200: baud = UART_BAUDRATE_BAUDRATE_Baud1200; break;
		case 2400: baud = UART_BAUDRATE_BAUDRATE_Baud2400; break;
		case 4800: baud = UART_BAUDRATE_BAUDRATE_Baud4800; break;
		case 9600: baud = UART_BAUDRATE_BAUDRATE_Baud9600; break;
		case 14400: baud = UART_BAUDRATE_BAUDRATE_Baud14400; break;
		case 19200: baud = UART_BAUDRATE_BAUDRATE_Baud19200; break;
		case 28800: baud = UART_BAUDRATE_BAUDRATE_Baud28800; break;
		case 38400: baud = UART_BAUDRATE_BAUDRATE_Baud38400; break;
		case 57600: baud = UART_BAUDRATE_BAUDRATE_Baud57600; break;
		case 76800: baud = UART_BAUDRATE_BAUDRATE_Baud76800; break;
		case 115200: baud = UART_BAUDRATE_BAUDRATE_Baud115200; break;
		case 230400: baud = UART_BAUDRATE_BAUDRATE_Baud230400; break;
		case 250000: baud = UART_BAUDRATE_BAUDRATE_Baud250000; break;
		case 460800: baud = UART_BAUDRATE_BAUDRATE_Baud460800; break;
		case 921600: baud = UART_BAUDRATE_BAUDRATE_Baud921600; break;
		case 1000000: baud = UART_BAUDRATE_BAUDRATE_Baud1M; break;			
		default: baud = UART_BAUDRATE_BAUDRATE_Baud115200;break;
	}
	
	NRF_UART0->BAUDRATE         = (baud << UART_BAUDRATE_BAUDRATE_Pos);
	NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
	NRF_UART0->TASKS_STARTTX    = 1;
	NRF_UART0->TASKS_STARTRX    = 1;
	NRF_UART0->EVENTS_RXDRDY    = 0;
	
	NRF_UART0->INTENCLR = 0xffffffffUL;
	NRF_UART0->INTENSET = UART_INTENSET_RXDRDY_Enabled << UART_INTENSET_RXDRDY_Pos;
	NVIC_SetPriority(UART0_IRQn, 3);
	
	LinkInterrupt( UART0_IRQn, UART0_handler);
    NVIC_EnableIRQ(UART0_IRQn);
	
	UART0_State = UART0_BeforeFirstTX;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UART0_Stop()
{
	if(UART0_State == UART0_NotStart)
	{
		return;
	}
	
	NVIC_DisableIRQ(UART0_IRQn);
	// wait for any date to be sent
	if( UART0_State == UART0_AfterFirstTX )
	{
		while( !NRF_UART0->EVENTS_TXDRDY);
	}
	//must clear PSELTXD and PSELRXD
	NRF_UART0->PSELTXD = 0xFFFFFFFF;	
	NRF_UART0->PSELRXD = 0xFFFFFFFF;
	NRF_UART0->TASKS_STOPTX = 1;
	NRF_UART0->TASKS_STOPRX = 1;
	//Disable UART and clear event
	NRF_UART0->ENABLE       = (UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos);
	NRF_UART0->INTENCLR = 0xffffffffUL;
	
	UART0_State = UART0_NotStart;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UART0_TX(uint8_t dat)
{
	if( UART0_State == UART0_AfterFirstTX )
	{
		while( !NRF_UART0->EVENTS_TXDRDY);
	}
	
	NRF_UART0->EVENTS_TXDRDY = 0;
	NRF_UART0->TXD = dat;
	
	if (UART0_State == UART0_BeforeFirstTX)
	{
		UART0_State = UART0_AfterFirstTX;
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t UART0_CheckRXError()
{
	if (NRF_UART0->ERRORSRC & UART_ERRORSRC_OVERRUN_Msk)
	{
		NRF_UART0->ERRORSRC = (UART_ERRORSRC_OVERRUN_Clear << UART_ERRORSRC_OVERRUN_Pos);
		return true;
	}
	  /*
	  else if (NRF_UART0->ERRORSRC & UART_ERRORSRC_PARITY_Msk)
	  {
		NRF_UART0->ERRORSRC = (UART_ERRORSRC_PARITY_Clear << UART_ERRORSRC_PARITY_Pos);
	  }
	  */
	else if (NRF_UART0->ERRORSRC & UART_ERRORSRC_FRAMING_Msk)
	{
		NRF_UART0->ERRORSRC = (UART_ERRORSRC_FRAMING_Clear << UART_ERRORSRC_FRAMING_Pos);
		return true;
	}
	  /*
	  else if (NRF_UART0->ERRORSRC & UART_ERRORSRC_BREAK_Msk)
	  {
		NRF_UART0->ERRORSRC = (UART_ERRORSRC_BREAK_Clear << UART_ERRORSRC_BREAK_Pos);
	  }
	  */

	return false;	
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t UART0_RX()
{
	uint8_t dat;
	
	do
	{
		while(!NRF_UART0->EVENTS_RXDRDY);
		
		NRF_UART0->EVENTS_RXDRDY = 0;
		
		dat = NRF_UART0->RXD;
	}while( UART0_CheckRXError() );

	return dat;
}


