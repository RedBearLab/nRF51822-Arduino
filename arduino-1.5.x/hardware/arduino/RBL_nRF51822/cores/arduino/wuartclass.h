
#ifndef _WUARTCLASS_H_
#define _WUARTCLASS_H_

#include "HardwareSerial.h"
#include "wuartbuff.h"

typedef void (*uart_callback_t)(void);

class UARTClass : public HardwareSerial
{
	protected:
		Buffer             *rx_buffer;
		uart_callback_t    UART_CallBack;
	public:
		UARTClass( Buffer * pRx_buffer );
		
		void begin(const uint32_t BaudRate );
		void begin(const uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin);
		void end(void);
		
		int available( void );
		int read( void );
		int peek( void );
		void flush( void );
		size_t write( const uint8_t c); //just keep the format same with the virtual function
		
		void irq_handler( void );
		
        void irq_attach( uart_callback_t UART_CallBackFunc );
        
#if defined __GNUC__ /* GCC CS3 */
		using Print::write ; // pull in write(str) and write(buf, size) from Print
#elif defined __ICCARM__ /* IAR Ewarm 5.41+ */
//    virtual void write( const char *str ) ;
//    virtual void write( const uint8_t *buffer, size_t size ) ;
#endif

		operator bool() { return true; }; // USART always active
};

#endif



