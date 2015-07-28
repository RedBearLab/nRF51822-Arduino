
#ifndef WIRING_SERIAL_H_
#define WIRING_SERIAL_H_

#include "HardwareSerial.h"

class UARTClass : public HardwareSerial
{
	public:
		UARTClass();
	
		void attach(uart_irq_handler handle);
		void begin(const uint32_t BaudRate );
		void begin(const uint32_t BaudRate, uint32_t rx_pin, uint32_t tx_pin);
		void end(void);
		
		int available( void );
		int read( void );
		int peek( void );
		void flush( void );
		size_t write( const uint8_t c); //just keep the format same with the virtual function

#if defined __GNUC__ /* GCC CS3 */
		using Print::write ; // pull in write(str) and write(buf, size) from Print
#elif defined __ICCARM__ /* IAR Ewarm 5.41+ */
		//virtual void write( const char *str ) ;
		//virtual void write( const uint8_t *buffer, size_t size ) ;
#endif
		operator bool() { return true; }; // USART always active	
};

extern "C" void store(uint8_t c);

#endif

