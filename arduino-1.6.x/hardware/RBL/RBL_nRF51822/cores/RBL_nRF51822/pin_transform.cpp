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
#include "pin_transform.h"

/**********************************************************************
name :
function : 
**********************************************************************/
PinName Pin_nRF51822_to_Arduino(uint32_t pin)
{
	PinName return_pin = (PinName)NC;
	
#ifdef RBL_NRF51822	
	switch(pin)
	{	
		//20150728
		case 0 : return_pin = P0_11;break;//D0/RX
		case 1 : return_pin = P0_9;break;//D1/TX
		case 2 : return_pin = P0_10;break;//D2/SDA0
		case 3 : return_pin = P0_8;break;//D3/SCL0
		case 4 : return_pin = P0_21;break;//D4
		case 5 : return_pin = P0_23;break;//D5
		case 6 : return_pin = P0_16;break;//D6
		case 7 : return_pin = P0_17;break;//D7
		//J10
		case 8 : return_pin = P0_19;break;//D8
		case 9 : return_pin = P0_18;break;//D9
		case 10 : return_pin = P0_14;break;//D10
		case 11 : return_pin = P0_12;break;//D11
		case 12 : return_pin = P0_13;break;//D12
		case 13 : return_pin = P0_15;break;//D13/LED
		
		case 14 : return_pin = P0_29;break;//SDA1
		case 15 : return_pin = P0_28;break;//SCL1
		//SPI
		case 16 : return_pin = P0_25;break;//SCK
		case 17 : return_pin = P0_22;break;//MISO
		case 18 : return_pin = P0_20;break;//MOSI
		//J8
		case 19 : return_pin = P0_1;break;//A0
		case 20 : return_pin = P0_2;break;//A1
		case 21 : return_pin = P0_3;break;//A2
		case 22 : return_pin = P0_4;break;//A3
		case 23 : return_pin = P0_5;break;//A4
		case 24 : return_pin = P0_6;break;//A5	
		
		default : return_pin = (PinName)NC;break;		
	}
	
#endif

#ifdef BLE_NANO
	switch(pin)
	{	
		case 0 : return_pin = P0_11;break;		//D0/RXD/MISO
		case 1 : return_pin = P0_9;break;		//D1/TXD/MOSI
		case 2 : return_pin = P0_10;break;		//D2/CTS/CS/SDA
		case 3 : return_pin = P0_8;break;		//D3/RTS/SCK/SCL
		
		case 4 : return_pin = P0_28;break;		//D4
		case 5 : return_pin = P0_29;break;		//D5
		case 6 : return_pin = P0_15;break;		//D6
		case 7 : return_pin = P0_7;break;		//D7

		case 13 : return_pin = P0_19;break;		//D13/LED
		
		case 14 : return_pin = P0_1;break;		//A0
		case 15 : return_pin = P0_2;break;		//A1
		case 16 : return_pin = P0_3;break;		//A2
		case 17 : return_pin = P0_4;break;		//A3
		case 18 : return_pin = P0_5;break;		//A4
		case 19 : return_pin = P0_6;break;		//A5

		default : return_pin = (PinName)NC;break;		
	}
#endif

	return (return_pin);
}




















