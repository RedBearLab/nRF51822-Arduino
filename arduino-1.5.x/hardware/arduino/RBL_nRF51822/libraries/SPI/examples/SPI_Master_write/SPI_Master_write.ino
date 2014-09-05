
#include <SPI.h>

void setup()
{
    // put your setup code here, to run once:
    pinMode(7, OUTPUT);
    digitalWrite(7, HIGH);
    delay(500);
    SPI.begin(); 
}



void loop() 
{
	digitalWrite(7, LOW);
    SPI.transfer(0x53);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    SPI.transfer(0x00);
    digitalWrite(7, HIGH);
    delay(1000);   
}
