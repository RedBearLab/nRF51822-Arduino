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

#include "interrupt.h"

dynamic_handler_t pin_interrupterCB[4];

static uint8_t GPIOTE_Channel_for_Inter[4] = { 255,255,255,255};
static uint32_t pin_for_inter[4] =  {0xff, 0xff, 0xff, 0xff};
static uint8_t inter_mode[4] = {0x00,0x00,0x00,0x00};

static void GPIOTE_handler( void );

/**********************************************************************
name :
function : 
**********************************************************************/
void delay_ex_interrupter(uint32_t us) 
{
	while (us--)
    {
        __ASM(" NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t"
        " NOP\n\t");
    };
}

/**********************************************************************
name :
function : 
**********************************************************************/
void attachInterrupt(uint8_t pin, dynamic_handler_t event_handler, uint32_t mode)
{
	uint32_t nrf_pin;
	uint8_t channel;
	
	channel = GPIOTE_Channel_Find();
	if(channel == 255)
	{
		return;
	}
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
    //simple_uart_putstring("here1 \r\n");
	if( nrf_pin < 31 &&  (mode ==RISING || mode == FALLING) && channel<4 )
	{	
		GPIOTE_Channel_Set(channel);
		
		if(channel == 0)
		{	
			pin_interrupterCB[0] = event_handler;
			pin_for_inter[0] = nrf_pin;
			if( mode == RISING )
			{	
				inter_mode[0] = 0x01;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[0] =  (NRF_GPIOTE_POLARITY_LOTOHI << GPIOTE_CONFIG_POLARITY_Pos)
										| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
										| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);							
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos; 
			}
			else if( mode == FALLING )
			{	
				inter_mode[0] = 0x02;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[0] =  (NRF_GPIOTE_POLARITY_HITOLO << GPIOTE_CONFIG_POLARITY_Pos)
										| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
										| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);							
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos; 				
			}
		}
		else if(channel == 1)
		{
			pin_interrupterCB[1] = event_handler;
			pin_for_inter[1] = nrf_pin;
			if( mode == RISING )
			{
				inter_mode[1] = 0x01;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[1] =  (NRF_GPIOTE_POLARITY_LOTOHI << GPIOTE_CONFIG_POLARITY_Pos)
											| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
											| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);	
											
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN1_Set << GPIOTE_INTENSET_IN1_Pos;
			}
			else if( mode == FALLING )
			{
				inter_mode[1] = 0x02;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[1] =  (NRF_GPIOTE_POLARITY_HITOLO << GPIOTE_CONFIG_POLARITY_Pos)
										| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
										| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);							
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN1_Set << GPIOTE_INTENSET_IN1_Pos; 				
			}					
		}
		else if(channel == 2)
		{
			pin_interrupterCB[2] = event_handler;
			pin_for_inter[2] = nrf_pin;
			if( mode == RISING )
			{
				inter_mode[2] = 0x01;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[2] =  (NRF_GPIOTE_POLARITY_LOTOHI << GPIOTE_CONFIG_POLARITY_Pos)
											| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
											| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);	
											
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN2_Set << GPIOTE_INTENSET_IN2_Pos;
			}
			else if( mode == FALLING )
			{
				inter_mode[2] = 0x02;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[2] =  (NRF_GPIOTE_POLARITY_HITOLO << GPIOTE_CONFIG_POLARITY_Pos)
										| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
										| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);							
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN2_Set << GPIOTE_INTENSET_IN2_Pos; 				
			}
		}
		else if(channel == 3)
		{
			pin_interrupterCB[3] = event_handler;
			pin_for_inter[3] = nrf_pin;
			if( mode == RISING )
			{
				inter_mode[3] = 0x01;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[3] =  (NRF_GPIOTE_POLARITY_LOTOHI << GPIOTE_CONFIG_POLARITY_Pos)
											| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
											| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);	
											
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN3_Set << GPIOTE_INTENSET_IN3_Pos;
			}
			else if( mode == FALLING )
			{
				inter_mode[3] = 0x02;
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
											| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
											| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
											| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
											| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
											
				NRF_GPIOTE->CONFIG[3] =  (NRF_GPIOTE_POLARITY_HITOLO << GPIOTE_CONFIG_POLARITY_Pos)
										| (nrf_pin << GPIOTE_CONFIG_PSEL_Pos)  
										| (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);							
				NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN3_Set << GPIOTE_INTENSET_IN3_Pos; 				
			}									
			
		}

		/*
		NRF_GPIOTE->INTENSET  = GPIOTE_INTENSET_IN0_Set << GPIOTE_INTENSET_IN0_Pos 
								| GPIOTE_INTENSET_IN1_Set << GPIOTE_INTENSET_IN1_Pos
								| GPIOTE_INTENSET_IN2_Set << GPIOTE_INTENSET_IN2_Pos		
								| GPIOTE_INTENSET_IN3_Set << GPIOTE_INTENSET_IN3_Pos;
		*/	
		LinkInterrupt(GPIOTE_IRQn, GPIOTE_handler);
		NVIC_EnableIRQ(GPIOTE_IRQn);
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void detachInterrupt(uint32_t pin )
{	
	uint32_t nrf_pin;
	uint8_t channel;
	//Get the GPIOTE Channel
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin == pin_for_inter[0])
	{
		channel = 0;
	}
	else if( nrf_pin == pin_for_inter[1] ) 
	{
		channel = 1;
	}
	else if( nrf_pin == pin_for_inter[2] ) 
	{
		channel = 2;
	}
	else if( nrf_pin == pin_for_inter[3] ) 
	{
		channel = 3;
	}
	else
	{
		channel = 255;
	}
	//check right or wrong
	if(nrf_pin < 31 && channel<4)
	{
		//simple_uart_putstring("here1 \r\n");
		GPIOTE_Channel_Clean(channel);
		inter_mode[channel] == 0x00;
		pin_for_inter[channel] = 0xff;
	
		if( pin_for_inter[0] == 0xFF && pin_for_inter[1] == 0xFF && pin_for_inter[2] == 0xFF && pin_for_inter[3] == 0xFF )
		{	//simple_uart_putstring("here6 \r\n");
			NVIC_DisableIRQ(GPIOTE_IRQn);
			UnlinkInterrupt(GPIOTE_IRQn);
		}
	}
}

