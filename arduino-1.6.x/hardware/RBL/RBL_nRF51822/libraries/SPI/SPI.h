
/*

    Copyright (c) 2014 RedBearLab, All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef _SPI_H_
#define _SPI_H_

#include "Arduino.h"

#define SPI_FREQUENCY_125K 0x02000000UL
#define SPI_FREQUENCY_250K 0x04000000UL
#define SPI_FREQUENCY_500K 0x08000000UL
#define SPI_FREQUENCY_1M   0x10000000UL
#define SPI_FREQUENCY_2M   0x20000000UL
#define SPI_FREQUENCY_4M   0x40000000UL
#define SPI_FREQUENCY_8M   0x80000000UL

#define SPI_125K 0
#define SPI_250K 1
#define SPI_500K 2
#define SPI_1M 	 3
#define SPI_2M 	 4
#define SPI_4M   5
#define SPI_8M   6

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

typedef enum
{
    SPI_SLAVE_BUFFERS_SET_DONE,             /**< Memory buffer set event. Memory buffers have been set successfully to the SPI slave device and SPI transactions can be done. */
    SPI_SLAVE_XFER_DONE,                    /**< SPI transaction event. SPI transaction has been completed. */  
    SPI_SLAVE_EVT_TYPE_MAX                  /**< Enumeration upper bound. */      
} spi_slave_evt_type_t;

typedef enum
{
    SPI_STATE_INIT,                                 /**< Initialization state. In this state the module waits for a call to @ref spi_slave_buffers_set. */                                                                                             
    SPI_BUFFER_RESOURCE_REQUESTED,                  /**< State where the configuration of the memory buffers, which are to be used in SPI transaction, has started. */
    SPI_BUFFER_RESOURCE_CONFIGURED,                 /**< State where the configuration of the memory buffers, which are to be used in SPI transaction, has completed. */
    SPI_XFER_COMPLETED                              /**< State where SPI transaction has been completed. */
} spi_state_t;

typedef struct
{
    spi_slave_evt_type_t evt_type;          /**< Type of event. */    
    uint32_t             rx_amount;         /**< Number of bytes received in last transaction (parameter is only valid upon @ref SPI_SLAVE_XFER_DONE event). */
    uint32_t             tx_amount;         /**< Number of bytes transmitted in last transaction (parameter is only valid upon @ref SPI_SLAVE_XFER_DONE event). */    
} spi_slave_evt_t;

typedef void (*spi_slave_event_handler_t)(spi_slave_evt_t event);




class SPIClass
{
	public:
		SPIClass(NRF_SPI_Type *_spi);
	
		void begin();
		void begin(uint32_t sck, uint32_t mosi, uint32_t miso);
		uint8_t transfer(uint8_t data);
		void endTransfer();
			
		void setSPIMode( uint8_t mode );
		void setFrequency(uint8_t speed );
		void setBitORDER( BitOrder  bit);
		void setCPHA( bool trailing);
		void setCPOL( bool active_low);
		
		
	private:
		NRF_SPI_Type *spi;
		
		uint32_t SCK_Pin;
		uint32_t MOSI_Pin;
		uint32_t MISO_Pin;

};


class SPISlave
{
	public : 
		SPISlave();
		void begin(spi_slave_event_handler_t event);
		void begin(uint32_t cs, uint32_t sck, uint32_t mosi, uint32_t miso, spi_slave_event_handler_t event);
		void state_change(spi_state_t new_state);
		uint32_t spi_slave_evt_handler_register(spi_slave_event_handler_t event_handler);
		uint8_t spi_slave_buffers_set(uint8_t *p_tx_buf, uint8_t *p_rx_buf, uint8_t tx_len, uint8_t rx_len);
		void irq_handler();
		
		void setCPOL( bool active_low);
		void setCPHA( bool trailing);
		void setBitORDER( BitOrder  bit);
		void setSPIMode( uint8_t mode );
		
	private : 
		spi_state_t SPI_State;
	
		uint32_t CS_Pin;
		uint32_t SCK_Pin;
		uint32_t MOSI_Pin;
		uint32_t MISO_Pin;
		
		uint8_t *spi_tx_buf;
		uint8_t *spi_rx_buf;
		uint32_t spi_tx_buf_size;
		uint32_t spi_rx_buf_size;
		
		spi_slave_event_handler_t m_event_callback;		
};

extern SPIClass SPI;
extern SPISlave SPIS1;
#endif
