#pragma once

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "bithelper.h"

#define IRQD_NUM_SUBSCRIPTIONS 4

typedef struct {
	uint32_t gpio_pin;	
	void(*callback)(uint32_t GPIO_Pin);
} Subscription;
struct irqdispatcher {
	bool(*subscribe)(uint32_t GPIO_Pin, void* handler);	
	bool(*unsubscribe)(uint32_t GPIO_Pin);	
};

extern const struct irqdispatcher IRQDispatcher;

