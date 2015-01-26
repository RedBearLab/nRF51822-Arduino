

#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
extern "C" {
#endif


#define ADC_RESOLUTION        10
#define PWM_RESOLUTION        15

#define WRITE_CURRENT_RESOLUTION    8
#define READ_CURRENT_RESOLUTION		10


extern void analogWrite( uint32_t ulPin, uint32_t ulValue );
extern uint32_t analogRead( uint32_t ulPin );

extern void analogReference( uint32_t type );
extern void analogInpselType( uint32_t type);
//extern void analogExtReference( uint32_t type );
extern void analogReadResolution( uint8_t resolution);
extern void analogWriteResolution( uint8_t resolution );

extern uint8_t GPIOTE_Channel_Find();
extern void GPIOTE_Channel_Set(uint8_t channel);
extern void GPIOTE_Channel_Clean(uint8_t channel);

extern int find_free_PPI_channel(int exclude_channel);

extern void PPI_Off_FROM_GPIO(uint32_t pin);

#ifdef __cplusplus
}
#endif

#endif 
