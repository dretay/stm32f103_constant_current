#pragma once

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "pintoidx.h"

struct irqdispatcher {
	bool(*subscribe)(uint16_t GPIO_Pin, void* handler);	
	bool(*unsubscribe)(uint16_t GPIO_Pin);	
};

extern const struct irqdispatcher IRQDispatcher;

