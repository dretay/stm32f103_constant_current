#include "adc.h"

static ADC_HandleTypeDef *hadc;
#define ADCCONVERTEDVALUES_BUFFER_SIZE ((uint8_t)    2)
uint16_t  ADC_Converted_Values[ADCCONVERTEDVALUES_BUFFER_SIZE];
#define VDDA_APPLI                     ((uint32_t) 3300)    /* Value of analog voltage supply Vdda (unit: mV) */
#define RANGE_12BITS                   ((uint32_t) 4095)    /* Max digital value with a full range of 12 bits */
#define COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_DATA) ( ((ADC_DATA) * VDDA_APPLI) / RANGE_12BITS)
uint8_t counter;

static void configure(ADC_HandleTypeDef *hadc_in) {
	hadc = hadc_in;
	counter = 0;

	if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) {
		Error_Handler();
	}

	xTaskNotify(adcTaskHandle, 0x01, eSetBits);

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	T_SYSTEM_UPDATE *update;
	uint8_t i = 0;
	counter++;
	if (counter == 100)
	{
		for (i = 0; i < ADCCONVERTEDVALUES_BUFFER_SIZE; i++) {		
			update = osMailAlloc(SYS_UPDATE_MAILBOX_ID, osWaitForever); /* Allocate memory */
			update->idx = i;			
			update->float_val = COMPUTATION_DIGITAL_12BITS_TO_VOLTAGE(ADC_Converted_Values[i])/100;
			update->val_type = type_float;
			update->source = ADC_READING;
			osMailPut(SYS_UPDATE_MAILBOX_ID, update);
		}			
	}
}

void StartAdcTask(void const * argument) {	
	
	// Don't clear bits on entry., Clear all bits on exit., Stores the notified value.
	xTaskNotifyWait(pdFALSE, ULONG_MAX, NULL, osWaitForever);

	if (HAL_ADC_Start_DMA(hadc, ADC_Converted_Values, ADCCONVERTEDVALUES_BUFFER_SIZE) != HAL_OK) {
		Error_Handler();
	}

	while (true)
	{
		osThreadYield();
	}
}
const struct adc ADC = { 
	.configure = configure,
};