/**********************************************************************
name :
function : 
**********************************************************************/
//void GPIOTE_IRQHandler(void)
static void GPIOTE_handler( void )
{	
	uint32_t index_s , index_x;
	
	delay_ex_interrupter(30000); 
	//simple_uart_printHEX( NRF_GPIO->IN );
	for(index_x=0; index_x<4; index_x++)
	{
		if(pin_for_inter[index_x] != 0xFF)
		{	
			index_s = pin_for_inter[index_x];
			if(inter_mode[index_x] == 0x01)
			{
				if( NRF_GPIOTE->EVENTS_IN[index_x] == 1 && ( ((NRF_GPIO->IN >> index_s)&1UL) == 1)  )
				{
					NRF_GPIOTE->EVENTS_IN[index_x] = 0;
					pin_interrupterCB[index_x]();
				}
				else
				{
					NRF_GPIOTE->EVENTS_IN[index_x] = 0;
				}
			}
			else if( inter_mode[index_x] == 0x02 )
			{
				if( NRF_GPIOTE->EVENTS_IN[index_x] == 1 && ( ((NRF_GPIO->IN >> index_s)&1UL) == 0)  )
				{
					NRF_GPIOTE->EVENTS_IN[index_x] = 0;
					pin_interrupterCB[index_x]();
				}
				else
				{
					NRF_GPIOTE->EVENTS_IN[index_x] = 0;
				}		
			}
		}
	}
	/*
	if(pin_for_inter[1] != 0xFF)
	{	simple_uart_putstring("here \r\n");
		index_s = pin_for_inter[1];
		if(inter_mode[1] == 0x01)
		{
			if( NRF_GPIOTE->EVENTS_IN[1] == 1 && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_IN1_Msk) && ( ((NRF_GPIO->IN >> index_s)&1UL) == 1)  )
			{   simple_uart_putstring("here2 \r\n");
				NRF_GPIOTE->EVENTS_IN[1] = 0;
				pin_interrupterCB[1]();
			}
			else
			{
				NRF_GPIOTE->EVENTS_IN[1] = 0;
			}
		}
		else if( inter_mode[1] == 0x02 )
		{   simple_uart_putstring("here3 \r\n");
			if( NRF_GPIOTE->EVENTS_IN[1] == 1 && (NRF_GPIOTE->INTENSET & GPIOTE_INTENSET_IN1_Msk) && ( ((NRF_GPIO->IN >> index_s)&1UL) == 0)  )
			{
				NRF_GPIOTE->EVENTS_IN[1] = 0;
				pin_interrupterCB[1]();
			}
			else
			{
				NRF_GPIOTE->EVENTS_IN[1] = 0;
			}		
		}
	}	
	*/
}







