
/*

Flash chip: 45DB021D

*/

#include <SPI.h>


void Flash_Buff_WriteBytes(uint16_t addr, uint8_t *pbuf, uint16_t len)
{    
    uint16_t index;
#ifdef RBL_NRF51822	
    digitalWrite(10, LOW);
    delayMicroseconds(200);
#endif
#ifdef BLE_NANO
    digitalWrite(D7, LOW);
    delayMicroseconds(200);
#endif
    SPI.transfer(0x84);
    SPI.transfer(0x00);
    SPI.transfer( (uint8_t)(addr>>8) );
    SPI.transfer( (uint8_t)addr );
    for(index=0; index<len; index++)
    {
        SPI.transfer(*pbuf);
        pbuf++;
    }
#ifdef RBL_NRF51822
    delayMicroseconds(200);
    digitalWrite(10, HIGH);
#endif
#ifdef BLE_NANO
    delayMicroseconds(200);
    digitalWrite(D7, HIGH);
#endif
}
void Flash_Buff_ReadBytes(uint16_t addr, uint8_t *pbuf, uint16_t len)
{
    uint16_t index;
#ifdef RBL_NRF51822
    digitalWrite(10, LOW);
    delayMicroseconds(200); 
#endif
#ifdef BLE_NANO
    digitalWrite(D7, LOW);
    delayMicroseconds(200); 
#endif
    SPI.transfer(0xD1);
    SPI.transfer(0x00);  
    SPI.transfer( (uint8_t)(addr>>8) );
    SPI.transfer( (uint8_t)addr );    
    for(index=0; index<len; index++)
    {
        *pbuf = SPI.transfer(0x00);
        pbuf++;
    }   
#ifdef RBL_NRF51822 
    delayMicroseconds(200);
    digitalWrite(10, HIGH);   
#endif
#ifdef BLE_NANO 
    delayMicroseconds(200);
    digitalWrite(D7, HIGH);
#endif
}

uint8_t i;
uint8_t wt_buf[7] = {'H','e','l','l', 'o', '\r', '\n'};
uint8_t rd_buf[7];

void setup()
{
    delay(2000);
#ifdef RBL_NRF51822	
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    delay(500);
    //SPI.begin(); 
    SPI.begin(13, 11, 12);//SCK, MOSI, MISO
    SPI.setFrequency( SPI_4M );
#endif
#ifdef BLE_NANO
    pinMode(D7, OUTPUT);
    digitalWrite(D7, HIGH);
    delay(500);
    SPI.begin(A5, D6, D5);//SCK, MOSI, MISO
    SPI.setFrequency( SPI_4M );
    //pinMode(A5, OUTPUT);
    //pinMode(D6, OUTPUT);
    //pinMode(D5, OUTPUT);
#endif
    Serial.begin(115200);
    Serial.println("SPI Demo Start");
    delay(1000);
    Flash_Buff_WriteBytes(0, wt_buf, 7);
}

void loop() 
{  
    memset(rd_buf, 0x00, 7);
    Flash_Buff_ReadBytes(0, rd_buf, 7);
    for(i=0; i<7; i++)
      Serial.write(rd_buf[i]);
    delay(3000);   
}
