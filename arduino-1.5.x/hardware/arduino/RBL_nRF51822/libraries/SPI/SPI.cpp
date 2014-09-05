
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

#include "SPI.h"


static void SPI1_TWI1_handler( void );

/**********************************************************************
name :
function : 
**********************************************************************/
SPIClass::SPIClass(NRF_SPI_Type *_spi) : spi(_spi)
{
	//do nothing
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::setCPOL( bool active_low)
{
	if(active_low)
	{
		spi->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
	}
	else
	{
		spi->CONFIG |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::setCPHA( bool trailing)
{
	if(trailing)
	{
		spi->CONFIG |= (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos);
	}
	else
	{
		spi->CONFIG |= (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos);
	}

}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::setBitORDER( BitOrder  bit)
{
	if(bit == MSBFIRST)
	{
		spi->CONFIG |= (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos);
	}
	else
	{
		spi->CONFIG |= (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::setFrequency(uint8_t speed)
{
	if (speed == 0)
	{
		spi->FREQUENCY = SPI_FREQUENCY_125K;
	}
	else if (speed == 1)
	{
		spi->FREQUENCY = SPI_FREQUENCY_250K;
	}
	else if (speed == 2)
	{
		spi->FREQUENCY = SPI_FREQUENCY_500K;
	}
	else if (speed == 3)
	{
		spi->FREQUENCY = SPI_FREQUENCY_1M;
	}
	else if (speed == 4)
	{
		spi->FREQUENCY = SPI_FREQUENCY_2M;
	}
	else if (speed == 5)
	{
		spi->FREQUENCY = SPI_FREQUENCY_4M;
	}
	else if (speed == 6)
	{
		spi->FREQUENCY = SPI_FREQUENCY_8M;
	}
	else
	{
		spi->FREQUENCY = SPI_FREQUENCY_4M;
	}	
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::setSPIMode( uint8_t mode )
{
    if (SPI_MODE0 == mode)
    {
        setCPOL(0);
        setCPHA(0);		
    }
	else if (mode == SPI_MODE1)
    {
		setCPOL(0);
		setCPHA(1);
    }
    else if (mode == SPI_MODE2)
    {
		setCPOL(1);
		setCPHA(0);
    }
    else if (mode == SPI_MODE3)
    {
        setCPOL(1);
        setCPHA(1);
    }
    else
    {
        setCPOL(0);
        setCPHA(0);			
    }
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::begin(uint32_t sck, uint32_t mosi, uint32_t miso, uint8_t mode, uint8_t speed)
{
    uint32_t sck_p, mosi_p, miso_p;
	
    sck_p = Pin_nRF51822_to_Arduino(sck);
    mosi_p = Pin_nRF51822_to_Arduino(mosi);
    miso_p = Pin_nRF51822_to_Arduino(miso);

    if( sck_p<31 && mosi_p<31 && miso_p<31 )
    {
		SCK_Pin = sck_p;
		MOSI_Pin = mosi_p;
		MISO_Pin = miso_p;
		
		NRF_GPIO->PIN_CNF[SCK_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);	
										
		NRF_GPIO->PIN_CNF[MOSI_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
										| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
										| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
										| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
										| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
										
		NRF_GPIO->PIN_CNF[MISO_Pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
                                        | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                        | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                        | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                        | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);										
		spi->PSELSCK  = SCK_Pin;
		spi->PSELMOSI = MOSI_Pin;
		spi->PSELMISO = MISO_Pin;		
		
		setFrequency(speed);
		setSPIMode(mode);
		setBitORDER(MSBFIRST);
		
		spi->EVENTS_READY = 0;
		spi->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::begin()
{
	begin(SCK, MOSI, MISO, SPI_MODE0, SPI_4M);
}

/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t SPIClass::transfer(uint8_t data)
{
	spi->TXD = (uint32_t)data;
	
	while(spi->EVENTS_READY == 0);
	
	data = (uint8_t)spi->RXD;
	
	spi->EVENTS_READY = 0;
	
	return data;
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPIClass::endTransfer()
{
	spi->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);
}

/**********************************************************************
name :
function : 
**********************************************************************/

SPIClass SPI(NRF_SPI1);




/**********************************************************************
name :
function : 
**********************************************************************/
SPISlave::SPISlave()
{
	//do nothing
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::setCPOL( bool active_low)
{
	if(active_low)
	{
		NRF_SPIS1->CONFIG |= (SPI_CONFIG_CPOL_ActiveLow << SPI_CONFIG_CPOL_Pos);
	}
	else
	{
		NRF_SPIS1->CONFIG |= (SPI_CONFIG_CPOL_ActiveHigh << SPI_CONFIG_CPOL_Pos);
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::setCPHA( bool trailing)
{
	if(trailing)
	{
		NRF_SPIS1->CONFIG |= (SPI_CONFIG_CPHA_Trailing << SPI_CONFIG_CPHA_Pos);
	}
	else
	{
		NRF_SPIS1->CONFIG |= (SPI_CONFIG_CPHA_Leading << SPI_CONFIG_CPHA_Pos);
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::setBitORDER( BitOrder  bit)
{
	if(bit == MSBFIRST)
	{
		NRF_SPIS1->CONFIG |= (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos);
	}
	else
	{
		NRF_SPIS1->CONFIG |= (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::setSPIMode( uint8_t mode )
{
	if(SPI_MODE0 == mode)
	{
		setCPOL(0);
		setCPHA(0);		
	}
	else if (mode == SPI_MODE1)
    {
		setCPOL(0);
		setCPHA(1);
	}
    else if (mode == SPI_MODE2)
    {
		setCPOL(1);
		setCPHA(0);
	}
    else if (mode == SPI_MODE3)
    {
		setCPOL(1);
		setCPHA(1);
	}
	else
	{
		setCPOL(0);
		setCPHA(0);			
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t SPISlave::spi_slave_evt_handler_register(spi_slave_event_handler_t event_handler)
{
	m_event_callback = event_handler;
	
	return ( m_event_callback != 0) ? 1 : 0;
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::begin(uint32_t cs, uint32_t sck, uint32_t mosi, uint32_t miso, uint8_t mode, spi_slave_event_handler_t event)
{	
	uint32_t cs_p, sck_p, mosi_p, miso_p;

	cs_p   = Pin_nRF51822_to_Arduino(cs);
	sck_p  = Pin_nRF51822_to_Arduino(sck);
	mosi_p = Pin_nRF51822_to_Arduino(mosi);
	miso_p = Pin_nRF51822_to_Arduino(miso);
	
	if( cs_p<31 && sck_p<31 && mosi_p<31 && miso_p<31 )
	{	
		spi_slave_evt_handler_register(event);
		
		CS_Pin = cs_p;
		SCK_Pin = sck_p;
		MOSI_Pin = mosi_p;
		MISO_Pin = miso_p;
		
		NRF_GPIO->PIN_CNF[CS_Pin] = 
			(GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
			(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)     |
			(GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)   |
			(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  |
			(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);		
	
	    NRF_GPIO->PIN_CNF[SCK_Pin] = 
			(GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
			(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)     |
			(GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)   |
			(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  |
			(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
	
		NRF_GPIO->PIN_CNF[MOSI_Pin] = 
			(GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
			(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)     |
			(GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)   |
			(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  |
			(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);	
	
		NRF_GPIO->PIN_CNF[MISO_Pin] = 
			(GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) |
			(GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)     |
			(GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)   |
			(GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)  |
			(GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);	
	
		NRF_SPIS1->PSELCSN  = CS_Pin;
		NRF_SPIS1->PSELSCK  = SCK_Pin;
		NRF_SPIS1->PSELMOSI = MOSI_Pin;
		NRF_SPIS1->PSELMISO = MISO_Pin;
		NRF_SPIS1->MAXRX    = 0;
		NRF_SPIS1->MAXTX    = 0;

		setSPIMode(mode);
		setBitORDER(LSBFIRST);
		
        NRF_SPIS1->DEF    = 0xAAu;
        NRF_SPIS1->ORC    = 0x55u;		
		
		NRF_SPIS1->EVENTS_END	   = 0;
		NRF_SPIS1->EVENTS_ACQUIRED = 0;

		// Enable END_ACQUIRE shortcut.        
		NRF_SPIS1->SHORTS = (SPIS_SHORTS_END_ACQUIRE_Enabled << SPIS_SHORTS_END_ACQUIRE_Pos);	
		//update the SPI state
		SPI_State = SPI_STATE_INIT;
		// Set correct IRQ priority and clear any possible pending interrupt.
		NVIC_SetPriority(SPI1_TWI1_IRQn, 3);    
		NVIC_ClearPendingIRQ(SPI1_TWI1_IRQn);
		
	    // Enable IRQ.    
		NRF_SPIS1->INTENSET = (SPIS_INTENSET_ACQUIRED_Enabled << SPIS_INTENSET_ACQUIRED_Pos) | (SPIS_INTENSET_END_Enabled << SPIS_INTENSET_END_Pos);
		
		LinkInterrupt(SPI1_TWI1_IRQn, SPI1_TWI1_handler);
		NVIC_EnableIRQ(SPI1_TWI1_IRQn);
		// Enable SPI slave device.        
		NRF_SPIS1->ENABLE = (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);	
		//simple_uart_putstring("begin2... \r\n");
	}

}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::begin(spi_slave_event_handler_t event)
{
	begin(CS, SCK, MOSI, MISO, SPI_MODE0, event);	
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::state_change(spi_state_t new_state)
{
	spi_slave_evt_t event;
	
	SPI_State = new_state;
	
	switch(SPI_State)
	{
		case SPI_BUFFER_RESOURCE_REQUESTED:
            NRF_SPIS1->TASKS_ACQUIRE = 1u;   
			//simple_uart_putstring("state1... \r\n");         			
            break;            
     
        case SPI_BUFFER_RESOURCE_CONFIGURED:
            event.evt_type  = SPI_SLAVE_BUFFERS_SET_DONE;
            event.rx_amount = 0;
            event.tx_amount = 0;     
            //simple_uart_putstring("state2... \r\n");         
            //APP_ERROR_CHECK_BOOL(m_event_callback != NULL);
            m_event_callback(event);         
            break;
            
        case SPI_XFER_COMPLETED:        
            event.evt_type  = SPI_SLAVE_XFER_DONE;
            event.rx_amount = NRF_SPIS1->AMOUNTRX;
            event.tx_amount = NRF_SPIS1->AMOUNTTX;    
            //simple_uart_putstring("state3... \r\n");         
            //APP_ERROR_CHECK_BOOL(m_event_callback != NULL);
            m_event_callback(event);
            break;
            
        default:
            // No implementation required.            
            break;		
	
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t SPISlave::spi_slave_buffers_set(uint8_t *p_tx_buf, uint8_t *p_rx_buf, uint8_t tx_len, uint8_t rx_len)
{	
	uint8_t SPISlave_error;
	
	if( p_tx_buf == NULL || p_rx_buf == NULL )
	{
		return 0;
	}
	switch(SPI_State)
	{   
		case SPI_STATE_INIT:
        case SPI_XFER_COMPLETED:
        case SPI_BUFFER_RESOURCE_CONFIGURED:        
            spi_tx_buf     = p_tx_buf;
            spi_rx_buf     = p_rx_buf;
            spi_tx_buf_size = tx_len;
            spi_rx_buf_size = rx_len;        
            SPISlave_error    = 1;            
            //simple_uart_putstring("bufset1... \r\n");            
            state_change(SPI_BUFFER_RESOURCE_REQUESTED);             
            break;

        case SPI_BUFFER_RESOURCE_REQUESTED:
            SPISlave_error = 0; 
            break;
                        
        default:
            // @note: execution of this code path would imply internal error in the design.
            SPISlave_error = 0;             
            break;		
	}
	return SPISlave_error;
}

/**********************************************************************
name :
function : 
**********************************************************************/
void SPISlave::irq_handler()
{   
	if( NRF_SPIS1->EVENTS_ACQUIRED != 0 )
	{
		NRF_SPIS1->EVENTS_ACQUIRED = 0;
		switch(SPI_State)
		{	
			case SPI_BUFFER_RESOURCE_REQUESTED: 
                NRF_SPIS1->TXDPTR = (uint32_t)spi_tx_buf;
                NRF_SPIS1->RXDPTR = (uint32_t)spi_rx_buf;
                NRF_SPIS1->MAXRX  = spi_rx_buf_size;
                NRF_SPIS1->MAXTX  = spi_tx_buf_size;
                
                NRF_SPIS1->TASKS_RELEASE = 1u;
                
                state_change(SPI_BUFFER_RESOURCE_CONFIGURED);  				
                break;
                
            default:
                // No implementation required.                    
                break;		
		}
	}
	// Check for SPI transaction complete event.
    if (NRF_SPIS1->EVENTS_END != 0)
    {	
        NRF_SPIS1->EVENTS_END = 0;            
        switch (SPI_State)
        {
            case SPI_BUFFER_RESOURCE_CONFIGURED: 
					
                state_change(SPI_XFER_COMPLETED);                                                             
                break;

            default:
                // No implementation required.                    
                break;                
        }    
    }    		
}

SPISlave SPIS1;

/**********************************************************************
name :
function : 
**********************************************************************/
//void SPI1_TWI1_IRQHandler(void)
static void SPI1_TWI1_handler( void )
{	
	SPIS1.irq_handler();
}




