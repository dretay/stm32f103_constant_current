#include "Adc.h"

static ADC_HandleTypeDef *hadc;

static uint16_t  adc_data[ADC_BUFFER_SIZE];
static uint16_t  adc_data_avg[ADC_BUFFER_SIZE];
static uint16_t readings_cnt;

static inline unsigned short adc_to_voltage(unsigned short reading) {
	static const unsigned long range_12_bits = 4096;
	static const unsigned long vdda = 3300;
	return ((reading * vdda) / range_12_bits);
} 

static void configure(ADC_HandleTypeDef *hadc_in) {
	uint8_t i = 0;
	hadc = hadc_in;
	readings_cnt = 0;

	if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) {
		Error_Handler();
	}
	for (i = 0; i < ADC_BUFFER_SIZE; i++) {		
		adc_data_avg[i] = 0;
	}
	if (HAL_ADC_Start_DMA(hadc, (uint32_t*) adc_data, ADC_BUFFER_SIZE) != HAL_OK) {
		Error_Handler();
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	T_SYSTEM_UPDATE *update;
	uint8_t i = 0;	
	for (i = 0; i < ADC_BUFFER_SIZE; i++) {		
		adc_data_avg[i] = adc_data_avg[i] + ((adc_data[i] - adc_data_avg[i]) / ++readings_cnt);
	}
	if (readings_cnt == 4000)
	{
		for (i = 0; i < ADC_BUFFER_SIZE; i++) {		
			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, 0);
			if (update != NULL)
			{
				update->idx = i;			
				update->int_val = adc_to_voltage(adc_data_avg[i]);
				update->val_type = type_int;
				update->source = ADC_READ_EVENT;
				osMailPut(SYS_UPDATE_MAILBOX_ID, update);
				adc_data_avg[i] = 0;				
			}
		}
		readings_cnt = 0;			
	}
}


const struct adc Adc = { 
	.configure = configure,
};