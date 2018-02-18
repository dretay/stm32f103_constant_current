#include "DebouncingDispatcher.h"


volatile Button buttons[DD_NUM_BUTTONS];
static uint16_t button_usage_mask = 0;

static int8_t get_unused_idx() {
	uint8_t i = 0;
	for (i = 0; i < DD_NUM_BUTTONS; i++) {
		if (bitRead(button_usage_mask, i) == 0) {
			bitSet(button_usage_mask, i);
			return i;
		}	
	} 
	return -1;
}
static bool idx_valid(int8_t idx) {
	return idx >= 0 && idx < DD_NUM_BUTTONS;
}
static int8_t find_gpio_pin(uint16_t GPIO_Pin) {
	uint8_t i = 0;
	for (i = 0; i < DD_NUM_BUTTONS; i++) {
		if (bitRead(button_usage_mask, i) == 1) {
			if (buttons[i].gpio_pin == GPIO_Pin) {
				return i;
			}			
		}	
	} 
	return -1;
}

void buttonDispatchCallback(void const * argument) {
	uint8_t i =0, old_state = 0, state=0;
	
	for (i = 0; i < DD_NUM_BUTTONS; i++) {
		//Skip items that have been idle
		if (buttons[i].counter == 0)
		{
			continue;			
		}
		//If debounce period has elapsed
		if (--buttons[i].counter == 0)
		{
			state = bitRead(buttons[i].bitmap, DD_STATE);
			old_state = bitRead(buttons[i].bitmap, DD_OLD_STATE);

			if (state != old_state)
			{
				bitToggle(buttons[i].bitmap, DD_OLD_STATE);				
				if (buttons[i].callback != NULL)
				{
					buttons[i].callback(buttons[i].gpio_pin, state);									
				}
			}
		}
	}
}

static bool subscribe(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin_in, void* callback_in) {	
	uint8_t idx;
	if (IRQDispatcher.subscribe(GPIO_Pin_in, &signalStateChanged))
	{
		idx = get_unused_idx();
		if (idx_valid(idx)) {
			buttons[idx].gpio_pin = GPIO_Pin_in;
			buttons[idx].callback = callback_in;
			bitWrite(buttons[idx].bitmap, DD_STATE, HAL_GPIO_ReadPin(GPIOx, GPIO_Pin_in));
		}
		return true;
	}
	return false;
	
}

static bool unsubscribe(uint32_t GPIO_Pin) {
	uint8_t idx;
	if (IRQDispatcher.unsubscribe(GPIO_Pin)) {
		idx = find_gpio_pin(GPIO_Pin);
		if (idx_valid(idx)) {
			buttons[idx].callback = NULL;
			buttons[idx].bitmap = 0;
			buttons[idx].counter = 0;
			buttons[idx].gpio_pin = 0;
			return true;
		}
	}
	return false;
}

static void signalStateChanged(uint32_t GPIO_Pin) {
	uint8_t idx = find_gpio_pin(GPIO_Pin);
	if(idx_valid(idx)) {
		buttons[idx].counter = DD_COUNTER_TOP;
		bitToggle(buttons[idx].bitmap, DD_STATE);
	}
}

static uint8_t get_state(uint32_t GPIO_Pin) {
	uint8_t idx = find_gpio_pin(GPIO_Pin);
	if (idx_valid(idx)) {
		bitRead(buttons[idx].bitmap, DD_STATE);
	}
}

const struct debouncingdispatcher DebouncingDispatcher = { 
	.subscribe = subscribe,
	.unsubscribe = unsubscribe,
	.get_state = get_state,
};
