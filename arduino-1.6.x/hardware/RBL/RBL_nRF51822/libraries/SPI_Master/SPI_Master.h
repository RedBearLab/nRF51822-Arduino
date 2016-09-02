
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

#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

#include "Arduino.h"

#define SPI_125K    125000
#define SPI_250K    250000
#define SPI_500K    500000
#define SPI_1M      1000000
#define SPI_2M      2000000
#define SPI_4M      4000000
#define SPI_8M      8000000

#define SPI_MODE0   0
#define SPI_MODE1   1
#define SPI_MODE2   2
#define SPI_MODE3   3


class SPIClass
{
public:
    SPIClass();

    void begin();
    void begin(uint32_t scl, uint32_t mosi, uint32_t miso);
    uint8_t transfer(uint8_t data);
    void endTransfer(void);

    /*This also set bitOrder to MSBFIRST, so don't use setBitORDER befor this */
    void setSPIMode(uint8_t mode);
    void setFrequency(uint32_t speed );
    /*Set bitOrder, this must be after setSPIMode */
    void setBitORDER( BitOrder  bit);

protected:

    spi_t    spi;
};

extern SPIClass     SPI_Master;

#endif
