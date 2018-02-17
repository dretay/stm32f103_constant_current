#pragma once

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "events.h"
#include <stdbool.h>
#include "DebouncingDispatcher.h"

#define TOGGLESWITCH_COUNT 1


typedef struct {
	GPIO_TypeDef* pin_bus;
	uint16_t pin_idx;	
} TOGGLE_SWITCH_CONFIG;

struct toggle_switch{
	void(*configure)(TOGGLE_SWITCH_CONFIG* config, uint8_t cnt);			
};
static void handle(uint16_t gpio_pin, uint8_t state);

extern const struct toggle_switch TOGGLE_SWITCH;