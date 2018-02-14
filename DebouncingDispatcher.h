#pragma once

#include "stm32f1xx_hal.h"
#include "IRQDispatcher.h"
#include "pintoidx.h"
#include "bithelper.h"

struct debounceddispatcher {
	bool(*subscribe)(uint16_t GPIO_Pin, void* handler);	
	bool(*unsubscribe)(uint16_t GPIO_Pin);	
};

static void signalStateChanged(uint16_t);

extern const struct debounceddispatcher DebouncedDispatcher;