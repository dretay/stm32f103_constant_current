#include "IRQDispatcher.h"

static void(*subscriptions[16])();

static bool subscribe(uint16_t GPIO_Pin, void* handler) {
	int8_t idx = pintoidx(GPIO_Pin);
	if (idx > 0) {
		if (subscriptions[idx] == NULL) {
			subscriptions[idx] = handler;
			return true;
		}
	}
	return false;
}
static bool unsubscribe(uint16_t GPIO_Pin) {
	int8_t idx = pintoidx(GPIO_Pin);
	if (idx > 0) {
		if (subscriptions[idx] != NULL) {
			subscriptions[idx] = NULL;
			return true;
		}
	}
	return false;
}

static void dispatch() {
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	int8_t idx = pintoidx(GPIO_Pin);
	if (idx > 0)
	{
		if (subscriptions[idx] != NULL)
		{
			subscriptions[idx](GPIO_Pin);
		}
	}
}
const struct irqdispatcher IRQDispatcher = { 
	.subscribe = subscribe,
	.unsubscribe = unsubscribe
};
