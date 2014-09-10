
#ifndef PINS_ARDUINO_H_
#define PINS_ARDUINO_H_


#define D0 1 //P0_09 RX
#define D1 0 //P0_11 TX
#define D2 3 //P0_08 CTS
#define D3 2 //P0_10 RTS
#define D4 24
#define D5 26
#define D6 13 
#define D7 14
#define D8 15
#define D9 8

#define A0 25 //P0_00
#define A1 19 //P0_01
#define A2 20 //P0_02
#define A3 21 //P0_03
#define A4 22 //P0_04
#define A5 23 //P0_05

#define CS   D3
#define SCK  D2
#define MISO D1
#define MOSI D0

#define SDA1  D3
#define SCL1  D2

#define DEFAULT_RX_PIN 11  //do not need to pin_transform, default is P0_11
#define DERAULT_TX_PIN 9   //do not need to pin_transform, default is P0_09

#define TWI_SDA D3
#define TWI_SCL D2

#define LED D9

#endif

