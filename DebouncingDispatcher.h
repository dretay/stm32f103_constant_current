#pragma once

#include "stm32f1xx_hal.h"
#include "IRQDispatcher.h"
#include "bithelper.h"
#include "cmsis_os.h"

#define DD_NUM_BUTTONS 4
#define DD_COUNTER_TOP 5
#define DD_OLD_STATE 0
#define DD_STATE 1
#define DD_ACTIVE 2


typedef struct {
	uint32_t gpio_pin;
	uint8_t bitmap;
	uint8_t counter;	
	void(*callback)(uint16_t gpio_pin, uint8_t state);
} Button;

struct debouncingdispatcher {
	bool(*subscribe)(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, void* handler);	
	bool(*unsubscribe)(uint32_t GPIO_Pin);	
	uint8_t(*get_state)(uint32_t GPIO_Pin);	
};

extern osTimerId debouncingDispatchTimerHandle;
static void signalStateChanged(uint32_t);

extern const struct debouncingdispatcher DebouncingDispatcher;