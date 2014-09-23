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
#include "Arduino.h"

#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t Pin_nRF51822_to_Arduino(uint32_t pin)
{
	uint32_t return_pin=0x000000FF;
	
#ifdef RBL_NRF51822	
	switch(pin)
	{	

		//20140821
		case 0 : return_pin = 11;break;//D0/RX
		case 1 : return_pin = 9;break;//D1/TX
		case 2 : return_pin = 10;break;//D2/SDA0
		case 3 : return_pin = 8;break;//D3/SCL0
		case 4 : return_pin = 21;break;//D4
		case 5 : return_pin = 23;break;//D5
		case 6 : return_pin = 16;break;//D6
		case 7 : return_pin = 17;break;//D7
		//J10
		case 8 : return_pin = 19;break;//D8
		case 9 : return_pin = 18;break;//D9
		case 10 : return_pin = 14;break;//D10
		case 11 : return_pin = 12;break;//D11
		case 12 : return_pin = 13;break;//D12
		case 13 : return_pin = 15;break;//D13/LED
		
		case 14 : return_pin = 29;break;//SDA1
		case 15 : return_pin = 28;break;//SCL1
		//SPI
		case 16 : return_pin = 25;break;//SCK
		case 17 : return_pin = 22;break;//MISO
		case 18 : return_pin = 20;break;//MOSI
		//J8
		case 19 : return_pin = 1;break;//A0
		case 20 : return_pin = 2;break;//A1
		case 21 : return_pin = 3;break;//A2
		case 22 : return_pin = 4;break;//A3
		case 23 : return_pin = 5;break;//A4
		case 24 : return_pin = 6;break;//A5	
		
		default : break;		
	}
	
#endif

#ifdef BLE_NANO
	switch(pin)
	{	
		case 0 : return_pin = 9;break;		//D0/TXD/MOSI
		case 1 : return_pin = 11;break;		//D1/RXD/MISO
		case 2 : return_pin = 8;break;		//D2/RTS/SCK/SCL
		case 3 : return_pin = 10;break;		//D3/CTS/CS/SDA
		case 5 : return_pin = 7;break;		//D5
		case 6 : return_pin = 15;break;		//D6
		case 7 : return_pin = 29;break;		//D7
		case 8 : return_pin = 28;break;		//D8

		case 13 : return_pin = 19;break;	//D13/LED
		
		case 14 : return_pin = 1;break;	//A0
		case 15 : return_pin = 2;break;	//A1
		case 16 : return_pin = 3;break;	//A2
		case 17 : return_pin = 4;break;	//A3
		case 18 : return_pin = 5;break;	//A4
		case 19 : return_pin = 6;break;	//A5

		default : break;		
	}
#endif

	return (return_pin);
}

/**********************************************************************
name :
function : 
**********************************************************************/
void LinkInterrupt( uint8_t IRQn, dynamic_handler_t handler)
{
	dynamic_handlers[IRQn] = handler;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void UnlinkInterrupt( uint8_t IRQn )
{
	dynamic_handlers[IRQn] = NULL;
}

#ifdef __cplusplus
}
#endif



















