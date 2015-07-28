
#ifndef WIRING_ANALOG_H_
#define WIRING_ANALOG_H_


#define WRITE_DEFAULT_RESOLUTION    8
#define READ_DEFAULT_RESOLUTION		10

// ADC reference selection
#define REFSEL_VBG				ADC_CONFIG_REFSEL_VBG						// Use internal 1.2 V band gap reference
#define REFSEL_EXT				ADC_CONFIG_REFSEL_External					// Use external reference specified by CONCFIG EXTREFSEL
#define REFSEL_VDD_1_2_PS		ADC_CONFIG_REFSEL_SupplyOneHalfPrescaling	// Use VDD with 1/2 prescaling. (Only applicable when VDD is in the range 1.7V ?2.6V)
#define REFSEL_VDD_1_3_PS		ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling	// Use VDD with 1/3 prescaling. (Only applicable when VDD is in the range 2.5V ?3.6V)

// External reference pin selection
#define EXT_REFSEL_NONE			ADC_CONFIG_EXTREFSEL_None					// Analog reference inputs disabled
#define EXT_REFSEL_AREF0		ADC_CONFIG_EXTREFSEL_AnalogReference0		// Use AREF0 as analog reference
#define EXT_REFSEL_AREF1		ADC_CONFIG_EXTREFSEL_AnalogReference1		// Use AREF1 as analog reference

// ADC input selection
#define INPSEL_AIN_NO_PS		ADC_CONFIG_INPSEL_AnalogInputNoPrescaling 				// Analog input pin specified by CONFIG.PSEL with no prescaling
#define INPSEL_AIN_2_3_PS		ADC_CONFIG_INPSEL_AnalogInputTwoThirdsPrescaling		// Analog input pin specified by CONFIG. PSEL with 2/3 prescaling
#define INPSEL_AIN_1_3_PS		ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling 		// Analog input pin specified by CONFIG. PSEL with 1/3 prescaling
#define INPSEL_VDD_2_3_PS		ADC_CONFIG_INPSEL_SupplyTwoThirdsPrescaling				// Supply voltage with 2/3 prescaling used as input for the conversion
#define INPSEL_VDD_1_3_PS		ADC_CONFIG_INPSEL_SupplyOneThirdPrescaling				// Supply voltage with 1/3 prescaling used as input for the conversion



void analogReference(uint8_t ref_type, uint8_t input_type);
void analogReadResolution(uint8_t resolution);
void analogWriteResolution( uint8_t resolution );
uint32_t analogRead(uint32_t pin);
void analogWrite(uint32_t pin, uint32_t value);

#endif

