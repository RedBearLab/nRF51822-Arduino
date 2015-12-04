
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

#include "Wire.h"


/**********************************************************************
name :
function : 
**********************************************************************/
TwoWire::TwoWire() : 
					RX_BufferHead(0), RX_BufferTail(0),
					TX_BufferHead(0), TX_BufferTail(0), 
					Transfer_Addr(0),
					i2c(),
					twi_status(UNINITIALIZED)
{
	//do nothing
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::begin(uint32_t scl, uint32_t sda, uint32_t speed)
{	
	PinName nrf_sda, nrf_scl;
	
	nrf_scl = Pin_nRF51822_to_Arduino(scl);
	nrf_sda = Pin_nRF51822_to_Arduino(sda);
	
	/* Initialize i2c */
	i2c_init(&i2c, nrf_sda, nrf_scl);
	i2c_frequency(&i2c, speed);
	twi_status = UNINITIALIZED;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::begin()
{
	PinName nrf_sda, nrf_scl;
	
	nrf_scl = Pin_nRF51822_to_Arduino(TWI_SCL);
	nrf_sda = Pin_nRF51822_to_Arduino(TWI_SDA);	
	/* Initialize i2c, default 100KHz */
	i2c_init(&i2c, nrf_sda, nrf_scl);
	twi_status = UNINITIALIZED;
}
/**********************************************************************
name :
function : 
note : address has been shift right one bit, so need to shift left one bit
**********************************************************************/
void TwoWire::beginTransmission( uint8_t address )
{
	Transfer_Addr = (address << 1);
	TX_BufferTail = 0;
	TX_BufferHead = 0;
	twi_status = MASTER_SEND;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::beginTransmission( int address )
{
	beginTransmission( (uint8_t)address );
}
/**********************************************************************
name :
function : 
return : 0 success, -1 no slave, -2 bus busy
**********************************************************************/
int8_t TwoWire::endTransmission( uint8_t stop)
{
	int8_t result=0;
	
	result = i2c_write(&i2c, Transfer_Addr, (const char *)TX_Buffer, TX_BufferHead, stop);
	
    TX_BufferHead = 0;
	twi_status = MASTER_IDLE;	
    
    if(i2c.i2c->EVENTS_ERROR == 1) {
        if (i2c.i2c->ERRORSRC & TWI_ERRORSRC_ANACK_Msk) {
            i2c.i2c->EVENTS_ERROR = 0;
            i2c.i2c->TASKS_STOP   = 1;
            result = I2C_ERROR_NO_SLAVE;
        }
        else if(i2c.i2c->ERRORSRC & TWI_ERRORSRC_DNACK_Msk) {
            i2c.i2c->EVENTS_ERROR = 0;
            i2c.i2c->TASKS_STOP   = 1;
            result = I2C_ERROR_BUS_BUSY;
        }
    }
    
    if(stop)
        i2c_reset(&i2c);

    return result;
}
/**********************************************************************
name :
function : 
**********************************************************************/
int8_t TwoWire::endTransmission(void)
{
	int8_t result;
	
	result = endTransmission(1);
	
	return result;
}
/**********************************************************************
name :
function :
return :   0:full or error status
		   1:success
**********************************************************************/
size_t TwoWire::write(uint8_t data)
{
	if(twi_status == MASTER_SEND) {	
		if(TX_BufferHead >= BUFF_MAX_LENGTH) {
			return 1;
		}
		TX_Buffer[TX_BufferHead++] = data;
		return 0;
	}
	else {
		return 1;
	}
}
/**********************************************************************
name :
function : 
return :   0		:	full or error status
		   other	:	the true size of write 
**********************************************************************/
size_t TwoWire::write(const uint8_t *data, size_t quantity )
{
	size_t index;
	
	if( twi_status == MASTER_SEND ) {
		for( index=0; index<quantity; ++index ) {	
			if(TX_BufferHead >= BUFF_MAX_LENGTH) {
				return index;
			}
			TX_Buffer[TX_BufferHead++] = data[index];
		}
	}
	else {
		return 0;
	}
	
	return quantity;
}
/**********************************************************************
name :
function : 
return :   0		:	error
		   other	:	the true size of read 
**********************************************************************/
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t quantity, uint8_t stop)
{
	uint8_t result;
	
	Transfer_Addr = (addr << 1);
	if(quantity > BUFF_MAX_LENGTH) {   
		quantity = BUFF_MAX_LENGTH;
	}
	if(quantity > 0 ) {   
		result = i2c_read(&i2c, Transfer_Addr, (char *)RX_Buffer, quantity, stop);
	}
	RX_BufferTail = 0;
	RX_BufferHead = result;
	
	return result;
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t TwoWire::requestFrom(uint8_t address, uint8_t quantity)
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t TwoWire::requestFrom(int address, int quantity) 
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) true);
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint8_t TwoWire::requestFrom(int address, int quantity, int sendStop) 
{
	return requestFrom((uint8_t) address, (uint8_t) quantity, (uint8_t) sendStop);
}
/**********************************************************************
name :
function : 
**********************************************************************/
int TwoWire::available(void)
{
	if(RX_BufferTail <= RX_BufferHead) {
		return (RX_BufferHead - RX_BufferTail);
	}
	else {
		return 0;
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
int TwoWire::read(void)
{
	if(RX_BufferTail < RX_BufferHead) {
		return RX_Buffer[RX_BufferTail++];
	}
	else {
		return -1;
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
int TwoWire::peek(void)
{
	if(RX_BufferTail < RX_BufferHead) {
		return RX_Buffer[RX_BufferTail];
	}
	else {
		return -1;
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void TwoWire::flush(void)
{
	//do nothing
}


TwoWire Wire;







