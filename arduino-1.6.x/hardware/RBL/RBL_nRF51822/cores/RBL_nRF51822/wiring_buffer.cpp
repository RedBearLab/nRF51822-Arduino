
#include "Arduino.h"
#include "wiring_buffer.h"


/**********************************************************************
name :
function : 
**********************************************************************/
Buffer::Buffer( void )
{
	memset(RX_buff, 0, SERIAL_BUFFER_MAX_SIZE);
	rx_Head = 0;
	rx_Tail = 0;
}

/**********************************************************************
name :
function : 
**********************************************************************/
void Buffer::store_char( uint8_t c)
{
	uint8_t i = ( uint8_t )( rx_Head +1 ) % SERIAL_BUFFER_MAX_SIZE;
	
	if( i != rx_Tail )
	{
		RX_buff[rx_Head] = c;
		rx_Head = i;
	}
}
