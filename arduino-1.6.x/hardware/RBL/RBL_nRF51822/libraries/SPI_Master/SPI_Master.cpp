
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

#include "SPI_Master.h"

static void spi_release(spi_t *obj, int slave)
{
    if (slave) {
        obj->spis->ENABLE = (SPIS_ENABLE_ENABLE_Disabled << SPIS_ENABLE_ENABLE_Pos);
    } else {
        obj->spi->ENABLE = (SPI_ENABLE_ENABLE_Disabled << SPI_ENABLE_ENABLE_Pos);
    }	
}

static void spi_start(spi_t *obj, int slave)
{
    if (slave) {
        obj->spis->ENABLE = (SPIS_ENABLE_ENABLE_Enabled << SPIS_ENABLE_ENABLE_Pos);
    } else {
        obj->spi->ENABLE = (SPI_ENABLE_ENABLE_Enabled << SPI_ENABLE_ENABLE_Pos);
    }
}

static void spi_BitOrder(spi_t *obj, BitOrder bit)
{
	//spi_release(obj, 0);
	if(bit == MSBFIRST)
		obj->spi->CONFIG |= (SPI_CONFIG_ORDER_MsbFirst << SPI_CONFIG_ORDER_Pos); 
	else
		obj->spi->CONFIG |= (SPI_CONFIG_ORDER_LsbFirst << SPI_CONFIG_ORDER_Pos);
	//spi_start(obj, 0);
}

SPIClass::SPIClass()
{
	//do nothing
}

void SPIClass::begin(uint32_t scl, uint32_t mosi, uint32_t miso)
{
	PinName nrf_scl, nrf_mosi, nrf_miso;
	
	nrf_scl  = Pin_nRF51822_to_Arduino(scl);
	nrf_mosi = Pin_nRF51822_to_Arduino(mosi);
	nrf_miso = Pin_nRF51822_to_Arduino(miso);

	spi_init(&spi, nrf_mosi, nrf_miso, nrf_scl, (PinName)NC);
	/* 8-bits, MODE0, master(0)/slave(1) */
	spi_format(&spi, 8, SPI_MODE0, 0);
	/* Default speed : 1MHz */
	spi_frequency(&spi, SPI_1M);
}

void SPIClass::begin()
{
	begin(SCK, MOSI, MISO);	
}

uint8_t SPIClass::transfer(uint8_t data)
{
	return ((uint8_t)spi_master_write(&spi, data));
}

void SPIClass::endTransfer(void)
{
	spi_release(&spi, 0);	
}

void SPIClass::setSPIMode(uint8_t mode)
{
	if(mode > 3)
		return;
	/* 8-bits, MODE0, master(0)/slave(1) */
	spi_format(&spi, 8, mode, 0);	
}

void SPIClass::setFrequency(uint32_t speed )
{	
	/* Default speed : 1MHz */
	spi_frequency(&spi, speed);	
}

void SPIClass::setBitORDER( BitOrder  bit)
{
	spi_BitOrder(&spi, bit);
}


SPIClass	SPI_Master;












