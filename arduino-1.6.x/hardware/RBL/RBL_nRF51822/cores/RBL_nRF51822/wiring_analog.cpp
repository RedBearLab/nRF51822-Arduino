
#include "Arduino.h"
#include "wiring_analog.h"

//initialize default
static uint32_t analogReference_ext_type 	= EXT_REFSEL_NONE;
static uint32_t analogReference_ref_type 	= REFSEL_VDD_1_3_PS;
static uint32_t analogReference_inpsel_type = INPSEL_AIN_1_3_PS;
//resolution bit
static uint8_t analogReadResolution_bit 	= READ_DEFAULT_RESOLUTION;
static uint8_t analogWriteResolution_bit	= WRITE_DEFAULT_RESOLUTION;

//PWM channels, record the pin of PWM
static PinName PWM_Pins[3] = {(PinName)NC, (PinName)NC, (PinName)NC};


void analogReference(uint8_t ref_type, uint8_t input_type)
{
	if( ((ref_type == REFSEL_VBG) || (ref_type == REFSEL_VDD_1_2_PS) || (ref_type == REFSEL_VDD_1_3_PS) || (ref_type == EXT_REFSEL_AREF0) || (ref_type == EXT_REFSEL_AREF1)) &&
		((input_type == INPSEL_AIN_NO_PS) || (input_type == INPSEL_AIN_2_3_PS) || (input_type == INPSEL_AIN_1_3_PS) || (input_type == INPSEL_VDD_2_3_PS) || (input_type == INPSEL_VDD_1_3_PS)) )
	{	
		if( ref_type == REFSEL_VBG || ref_type == REFSEL_VDD_1_2_PS || ref_type == REFSEL_VDD_1_3_PS)
		{	
			analogReference_ext_type = EXT_REFSEL_NONE;
			analogReference_ref_type = ref_type;
		}
		else if( ref_type == EXT_REFSEL_AREF0 || ref_type == EXT_REFSEL_AREF1 )
		{
			analogReference_ext_type = ref_type;
			analogReference_ref_type = REFSEL_EXT;		
		}		
		
		analogReference_inpsel_type = input_type;
	}
}

void analogReadResolution(uint8_t resolution)
{
	if(resolution <= 10)
		analogReadResolution_bit = resolution;
}

void analogWriteResolution( uint8_t resolution )
{
	if(resolution <= 16 )
	{
		analogWriteResolution_bit = resolution;
	}
}

static inline uint32_t conversion_Resolution(uint32_t value, uint32_t from, uint32_t to) 
{
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}

static uint8_t getFreeChannel(void)
{
	uint8_t index;
	
	for(index=0; index<3; index++){
		if(PWM_Pins[index] == (PinName)NC)
			return index;
	}
	return 0xFF;
}

static uint8_t getMatchChannel(PinName pin)
{
	uint8_t index;
	
	for(index=0; index<3; index++){
		if(PWM_Pins[index] == pin)
			return index;
	}
	return 0xFF;	
}

static void setPWMChannel(uint8_t channel, PinName pin)
{
	if(channel < 3)
		PWM_Pins[channel] = pin;
}

uint32_t analogRead(uint32_t pin)
{
	PinName 	nrf_pin;
	uint32_t pValue, value;
	
	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	MBED_ASSERT(nrf_pin != (PinName)NC);
	
	if((uint32_t)nrf_pin > 7 || (uint32_t)nrf_pin == 0)
		return 0xFFFF;
	
	pValue = (1 << ((uint32_t)nrf_pin + 1));
	NRF_ADC->CONFIG = ( ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos) |
					  ( analogReference_inpsel_type << ADC_CONFIG_INPSEL_Pos) |
					  ( analogReference_ref_type << ADC_CONFIG_REFSEL_Pos) |
					  ( pValue << ADC_CONFIG_PSEL_Pos) |
					  ( analogReference_ext_type << ADC_CONFIG_EXTREFSEL_Pos);
	
	NRF_ADC->INTENCLR = 0xFFFFFFFF;
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled << ADC_ENABLE_ENABLE_Pos;
	NRF_ADC->TASKS_START = 1;
	
	while( (NRF_ADC->BUSY & ADC_BUSY_BUSY_Msk) == (ADC_BUSY_BUSY_Busy << ADC_BUSY_BUSY_Pos) );
	
	value = NRF_ADC->RESULT;
	
	value = conversion_Resolution(value, 10, analogReadResolution_bit);
	NRF_ADC->ENABLE = (ADC_ENABLE_ENABLE_Disabled 	<< ADC_ENABLE_ENABLE_Pos);
	/* Sure pin is released */
	NRF_ADC->CONFIG |= (ADC_CONFIG_PSEL_Disabled << ADC_CONFIG_PSEL_Pos);
	
	return value;
}

void analogWrite(uint32_t pin, uint32_t value)
{
	PinName	nrf_pin;
	uint8_t num;
	pwmout_t obj;

	nrf_pin = Pin_nRF51822_to_Arduino(pin);
	MBED_ASSERT(nrf_pin != (PinName)NC);
	
	num = getMatchChannel(nrf_pin);
	if(num < 3) {
		/* Channel has exit, Write value */	
		// if( value == 0 )
		// {	
			// setPWMChannel(num, (PinName)NC);
			// obj.pwm = (PWMName)num;
			// pwmout_free(&obj);
			// pinMode(pin, OUTPUT);
			// digitalWrite(pin, LOW);
			// return;
		// }
		// Serial1.print("here1 : ");
		// Serial1.println((pow(2, analogWriteResolution_bit)-2), DEC);
		// Serial1.println(value, DEC);
		// if( value >= (pow(2, analogWriteResolution_bit)-2) )
		// {	Serial1.print("here2 : ");
			// setPWMChannel(num, (PinName)NC);
			// obj.pwm = (PWMName)num;
			// pwmout_free(&obj);
			// pinMode(pin, OUTPUT);
			// digitalWrite(pin, HIGH);
			// return;	
		// }
		uint32_t value_8bit;
		float duty;
		value_8bit = conversion_Resolution(value, analogWriteResolution_bit, 8);
		duty = (float)value_8bit / (255);
		
		obj.pwm = (PWMName)num;
		pwmout_write(&obj, duty);	
	}
	else {
		num = getFreeChannel();
		if(num >= 3)
			return;
			
		if( value == 0 )
		{	
			setPWMChannel(num, (PinName)NC);
			obj.pwm = (PWMName)num;
			pwmout_free(&obj);
			pinMode(pin, OUTPUT);
			digitalWrite(pin, LOW);
			return;
		}
		if( value >= (pow(2, analogWriteResolution_bit)-2) )
		{
			setPWMChannel(num, (PinName)NC);
			obj.pwm = (PWMName)num;
			pwmout_free(&obj);
			pinMode(pin, OUTPUT);
			digitalWrite(pin, HIGH);
			return;	
		}
		/* Have a free channel */	
		obj.pwm = (PWMName)num;
		pwmout_init(&obj, nrf_pin);	
		/* Write value */	
		uint32_t value_8bit;
		float duty;
		value_8bit = conversion_Resolution(value, analogWriteResolution_bit, 8);
		duty = (float)value_8bit / (255);
		pwmout_write(&obj, duty);
		setPWMChannel(num, nrf_pin);
	}
}



















