
#include "Arduino.h"
#include "wiring_serial.h"
#include "serial_api.h"

Buffer 		rxbuffer;
UARTClass 	Serial;

void store(uint8_t c)
{
	uint8_t i = ( uint8_t )( rxbuffer.rx_Head +1 ) % SERIAL_BUFFER_MAX_SIZE;
	
	if( i != rxbuffer.rx_Tail )
	{
		rxbuffer.RX_buff[rxbuffer.rx_Head] = c;
		rxbuffer.rx_Head = i;
	}
}

UARTClass::UARTClass()
{

}

void UARTClass::attach( uart_irq_handler handle )
{
	UARTName uart = UART_0;
	serial_t obj;
	
	obj.uart = (NRF_UART_Type *)uart;
	obj.index = 0;	/* must be 0 */
	
	if(handle == NULL)
		return;
	/* id : if 0, don't call handle */
	serial_irq_handler(&obj, handle, 1);
}

void UARTClass::begin(const uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin)
{
	PinName	pin_rx, pin_tx;
	UARTName uart = UART_0;
	serial_t obj;
	
	pin_rx = Pin_nRF51822_to_Arduino(rx_pin);
	pin_tx = Pin_nRF51822_to_Arduino(tx_pin);
	
	obj.uart = (NRF_UART_Type *)uart;
	serial_init(&obj, pin_tx, pin_rx);
	serial_set_flow_control(&obj, FlowControlNone, (PinName)NC, (PinName)NC);
	serial_baud(&obj, BaudRate);
	/* TXIrq has a error, don't use it */
	serial_irq_set(&obj, RxIrq, 1);
	//serial_irq_set(&obj, TxIrq, 1);
}

void UARTClass::begin(const uint32_t BaudRate )
{	
	begin(BaudRate, DEFAULT_RX_PIN, DERAULT_TX_PIN);
}

void UARTClass::end(void)
{
	//must clear PSELTXD and PSELRXD
	NRF_UART0->PSELTXD = 0xFFFFFFFF;	
	NRF_UART0->PSELRXD = 0xFFFFFFFF;
	NRF_UART0->PSELCTS = 0xFFFFFFFF;	
	NRF_UART0->PSELRTS = 0xFFFFFFFF;	
	NRF_UART0->TASKS_STOPTX = 1;
	NRF_UART0->TASKS_STOPRX = 1;
	//Disable UART and clear event
	NRF_UART0->ENABLE       = (UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos);
	NRF_UART0->INTENCLR = 0xffffffffUL;
	//Clean buffer
	rxbuffer.rx_Head = rxbuffer.rx_Tail;
}

size_t UARTClass::write( const uint8_t c)
{
	UARTName uart = UART_0;
	serial_t obj;
	
	obj.uart = (NRF_UART_Type *)uart;
	serial_putc(&obj, c);
}

int UARTClass::available( void )
{
	return (uint32_t)(SERIAL_BUFFER_MAX_SIZE + rxbuffer.rx_Head - rxbuffer.rx_Tail) % SERIAL_BUFFER_MAX_SIZE; 
}

int UARTClass::read( void )
{	
	uint8_t dat;

	if(rxbuffer.rx_Head == rxbuffer.rx_Tail)
		return -1;
		
	dat = rxbuffer.RX_buff[rxbuffer.rx_Tail];
	rxbuffer.rx_Tail = (unsigned int)(rxbuffer.rx_Tail + 1) % SERIAL_BUFFER_MAX_SIZE;
	
	return dat;
}

int UARTClass::peek( void )
{
	if(rxbuffer.rx_Head == rxbuffer.rx_Tail)
		return -1;	
		
	return rxbuffer.RX_buff[rxbuffer.rx_Tail];
}

void UARTClass::flush( void )
{
	UARTName uart = UART_0;
	serial_t obj;
	
	obj.uart = (NRF_UART_Type *)uart;	
	/* Wait EVENTS_TXDRDY = 1 */
	while( !(serial_writable(&obj)) );
}


















