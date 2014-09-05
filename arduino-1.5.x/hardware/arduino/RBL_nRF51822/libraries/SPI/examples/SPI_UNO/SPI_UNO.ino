#include <SPI.h>

void setup()
{    
    //pinMode(SS, OUTPUT);
    
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(LSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV32);
}

void loop()
{  
    digitalWrite(10, LOW);
    delay(1);
    SPI.transfer(0x01);
    SPI.transfer(0x02);
    SPI.transfer(0x03);
    delay(1);
    digitalWrite(10, HIGH);
    delay(1000);
}
