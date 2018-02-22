#pragma once
#include "stm32f1xx_hal.h"
#include "events.h"
#include "limits.h"

extern osThreadId adcTaskHandle;						         
#define ADC_BUFFER_SIZE ((uint8_t)    2)
struct adc {
	void(*configure)(ADC_HandleTypeDef *hadc_in);			
};

extern const struct adc Adc;