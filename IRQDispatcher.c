#include "IRQDispatcher.h"

volatile Subscription subscriptions[IRQD_NUM_SUBSCRIPTIONS];
static uint16_t subscriptions_usage_mask = 0;


static int8_t get_unused_idx() {
	uint8_t i = 0;
	for (i = 0; i < IRQD_NUM_SUBSCRIPTIONS; i++) {
		if (bitRead(subscriptions_usage_mask, i) == 0) {
			bitSet(subscriptions_usage_mask, i);
			return i;
		}	
	} 
	return -1;
}
static bool idx_valid(int8_t idx) {
	return idx >= 0 && idx < IRQD_NUM_SUBSCRIPTIONS;
}
static int8_t find_gpio_pin(uint32_t GPIO_Pin) {
	uint8_t i = 0;
	for (i = 0; i < IRQD_NUM_SUBSCRIPTIONS; i++) {
		if (bitRead(subscriptions_usage_mask, i) == 1) {
			if (subscriptions[i].gpio_pin == GPIO_Pin) {
				return i;
			}			
		}	
	} 
	return -1;
}

static bool subscribe(uint32_t GPIO_Pin, void* handler) {
	int8_t idx = get_unused_idx();
	if (idx_valid(idx)) {	
		subscriptions[idx].callback = handler;
		subscriptions[idx].gpio_pin = GPIO_Pin;
		return true;
	}
	return false;
}
static bool unsubscribe(uint32_t GPIO_Pin) {
	uint8_t idx = find_gpio_pin(GPIO_Pin);
	if (idx_valid(idx)) {
		subscriptions[idx].callback = NULL;		
		subscriptions[idx].gpio_pin = 0;
		return true;
	}
	return false;
}

static void dispatch() {
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint8_t idx = find_gpio_pin(GPIO_Pin);
	if (idx_valid(idx)) {
		subscriptions[idx].callback(GPIO_Pin);
	}
}
const struct irqdispatcher IRQDispatcher = { 
	.subscribe = subscribe,
	.unsubscribe = unsubscribe
};
