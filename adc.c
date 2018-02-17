#include "adc.h"

static ADC_HandleTypeDef *hadc;
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint8_t)    2)
uint16_t  ADC_Converted_Values[ADCCONVERTEDVALUES_BUFFER_SIZE];
#define VDDA_APPLI                     ((uint32_t) 3300)    /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */
#define COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_DATA) ( ((ADC_DATA) * VDDA_APPLI) / RANGE_12BITS)
uint16_t counter;
uint16_t  ADC_Converted_Values_Avg[ADCCONVERTEDVALUES_BUFFER_SIZE];

static void configure(ADC_HandleTypeDef *hadc_in) {
	uint8_t i = 0;
	hadc = hadc_in;
	counter = 0;

	if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) {
		Error_Handler();
	}
	for (i = 0; i < ADCCONVERTEDVALUES_BUFFER_SIZE; i++) {		
		ADC_Converted_Values_Avg[i] = 0;
	}
	if (HAL_ADC_Start_DMA(hadc, ADC_Converted_Values, ADCCONVERTEDVALUES_BUFFER_SIZE) != HAL_OK) {
		Error_Handler();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	T_SYSTEM_UPDATE *update;
	uint8_t i = 0;
	counter++;
	for (i = 0; i < ADCCONVERTEDVALUES_BUFFER_SIZE; i++) {		
		ADC_Converted_Values_Avg[i] = ADC_Converted_Values_Avg[i] + ((ADC_Converted_Values[i] - ADC_Converted_Values_Avg[i]) / counter);
	}
	if (counter == 2000)
	{
		for (i = 0; i < ADCCONVERTEDVALUES_BUFFER_SIZE; i++) {		
			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, 0); /* Allocate memory */
			if (update != NULL)
			{
				update->idx = i;			
				update->int_val = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_Converted_Values_Avg[i]);
				update->val_type = type_int;
				update->source = ADC_READING_EVENT;
				osMailPut(SYS_UPDATE_MAILBOX_ID, update);
				ADC_Converted_Values_Avg[i] = 0;				
			}
		}
		counter = 0;			
	}
}


const struct adc ADC = { 
	.configure = configure,
};