
#include <SPI.h>

#define TX_BUF_SIZE   3                  /**< SPI TX buffer size. */      
#define RX_BUF_SIZE   TX_BUF_SIZE        /**< SPI RX buffer size. */  

uint8_t m_tx_buf[TX_BUF_SIZE] = {0x01, 0x03, 0x09};
uint8_t m_rx_buf[RX_BUF_SIZE];

void spi_slave_event_handle(spi_slave_evt_t event)
{   
    if (event.evt_type == SPI_SLAVE_XFER_DONE)
    {   
        Serial.write(m_rx_buf[0]);
        Serial.write(m_rx_buf[1]);
        Serial.write(m_rx_buf[2]);
        SPIS1.spi_slave_buffers_set(m_tx_buf, m_rx_buf, sizeof(m_tx_buf), sizeof(m_rx_buf));
    }
} 

void setup() 
{    
    Serial.begin(115200);
    Serial.println("start...");
    
    SPIS1.begin(spi_slave_event_handle);
    delay(1000);
    SPIS1.spi_slave_buffers_set(m_tx_buf, m_rx_buf, sizeof(m_tx_buf), sizeof(m_rx_buf));
}

void loop() 
{
    //wait for interrupt
}
