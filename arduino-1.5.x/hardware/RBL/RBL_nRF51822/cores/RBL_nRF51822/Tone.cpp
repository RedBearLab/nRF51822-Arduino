

#include "Tone.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"
/**********************************************************************
declared
**********************************************************************/
static uint8_t tone_channel=255;
static uint8_t pin_state = 0;
static uint8_t finish_flag = 0;
static uint32_t inter_count;
static uint32_t tone_pin;

static void TIMER2_handler( void );
/**********************************************************************
name :
function : 
**********************************************************************/
void tone(uint32_t pin, uint16_t freq, uint32_t duration)
{
	uint32_t i,prescaler, compare, nrf_pin, err_code = NRF_SUCCESS;
	uint8_t channel, softdevice_enabled;
	
	channel = GPIOTE_Channel_Find();
	if(channel == 255)
	{
		return;
	}
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin < 31)
	{
		//save the pin number
		tone_pin = nrf_pin;
		
		// Find appropriate values for PRESCALER and COMPARE registers
		for (i = 0; i <= 9; i++)
		{
			prescaler = i;
			compare = VARIANT_MCK / freq;
			compare = compare >> (prescaler+1);
			compare = compare - 1;
			if ((compare >= 2) && (compare <= 65535))
				break;
		}
		//calculate the interrupts count
		if(duration > 0)
		{	
			finish_flag = 1;
			inter_count = ((freq * duration) / 1000) * 2;
		}
		else
		{
			finish_flag = 0;
			inter_count = 0xFFFFFFFF;
		}
		//
		//pinMode(pin, OUTPUT);
		//digitalWrite(pin, LOW);
		NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
								| (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
								| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
								| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
								| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);	
		NRF_GPIO->OUTCLR = (1 << nrf_pin);						
		//pin_state = 0;
		//use ppi and gpiote_task 
		tone_channel = channel;
		GPIOTE_Channel_Set(channel);
		nrf_gpiote_task_config(channel, nrf_pin, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
		//configure PPI
		err_code = sd_softdevice_is_enabled(&softdevice_enabled);
		APP_ERROR_CHECK(err_code);
		if (softdevice_enabled == 0)
		{	
			NRF_PPI->CH[6].EEP = (uint32_t)( &NRF_TIMER2->EVENTS_COMPARE[0] );
			NRF_PPI->CH[6].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[channel]);
			NRF_PPI->CHEN |= ( 1 << 6);
		}
		else
		{
			err_code = sd_ppi_channel_assign(6, &NRF_TIMER2->EVENTS_COMPARE[0], &NRF_GPIOTE->TASKS_OUT[channel]);
			APP_ERROR_CHECK(err_code);
			err_code = sd_ppi_channel_enable_set(1 << 6);
			APP_ERROR_CHECK(err_code);
		}
		//Configure TIMER2
		NRF_TIMER2->TASKS_STOP = 1;
		NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;
		NRF_TIMER2->PRESCALER = prescaler;
		NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;
		
		NRF_TIMER2->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);

		NRF_TIMER2->TASKS_CLEAR = 1;
		NRF_TIMER2->CC[0] = (uint16_t)(compare);
		NRF_TIMER2->EVENTS_COMPARE[0] = 0;
		
		NRF_TIMER2->INTENCLR = 0xFFFFFFFF;
		NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);
		//open IRQ
		LinkInterrupt(TIMER2_IRQn, TIMER2_handler);
		NVIC_EnableIRQ(TIMER2_IRQn);
		NRF_TIMER2->TASKS_START = 1;
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void tone(uint32_t pin, uint16_t freq)
{
	tone(pin, freq, 0);
}
/**********************************************************************
name :
function : 
**********************************************************************/
void noTone(uint32_t pin)
{	
	uint8_t softdevice_enabled;
	uint32_t nrf_pin, err_code = NRF_SUCCESS;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	if(nrf_pin < 31)
	{	
		if(nrf_pin == tone_pin )
		{	//stop TIMER2
			NRF_TIMER2->TASKS_STOP = 1;
			UnlinkInterrupt(TIMER2_IRQn);
			NVIC_DisableIRQ(TIMER2_IRQn); 
			//uconfig GPIOTE channel
			GPIOTE_Channel_Clean(tone_channel);
			tone_channel = 255;
			
			err_code = sd_softdevice_is_enabled(&softdevice_enabled);
			APP_ERROR_CHECK(err_code);
			if (softdevice_enabled == 0)
			{
				NRF_PPI->CHEN &= ~( 1 << 6);
			}
			else
			{
				err_code = sd_ppi_channel_enable_clr( 1 << 6 );
				APP_ERROR_CHECK(err_code);	
			}
			//nrf_gpiote_unconfig(3);
			//digitalWrite(tone_pin, LOW);
			NRF_GPIO->OUTCLR = (1 << tone_pin);		
			//pin_state = 0;
			tone_pin = 255;		
		}
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
//void TIMER2_IRQHandler( void )
static void TIMER2_handler( void )
{	
	NRF_TIMER2->EVENTS_COMPARE[0] = 0;
	
	if(finish_flag == 1)
	{
		if(inter_count)
		{
			inter_count--;
		}
		else
		{
			noTone(tone_pin);
		}
	}
	/*
	if( finish_flag == 1)
	{	
		if(inter_count)
		{
			inter_count--;
			if( pin_state == 0)
			{
				pin_state = 1;
				//digitalWrite( tone_pin, HIGH);
				NRF_GPIO->OUTSET = (1 << tone_pin);		
			}
			else if( pin_state == 1)
			{
				pin_state = 0;
				//digitalWrite( tone_pin, LOW);
				NRF_GPIO->OUTCLR = (1 << tone_pin);		
			}
		}
		else
		{
			NRF_TIMER2->TASKS_STOP = 1;
			//digitalWrite(tone_pin, LOW);
			NRF_GPIO->OUTCLR = (1 << tone_pin);		
			pin_state = 0;
			tone_pin = 255;
		}
	}
	else if( finish_flag == 0)
	{
		if( pin_state == 0)
		{
			pin_state = 1;
			//digitalWrite( tone_pin, HIGH);
			NRF_GPIO->OUTSET = (1 << tone_pin);	
		}
		else if( pin_state == 1)
		{
			pin_state = 0;
			//digitalWrite( tone_pin, LOW);
			NRF_GPIO->OUTCLR = (1 << tone_pin);		
		}
	}
	*/
}





























