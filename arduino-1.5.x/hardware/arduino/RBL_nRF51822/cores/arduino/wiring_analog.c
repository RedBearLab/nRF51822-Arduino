

#include "Arduino.h"
#include "nrf_soc.h"
#include "nrf_sdm.h"

#ifdef __cplusplus
extern "C" {
#endif

static uint16_t PPI_Channels_Occupied[4][2] = {255, 255, 255, 255, 255, 255, 255, 255}; 	//Save PPI channel number, each GPIOTE channel takes up two PPI channels
static uint8_t GPIOTE_Channels_Occupied[4]  = {255,255,255,255}; 			  				//GPIOTE channel Status, 1--have occupied, 255--not occupied
static uint8_t GPIOTE_Channel_for_Analog[3] = {255, 255, 255};				  				//Save the Channel number used by PWM,	
																						   
static uint8_t Timer1_Occupied_Pin[3] = {255, 255, 255}; 				      				//the pin which used for analogWrite
//uint8_t Timer2_Occupied_Pin[3] = {255, 255, 255}; 

static uint32_t PWM_Channels_Value[3] = {((2^PWM_RESOLUTION) - 1), ((2^PWM_RESOLUTION) - 1), ((2^PWM_RESOLUTION) - 1)};		//the PWM value to update
static uint8_t  PWM_Channels_Start[3] = {0, 0, 0};																			//1:start, 0:stop

//initialize default
static uint32_t analogReference_ext_type 	= EXT_REFSEL_NONE;
static uint32_t analogReference_type 	 	= REFSEL_VDD_1_3_PS;
static uint32_t analogReference_inpsel_type = INPSEL_AIN_1_3_PS;
//
static uint8_t analogReadResolution_bit 	= READ_CURRENT_RESOLUTION;
static uint8_t analogWriteResolution_bit	= WRITE_CURRENT_RESOLUTION;

static uint8_t  softdevice_enabled;

/**********************************************************************
name :
function : 
**********************************************************************/
void analogInpselType( uint32_t type)
{
	if(type == INPSEL_AIN_NO_PS || type == INPSEL_AIN_2_3_PS || type == INPSEL_AIN_1_3_PS || type == INPSEL_VDD_2_3_PS || type == INPSEL_VDD_1_3_PS)
	{
		analogReference_inpsel_type = type;
	}
}
/**********************************************************************
name :
function : Using an external reference voltage cannot be more than: 1.3 V
**********************************************************************/
void analogReference( uint32_t type )
{	
	if( type == REFSEL_VBG || type == REFSEL_VDD_1_2_PS || type == REFSEL_VDD_1_3_PS)
	{
		analogReference_ext_type = EXT_REFSEL_NONE;
		analogReference_type = type;
	}
	else if( type == EXT_REFSEL_AREF0 || type == EXT_REFSEL_AREF1 )
	{
		analogReference_ext_type = type;
		analogReference_type = REFSEL_EXT;		
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void analogReadResolution( uint8_t resolution)
{	
	if(resolution <= 10 )
	{
		analogReadResolution_bit = resolution;
	}
}
/**********************************************************************
name :
function : 
**********************************************************************/
void analogWriteResolution( uint8_t resolution )
{
	if(resolution <= 16 )
	{
		analogWriteResolution_bit = resolution;
	}
}
/**********************************************************************
name :
function : find free GPIOTE channel
**********************************************************************/
uint8_t GPIOTE_Channel_Find()
{	
	uint8_t _index;
	
	for(_index=0; _index<4; _index++)
	{
		if( GPIOTE_Channels_Occupied[_index] == 255)
		{	
			return _index;
		}
	}
	
	return 255;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void GPIOTE_Channel_Set(uint8_t channel)
{
	GPIOTE_Channels_Occupied[channel] = 1;
}
/**********************************************************************
name :
function : 
**********************************************************************/
void GPIOTE_Channel_Clean(uint8_t channel)
{	
	nrf_gpiote_unconfig(channel);
	GPIOTE_Channels_Occupied[channel] = 255;
}

/**********************************************************************
name :
function : 
**********************************************************************/
static inline uint32_t conversion_Resolution(uint32_t value, uint32_t from, uint32_t to) {
	
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}
/**********************************************************************
name :
function : 
**********************************************************************/
uint32_t analogRead(uint32_t pin)
{	
    uint32_t value = 0;
	uint32_t pValue = 0;
	uint32_t nrf_pin = 0;
	//PIN transform to nRF51822
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	
	if( nrf_pin > 0 && nrf_pin < 7)
	{	
		pValue = (1 << (nrf_pin + 1));
		NRF_ADC->CONFIG = ( ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos) |
						  ( analogReference_inpsel_type << ADC_CONFIG_INPSEL_Pos) |
						  ( analogReference_type << ADC_CONFIG_REFSEL_Pos) |
						  ( pValue << ADC_CONFIG_PSEL_Pos) |
						  ( analogReference_ext_type << ADC_CONFIG_EXTREFSEL_Pos);
		
		NRF_ADC->INTENCLR = 0xFFFFFFFF;
		NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos;
		NRF_ADC->TASKS_START = 1;
		
		while( (NRF_ADC->BUSY & ADC_BUSY_BUSY_Msk) == (ADC_BUSY_BUSY_Busy << ADC_BUSY_BUSY_Pos) );
		
		value = NRF_ADC->RESULT;
		
		value = conversion_Resolution(value, ADC_RESOLUTION, analogReadResolution_bit);
		NRF_ADC->ENABLE = (ADC_ENABLE_ENABLE_Disabled 	<< ADC_ENABLE_ENABLE_Pos);
		NRF_ADC->CONFIG = 	(ADC_CONFIG_RES_8bit << ADC_CONFIG_RES_Pos) |
							(ADC_CONFIG_INPSEL_SupplyTwoThirdsPrescaling << ADC_CONFIG_INPSEL_Pos) |
							(ADC_CONFIG_REFSEL_VBG << ADC_CONFIG_REFSEL_Pos) |
							(ADC_CONFIG_PSEL_Disabled << ADC_CONFIG_PSEL_Pos) |
							(ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);			
		
		
	}
	return value;
}
/**********************************************************************
name :
function : PWM update
**********************************************************************/
static void update_PWM_value(uint32_t ulPin, uint32_t ulValue, uint32_t PWM_channel)
{
	uint32_t channel;

	channel = GPIOTE_Channel_for_Analog[PWM_channel];
	PWM_Channels_Value[PWM_channel] = ((2^PWM_RESOLUTION) - 1) - conversion_Resolution(ulValue, analogWriteResolution_bit, PWM_RESOLUTION);

	if ((NRF_GPIOTE->CONFIG[channel] & GPIOTE_CONFIG_MODE_Msk) == (GPIOTE_CONFIG_MODE_Disabled << GPIOTE_CONFIG_MODE_Pos))
	{	
		PWM_Channels_Start[PWM_channel] = 1;
	}
	
}
/**********************************************************************
name :
function : find free PPI channel
**********************************************************************/
int find_free_PPI_channel(int exclude_channel)
{
	uint32_t err_code = NRF_SUCCESS, chen;
	int start = 0;
	int end = 8;
	int i;

	err_code = sd_softdevice_is_enabled(&softdevice_enabled);
	APP_ERROR_CHECK(err_code);
	if(softdevice_enabled == 0)
	{
		chen = NRF_PPI->CHEN;
	}
	else
	{ 
		err_code = sd_ppi_channel_enable_get(&chen);
		APP_ERROR_CHECK(err_code);
	}
	for (i = start; i < end; i++)
	{	
		//if (! (NRF_PPI->CHEN & (1 << i)))
		if(! ( chen & (1 << i) ) )
		{
			if (i != exclude_channel)
			{
				return i;
			}
		}
	}
	return 255;
}
/**********************************************************************
name :  
function : called by wiring_digital.c
**********************************************************************/
void PPI_ON_TIMER_GPIO(uint32_t gpiote_channel, NRF_TIMER_Type* Timer, uint32_t CC_channel)
{	
	uint32_t err_code = NRF_SUCCESS, chen;

	// Initialize Programmable Peripheral Interconnect
	int chan_0 = find_free_PPI_channel(255);
	int chan_1 = find_free_PPI_channel(chan_0);
	
	if ((chan_0 != 255) && (chan_1 != 255))
	{	
		err_code = sd_softdevice_is_enabled(&softdevice_enabled);
		APP_ERROR_CHECK(err_code);
		if (softdevice_enabled == 0)
		{	
			// Enable PPI using registers
			NRF_PPI->CH[chan_0].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[CC_channel]) );
			NRF_PPI->CH[chan_0].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
			NRF_PPI->CHEN |= ( 1 << chan_0);
			// Save PPI channel number
			PPI_Channels_Occupied[gpiote_channel][0] = chan_0;
			
			// Configure PPI channel "chan_1" to toggle "ulPin" pin on every Timer COMPARE[3] match
			NRF_PPI->CH[chan_1].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[3]) );
			NRF_PPI->CH[chan_1].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
			NRF_PPI->CHEN |= ( 1 << chan_1);
			// Save PPI channel number
			PPI_Channels_Occupied[gpiote_channel][1] = chan_1;
			//simple_uart_printHEX(NRF_PPI->CHEN);
						
		}
		else
		{	
			//Enable PPI using sd_ppi_x
			err_code = sd_ppi_channel_assign(chan_0, &((*Timer).EVENTS_COMPARE[CC_channel]), &NRF_GPIOTE->TASKS_OUT[gpiote_channel]);
			APP_ERROR_CHECK(err_code);
			err_code = sd_ppi_channel_enable_set(1 << chan_0);
			APP_ERROR_CHECK(err_code);
			//Save PPI channel number
			PPI_Channels_Occupied[gpiote_channel][0] = chan_0;
			
			err_code = sd_ppi_channel_assign(chan_1, &((*Timer).EVENTS_COMPARE[3]), &NRF_GPIOTE->TASKS_OUT[gpiote_channel]);
			APP_ERROR_CHECK(err_code);
			err_code = sd_ppi_channel_enable_set(1 << chan_1 );	
			APP_ERROR_CHECK(err_code);
			//Save PPI channel number
			PPI_Channels_Occupied[gpiote_channel][1] = chan_1;	
			
			err_code = sd_ppi_channel_enable_get(&chen);
			APP_ERROR_CHECK(err_code);
		}

	}

	/*
	if(0 == gpiote_channel)
	{
		NRF_PPI->CH[9].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[CC_channel]) );
		NRF_PPI->CH[9].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
		
		NRF_PPI->CHEN |= ( 1 << 9);
		// Save PPI channel number
		PPI_Channels_Occupied[gpiote_channel][0] = 9;

		NRF_PPI->CH[10].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[3]) );
		NRF_PPI->CH[10].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
		NRF_PPI->CHEN |= ( 1 << 10);
		// Save PPI channel number
		PPI_Channels_Occupied[gpiote_channel][1] = 10;		
	}
	else if(1 == gpiote_channel)
	{
		NRF_PPI->CH[11].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[CC_channel]) );
		NRF_PPI->CH[11].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
		NRF_PPI->CHEN |= ( 1 << 11);
		// Save PPI channel number
		PPI_Channels_Occupied[gpiote_channel][0] = 11;

		NRF_PPI->CH[12].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[3]) );
		NRF_PPI->CH[12].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
		NRF_PPI->CHEN |= ( 1 << 12);
		// Save PPI channel number
		PPI_Channels_Occupied[gpiote_channel][1] = 12;		
	}
	else if(2 == gpiote_channel)
	{
		NRF_PPI->CH[13].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[CC_channel]) );
		NRF_PPI->CH[13].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
		NRF_PPI->CHEN |= ( 1 << 13);
		// Save PPI channel number
		PPI_Channels_Occupied[gpiote_channel][0] = 13;

		NRF_PPI->CH[14].EEP = (uint32_t)( &((*Timer).EVENTS_COMPARE[3]) );
		NRF_PPI->CH[14].TEP = (uint32_t)( &NRF_GPIOTE->TASKS_OUT[gpiote_channel] );
		NRF_PPI->CHEN |= ( 1 << 14);
		// Save PPI channel number
		PPI_Channels_Occupied[gpiote_channel][1] = 14;		
	}
	*/
}
/**********************************************************************
name :
function : disconnect PPI
**********************************************************************/
void PPI_Off_FROM_GPIO(uint32_t pin)
{
	uint8_t channel;
	uint32_t err_code = NRF_SUCCESS;
	
	err_code = sd_softdevice_is_enabled(&softdevice_enabled);
	APP_ERROR_CHECK(err_code);

	if(Timer1_Occupied_Pin[0] == pin)
	{	
		channel = GPIOTE_Channel_for_Analog[0];
	
		if(softdevice_enabled == 0)
		{
			NRF_PPI->CHEN &= ~( 1 << PPI_Channels_Occupied[channel][0] );
			NRF_PPI->CHEN &= ~( 1 << PPI_Channels_Occupied[channel][1] );
		}
		else
		{   
			err_code = sd_ppi_channel_enable_clr( 1 << PPI_Channels_Occupied[channel][0] );
			APP_ERROR_CHECK(err_code);
			err_code = sd_ppi_channel_enable_clr( 1 << PPI_Channels_Occupied[channel][1] );
			APP_ERROR_CHECK(err_code);			
		}
		PPI_Channels_Occupied[channel][0] = 255;
		PPI_Channels_Occupied[channel][1] = 255;
		Timer1_Occupied_Pin[0] = 255;

		GPIOTE_Channel_Clean(channel);
		GPIOTE_Channel_for_Analog[0] = 255;
	}
	else if(Timer1_Occupied_Pin[1] == pin)
	{

		channel = GPIOTE_Channel_for_Analog[1];
		if(softdevice_enabled == 0)
		{
			NRF_PPI->CHEN &= ~( 1 << PPI_Channels_Occupied[channel][0] );
			NRF_PPI->CHEN &= ~( 1 << PPI_Channels_Occupied[channel][1] );
		}
		else
		{
			err_code = sd_ppi_channel_enable_clr( 1 << PPI_Channels_Occupied[channel][0] );
			APP_ERROR_CHECK(err_code);
			err_code = sd_ppi_channel_enable_clr( 1 << PPI_Channels_Occupied[channel][1] );
			APP_ERROR_CHECK(err_code);			
		}
		PPI_Channels_Occupied[channel][0] = 255;
		PPI_Channels_Occupied[channel][1] = 255;
		Timer1_Occupied_Pin[1] = 255;
		GPIOTE_Channel_Clean(channel);
		GPIOTE_Channel_for_Analog[1] = 255;

	}
	else if(Timer1_Occupied_Pin[2] == pin)
	{
		channel = GPIOTE_Channel_for_Analog[2];
		if(softdevice_enabled == 0)
		{
			NRF_PPI->CHEN &= ~( 1 << PPI_Channels_Occupied[channel][0] );
			NRF_PPI->CHEN &= ~( 1 << PPI_Channels_Occupied[channel][1] );
		}
		else
		{
			err_code = sd_ppi_channel_enable_clr( 1 << PPI_Channels_Occupied[channel][0] );
			APP_ERROR_CHECK(err_code);
			err_code = sd_ppi_channel_enable_clr( 1 << PPI_Channels_Occupied[channel][1] );
			APP_ERROR_CHECK(err_code);			
		}
		PPI_Channels_Occupied[channel][0] = 255;
		PPI_Channels_Occupied[channel][1] = 255;
		Timer1_Occupied_Pin[2] = 255;

		GPIOTE_Channel_Clean(channel);
		GPIOTE_Channel_for_Analog[2] = 255;
	}
	
	//if all PWM stop, disable TIMER1
	if( Timer1_Occupied_Pin[0] == 255 && Timer1_Occupied_Pin[1] == 255 && Timer1_Occupied_Pin[2] == 255 )
	{
		NRF_TIMER1->TASKS_STOP = 1;
		NRF_TIMER1->INTENCLR = (TIMER_INTENCLR_COMPARE3_Disabled << TIMER_INTENCLR_COMPARE3_Pos);
		NVIC_DisableIRQ(TIMER1_IRQn); 
		UnlinkInterrupt(TIMER1_IRQn);
	}
	
}
/**********************************************************************
name :
function : 
**********************************************************************/
//void TIMER1_IRQHandler(void)  
static void TIMER1_handler( void )
{	
	uint8_t index;
	// Update the CCx values
	NRF_TIMER1->CC[0] = PWM_Channels_Value[0];
	NRF_TIMER1->CC[1] = PWM_Channels_Value[1];
	NRF_TIMER1->CC[2] = PWM_Channels_Value[2];
	NRF_TIMER1->CC[3] = 0;
	
	if (PWM_Channels_Start[0] == 1)
	{
		NRF_TIMER1->EVENTS_COMPARE[0] = 0;
		nrf_gpiote_task_config(GPIOTE_Channel_for_Analog[0], Timer1_Occupied_Pin[0], NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
		PWM_Channels_Start[0] = 0;
	}
	if (PWM_Channels_Start[1] == 1)
	{
		NRF_TIMER1->EVENTS_COMPARE[1] = 0;
		nrf_gpiote_task_config(GPIOTE_Channel_for_Analog[1], Timer1_Occupied_Pin[1], NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
		PWM_Channels_Start[1] = 0;
	}
	if (PWM_Channels_Start[2] == 1)
	{
		NRF_TIMER1->EVENTS_COMPARE[2] = 0;
		nrf_gpiote_task_config(GPIOTE_Channel_for_Analog[2], Timer1_Occupied_Pin[2], NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
		PWM_Channels_Start[2] = 0;
	}
	//when IRQHandle, make sure all GPIO is LOW. If HIGH,reconfigure it.
	for(index=0; index<3; index++)
	{
		if(Timer1_Occupied_Pin[index] != 255)
		{
			if( (NRF_GPIO->IN >> Timer1_Occupied_Pin[index]) & 1UL )
			{
				nrf_gpiote_task_config(GPIOTE_Channel_for_Analog[index], Timer1_Occupied_Pin[index], NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
			}
		}
	}
	
	NRF_TIMER1->EVENTS_COMPARE[3] = 0;
}

/**********************************************************************
name :
function : 
**********************************************************************/
void analogWrite(uint32_t ulPin, uint32_t ulValue)
{	
	uint32_t nrf_pin, max_value, err_code = NRF_SUCCESS , crystal_is_running=0;
	uint8_t gpiote_channel;

	nrf_pin = Pin_nRF51822_to_Arduino(ulPin);
	if( nrf_pin < 31)
	{	//if vaule 0 or >255, set LOW or HIGH
		if(ulValue <= 0)
		{  
			PPI_Off_FROM_GPIO(nrf_pin);
			NRF_GPIO->OUTCLR = (1 << nrf_pin);
			return;
		}
		max_value = (uint32_t)( pow(2, analogWriteResolution_bit) - 1 );
		if(ulValue >= max_value )
		{	
			ulValue = max_value - 1;
		}
		//if exist,  update the value
		if (Timer1_Occupied_Pin[0] == nrf_pin)
		{	
			update_PWM_value(nrf_pin, ulValue, 0);
		}
		else if (Timer1_Occupied_Pin[1] == nrf_pin)
		{
			update_PWM_value(nrf_pin, ulValue, 1);
		}
		else if (Timer1_Occupied_Pin[2] == nrf_pin)
		{
			update_PWM_value(nrf_pin, ulValue, 2);
		}
		else
		{  
			if ((Timer1_Occupied_Pin[0] == 255) && (Timer1_Occupied_Pin[1] == 255) && (Timer1_Occupied_Pin[2] == 255))
			{	
				// Configure ulPin as output
				NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
						| (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
						| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
						| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
						| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
				NRF_GPIO->OUTCLR = (1 << nrf_pin);	
				
				//fine a free gpiote channel
				gpiote_channel = GPIOTE_Channel_Find();
				if( gpiote_channel == 255 )
				{
					return;
				}
				//configure TIMER1
				NRF_TIMER1->TASKS_STOP = 1;
				NRF_TIMER1->MODE = TIMER_MODE_MODE_Timer;
				NRF_TIMER1->PRESCALER = 0; // Source clock frequency is divided by 2^6 = 64 				
				//NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_08Bit;
				NRF_TIMER1->BITMODE = TIMER_BITMODE_BITMODE_16Bit;	
				// Clears the timer, sets it to 0
				NRF_TIMER1->TASKS_CLEAR = 1;
				NRF_TIMER1->CC[0] = ((2^PWM_RESOLUTION) - 1);
				NRF_TIMER1->CC[1] = ((2^PWM_RESOLUTION) - 1);
				NRF_TIMER1->CC[2] = ((2^PWM_RESOLUTION) - 1);
				NRF_TIMER1->CC[3] = 0;
				NRF_TIMER1->EVENTS_COMPARE[0] = 0;
				NRF_TIMER1->EVENTS_COMPARE[1] = 0;
				NRF_TIMER1->EVENTS_COMPARE[2] = 0;
				NRF_TIMER1->EVENTS_COMPARE[3] = 0;

				//Interrupt setup
				NRF_TIMER1->INTENSET = (TIMER_INTENSET_COMPARE3_Enabled << TIMER_INTENSET_COMPARE3_Pos);
				LinkInterrupt( TIMER1_IRQn, TIMER1_handler );
				//can't set low priority, else the GPIO polarity will change 
				//NVIC_SetPriority(TIMER1_IRQn, 3);  
				//NVIC_ClearPendingIRQ(TIMER1_IRQn);
				NVIC_EnableIRQ(TIMER1_IRQn); 
				NRF_TIMER1->TASKS_START = 1;				
				// PPI for TIMER1 and IO TASK
				nrf_gpiote_task_config(gpiote_channel, nrf_pin, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
				GPIOTE_Channel_Set(gpiote_channel);
				PPI_ON_TIMER_GPIO(gpiote_channel, NRF_TIMER1, 0);
				//Save pin , channel and value
				GPIOTE_Channel_for_Analog[0] = gpiote_channel;
				PWM_Channels_Value[0] = ((2^PWM_RESOLUTION) - 1) - conversion_Resolution(ulValue, analogWriteResolution_bit, PWM_RESOLUTION);
				Timer1_Occupied_Pin[0] = nrf_pin;
			}
			else
			{
				if (Timer1_Occupied_Pin[0] == 255)
				{
					NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
												| (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
												| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
												| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
												| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
					NRF_GPIO->OUTCLR = (1 << nrf_pin);
					//fine a free gpiote channel and configure the channel
					gpiote_channel = GPIOTE_Channel_Find();
					if( gpiote_channel == 255 )
					{
						return;
					}
					nrf_gpiote_task_config(gpiote_channel, nrf_pin, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
					GPIOTE_Channel_Set(gpiote_channel);
					PPI_ON_TIMER_GPIO(gpiote_channel, NRF_TIMER1, 0);
					//save the pin and value
					GPIOTE_Channel_for_Analog[0] = gpiote_channel;
					PWM_Channels_Value[0] = ((2^PWM_RESOLUTION) - 1) - conversion_Resolution(ulValue, analogWriteResolution_bit, PWM_RESOLUTION);
					Timer1_Occupied_Pin[0] = nrf_pin;
					NRF_TIMER1->EVENTS_COMPARE[0] = 0;
				}
				else if (Timer1_Occupied_Pin[1] == 255)
				{
					NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
												| (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
												| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
												| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
												| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
					NRF_GPIO->OUTCLR = (1 << nrf_pin);
					//find a free gpiote channel
					gpiote_channel = GPIOTE_Channel_Find();
					if( gpiote_channel == 255 )
					{
						return;
					}
					
					nrf_gpiote_task_config(gpiote_channel, nrf_pin, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
					GPIOTE_Channel_Set(gpiote_channel);
					PPI_ON_TIMER_GPIO(gpiote_channel, NRF_TIMER1, 1);
					//save the pin and value
					GPIOTE_Channel_for_Analog[1] = gpiote_channel;
					PWM_Channels_Value[1] = ((2^PWM_RESOLUTION) - 1) - conversion_Resolution(ulValue, analogWriteResolution_bit, PWM_RESOLUTION);
					Timer1_Occupied_Pin[1] = nrf_pin;
					NRF_TIMER1->EVENTS_COMPARE[1] = 0;
				}
				else if (Timer1_Occupied_Pin[2] == 255)
				{
					NRF_GPIO->PIN_CNF[nrf_pin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
												| (GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos)
												| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
												| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
												| (GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos);
					NRF_GPIO->OUTCLR = (1 << nrf_pin);
					//find a free gpiote channel
					gpiote_channel = GPIOTE_Channel_Find();
					if( gpiote_channel == 255 )
					{
						return;
					}
					
					nrf_gpiote_task_config(gpiote_channel, nrf_pin, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
					GPIOTE_Channel_Set(gpiote_channel);
					PPI_ON_TIMER_GPIO(gpiote_channel, NRF_TIMER1, 2);
					//save the pin and value
					GPIOTE_Channel_for_Analog[2] = gpiote_channel;
					PWM_Channels_Value[2] = ((2^PWM_RESOLUTION) - 1) - conversion_Resolution(ulValue, analogWriteResolution_bit, PWM_RESOLUTION);
					Timer1_Occupied_Pin[2] = nrf_pin;
					NRF_TIMER1->EVENTS_COMPARE[2] = 0;
				}
				else
				{   
					//no more
				}
			}
		}	
	}
}


#ifdef __cplusplus
}
#endif
